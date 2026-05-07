package com.storm.discordrpc;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.ProgressBar;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import android.view.View;
import android.text.Editable;
import android.text.TextWatcher;

// актив для ввода и проверки client id
public class ClientIDActivity extends AppCompatActivity {
    private EditText clientIdEditText;
    private Button validateButton;
    private Button continueButton;
    private TextView statusText;
    private TextView appNameText;
    private ProgressBar progressBar;
    
    private PythonBridge pythonBridge;
    private com.storm.discordrpc.StormNotificationManager notificationManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        try {
            Log.d("StormRPC", "ClientIDActivity onCreate started");
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_client_id);
            Log.d("StormRPC", "Layout set successfully");

            initializeViews();
            Log.d("StormRPC", "Views initialized");
            
            setupClickListeners();
            Log.d("StormRPC", "Click listeners setup");
            
            setupTextWatcher();
            Log.d("StormRPC", "Text watcher setup");
            
            initializePythonBridge();
            Log.d("StormRPC", "Python bridge initialized");
            
            initializeNotificationManager();
            Log.d("StormRPC", "Notification manager initialized");
            
            // показываем справку
            showHelpText();
            Log.d("StormRPC", "ClientIDActivity onCreate completed successfully");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in ClientIDActivity onCreate: " + e.getMessage(), e);
            Toast.makeText(this, "Ошибка загрузки приложения: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void initializeViews() {
        clientIdEditText = findViewById(R.id.clientIdEditText);
        validateButton = findViewById(R.id.validateButton);
        continueButton = findViewById(R.id.continueButton);
        statusText = findViewById(R.id.statusText);
        appNameText = findViewById(R.id.appNameText);
        progressBar = findViewById(R.id.progressBar);
        
        // изначально кнопка продолжения неактивна
        continueButton.setEnabled(false);
        continueButton.setAlpha(0.5f);
    }

    private void setupClickListeners() {
        validateButton.setOnClickListener(v -> validateClientID());
        continueButton.setOnClickListener(v -> continueToMain());
        
        // кнопка справки
        Button helpButton = findViewById(R.id.helpButton);
        helpButton.setOnClickListener(v -> showDetailedHelp());
        
        // обработчики кликов на ссылки
        setupLinkHandlers();
    }

    private void setupTextWatcher() {
        clientIdEditText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {}

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                // сбрасываем статус при изменении текста
                resetValidationStatus();
            }

            @Override
            public void afterTextChanged(Editable s) {
                // проверяем базовый формат
                String clientId = s.toString().trim();
                boolean basicValid = isBasicFormatValid(clientId);
                
                validateButton.setEnabled(basicValid);
                validateButton.setAlpha(basicValid ? 1.0f : 0.5f);
            }
        });
    }

    private void initializePythonBridge() {
        try {
            Log.d("StormRPC", "Initializing Python bridge");
            pythonBridge = PythonBridge.getInstance(this);
            pythonBridge.initialize();
            Log.d("StormRPC", "Python bridge initialized successfully");
        } catch (Exception e) {
            Log.e("StormRPC", "Error initializing Python bridge: " + e.getMessage(), e);
            pythonBridge = null; // не критично, продолжаем без Python
        }
    }
    
    private void initializeNotificationManager() {
        try {
            Log.d("StormRPC", "Initializing notification manager");
            notificationManager = new StormNotificationManager(this);
            
            // проверка разрешений
            if (!notificationManager.hasNotificationPermission()) {
                // можно показать диалог о необходимости разрешений
                Toast.makeText(this, "Рекомендуется включить уведомления в настройках", Toast.LENGTH_LONG).show();
            }
            Log.d("StormRPC", "Notification manager initialized successfully");
        } catch (Exception e) {
            Log.e("StormRPC", "Error initializing notification manager: " + e.getMessage(), e);
            notificationManager = null; // не критично, продолжаем без уведомлений
        }
    }

    private void showHelpText() {
        // helpText больше не используется, так как инструкция встроена в layout
    }

    private void showDetailedHelp() {
        Intent intent = new Intent(this, HelpActivity.class);
        startActivity(intent);
    }
    
    private void setupLinkHandlers() {
        // здесь можно добавить обработчики для прямых ссылок
        // пока что просто показываем toast с информацией
        // в реальном приложении здесь можно открыть браузер
        
        // Discord Developer Portal
        findViewById(R.id.discordPortalLink).setOnClickListener(v -> {
            openUrl("https://discord.com/developers/applications");
        });
        
        // Rich Presence документация
        findViewById(R.id.richPresenceDocsLink).setOnClickListener(v -> {
            openUrl("https://discord.com/developers/docs/rich-presence");
        });
    }
    
    private void openUrl(String url) {
        try {
            Intent intent = new Intent(Intent.ACTION_VIEW, android.net.Uri.parse(url));
            startActivity(intent);
        } catch (Exception e) {
            Toast.makeText(this, "Не удалось открыть ссылку", Toast.LENGTH_SHORT).show();
        }
    }

    private boolean isBasicFormatValid(String clientId) {
        if (clientId == null || clientId.isEmpty()) {
            return false;
        }
        
        // базовая проверка: только цифры и правильная длина
        return clientId.matches("\\d+") && clientId.length() == 18;
    }

    private void validateClientID() {
        String clientId = clientIdEditText.getText().toString().trim();
        
        if (!isBasicFormatValid(clientId)) {
            showError("Client ID должен содержать ровно 18 цифр");
            return;
        }

        showValidationProgress(true);
        
        // отправляем в python для полной проверки
        pythonBridge.sendEventToPython("validate_client_id", 
            java.util.Map.of("client_id", clientId));
    }

    private void showValidationProgress(boolean show) {
        progressBar.setVisibility(show ? View.VISIBLE : View.GONE);
        validateButton.setEnabled(!show);
        validateButton.setAlpha(show ? 0.5f : 1.0f);
        
        if (show) {
            statusText.setText("Проверка Client ID...");
            statusText.setTextColor(getResources().getColor(android.R.color.holo_orange_light));
        }
    }

    private void showError(String message) {
        statusText.setText("❌ " + message);
        statusText.setTextColor(getResources().getColor(android.R.color.holo_red_dark));
        appNameText.setText("");
        continueButton.setEnabled(false);
        continueButton.setAlpha(0.5f);
    }

    private void showSuccess(String appName) {
        try {
            statusText.setText("✅ Client ID действителен");
            statusText.setTextColor(getResources().getColor(android.R.color.holo_green_dark));
            
            if (appName != null && !appName.isEmpty()) {
                appNameText.setText("📱 Приложение: " + appName);
                appNameText.setVisibility(View.VISIBLE);
            } else {
                appNameText.setText("");
                appNameText.setVisibility(View.GONE);
            }
            
            continueButton.setEnabled(true);
            continueButton.setAlpha(1.0f);
            
            // отправляем уведомление об успехе
            if (notificationManager != null) {
                notificationManager.sendSuccessNotification();
            }
        } catch (Exception e) {
            Log.e("StormRPC", "Error in showSuccess: " + e.getMessage(), e);
        }
    }

    private void resetValidationStatus() {
        statusText.setText("");
        appNameText.setText("");
        continueButton.setEnabled(false);
        continueButton.setAlpha(0.5f);
    }

    private void continueToMain() {
        String clientId = clientIdEditText.getText().toString().trim();
        
        // сохраняем client id
        saveClientID(clientId);
        
        // переходим к главному экрану
        Intent intent = new Intent(this, MainActivity.class);
        intent.putExtra("client_id", clientId);
        startActivity(intent);
        finish();
    }

    private void saveClientID(String clientId) {
        // сохраняем в preferences
        getSharedPreferences("storm_discord_rpc", MODE_PRIVATE)
            .edit()
            .putString("client_id", clientId)
            .apply();
    }

    private String loadSavedClientID() {
        return getSharedPreferences("storm_discord_rpc", MODE_PRIVATE)
            .getString("client_id", "");
    }

    // обработчик событий из python
    public void onPythonValidationResult(String clientId, boolean valid, String message, String appName) {
        runOnUiThread(() -> {
            showValidationProgress(false);
            
            if (valid) {
                showSuccess(appName);
            } else {
                showError(message);
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (pythonBridge != null) {
            pythonBridge.shutdown();
        }
    }
}
