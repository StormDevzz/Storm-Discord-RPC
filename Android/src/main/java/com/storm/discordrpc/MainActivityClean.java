package com.storm.discordrpc;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;

// полностью новый MainActivity с простым интерфейсом
public class MainActivityClean extends AppCompatActivity {
    private String clientId;
    private EditText statusEditText;
    private EditText detailsEditText;
    private EditText stateEditText;
    private Button startButton;
    private Button stopButton;
    private Switch autoStartSwitch;
    private TextView clientIdDisplay;
    private TextView rpcStatusText;
    private Button settingsButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        try {
            Log.d("StormRPC", "MainActivityClean onCreate started");
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_main_new);
            Log.d("StormRPC", "MainActivityClean layout set");

            // получаем client id
            clientId = getIntent().getStringExtra("client_id");
            Log.d("StormRPC", "Client ID received: " + (clientId != null ? "YES" : "NO"));
            
            if (clientId == null) {
                // пробуем получить из preferences
                try {
                    android.content.SharedPreferences prefs = getSharedPreferences("storm_prefs", 0);
                    clientId = prefs.getString("client_id", null);
                    Log.d("StormRPC", "Client ID from prefs: " + (clientId != null ? "YES" : "NO"));
                } catch (Exception e) {
                    Log.e("StormRPC", "Error getting client ID from prefs: " + e.getMessage());
                }
            }
            
            // инициализация view
            initializeViews();
            updateClientIdDisplay();
            setupClickListeners();
            
            if (clientId == null) {
                // если client id нет, перенаправляем на экран ввода
                Log.d("StormRPC", "No client ID, redirecting to ClientIDActivity");
                Intent intent = new Intent(this, ClientIDActivity.class);
                startActivity(intent);
                finish();
                return;
            }

            Log.d("StormRPC", "MainActivityClean onCreate completed successfully");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in MainActivityClean onCreate: " + e.getMessage(), e);
            Toast.makeText(this, "Ошибка загрузки главного экрана: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void initializeViews() {
        try {
            Log.d("StormRPC", "Initializing views in MainActivityClean");
            
            statusEditText = findViewById(R.id.statusEditText);
            detailsEditText = findViewById(R.id.detailsEditText);
            stateEditText = findViewById(R.id.stateEditText);
            startButton = findViewById(R.id.startButton);
            stopButton = findViewById(R.id.stopButton);
            autoStartSwitch = findViewById(R.id.autoStartSwitch);
            clientIdDisplay = findViewById(R.id.clientIdDisplay);
            rpcStatusText = findViewById(R.id.rpcStatusText);
            settingsButton = findViewById(R.id.settingsButton);
            
            // проверка что все view найдены
            if (statusEditText == null || detailsEditText == null || stateEditText == null ||
                startButton == null || stopButton == null || autoStartSwitch == null ||
                clientIdDisplay == null || rpcStatusText == null || settingsButton == null) {
                Log.e("StormRPC", "Some views are null in MainActivityClean!");
                Toast.makeText(this, "Ошибка загрузки интерфейса", Toast.LENGTH_LONG).show();
                return;
            }
            
            // изначально кнопка остановки неактивна
            stopButton.setEnabled(false);
            stopButton.setAlpha(0.5f);
            
            Log.d("StormRPC", "All views initialized successfully in MainActivityClean");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in initializeViews: " + e.getMessage(), e);
            Toast.makeText(this, "Ошибка инициализации интерфейса: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void updateClientIdDisplay() {
        try {
            if (clientIdDisplay != null && clientId != null) {
                clientIdDisplay.setText("Client ID: " + clientId);
                Log.d("StormRPC", "Client ID displayed: " + clientId);
            } else if (clientIdDisplay != null) {
                clientIdDisplay.setText("Client ID: Не установлен");
                Log.w("StormRPC", "No client ID to display");
            }
        } catch (Exception e) {
            Log.e("StormRPC", "Error updating client ID display: " + e.getMessage(), e);
        }
    }

    private void setupClickListeners() {
        try {
            Log.d("StormRPC", "Setting up click listeners in MainActivityClean");
            
            if (startButton != null) {
                startButton.setOnClickListener(v -> startDiscordRPC());
                Log.d("StormRPC", "startButton listener set");
            }
            
            if (stopButton != null) {
                stopButton.setOnClickListener(v -> stopDiscordRPC());
                Log.d("StormRPC", "stopButton listener set");
            }

            if (settingsButton != null) {
                settingsButton.setOnClickListener(v -> openSettings());
                Log.d("StormRPC", "settingsButton listener set");
            }
            
            Log.d("StormRPC", "Click listeners setup completed in MainActivityClean");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in setupClickListeners: " + e.getMessage(), e);
            Toast.makeText(this, "Ошибка настройки кнопок: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void startDiscordRPC() {
        try {
            Log.d("StormRPC", "startDiscordRPC called in MainActivityClean");
            
            if (statusEditText == null || detailsEditText == null || stateEditText == null) {
                Log.e("StormRPC", "Some EditText views are null in startDiscordRPC");
                Toast.makeText(this, "Ошибка: поля ввода не найдены", Toast.LENGTH_SHORT).show();
                return;
            }
            
            if (clientId == null || clientId.isEmpty()) {
                Log.e("StormRPC", "No client ID available in MainActivityClean");
                Toast.makeText(this, "Ошибка: Client ID не установлен", Toast.LENGTH_SHORT).show();
                return;
            }
            
            String status = statusEditText.getText().toString().trim();
            String details = detailsEditText.getText().toString().trim();
            String state = stateEditText.getText().toString().trim();

            Log.d("StormRPC", "Status: " + status);
            Log.d("StormRPC", "Client ID: " + clientId);

            if (status.isEmpty()) {
                Toast.makeText(this, "введите статус", Toast.LENGTH_SHORT).show();
                return;
            }

            // запуск сервиса
            Intent serviceIntent = new Intent(this, DiscordRPCService.class);
            serviceIntent.putExtra("client_id", clientId);
            serviceIntent.putExtra("status", status);
            serviceIntent.putExtra("details", details);
            serviceIntent.putExtra("state", state);

            startForegroundService(serviceIntent);

            // обновление ui
            if (startButton != null && stopButton != null) {
                startButton.setEnabled(false);
                startButton.setAlpha(0.5f);
                stopButton.setEnabled(true);
                stopButton.setAlpha(1.0f);
            }

            updateRpcStatus(true);

            Toast.makeText(this, "discord rpc запущен", Toast.LENGTH_SHORT).show();
            Log.d("StormRPC", "Discord RPC started successfully");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in startDiscordRPC: " + e.getMessage(), e);
            Toast.makeText(this, "Ошибка запуска: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void stopDiscordRPC() {
        try {
            Log.d("StormRPC", "stopDiscordRPC called in MainActivityClean");
            
            Intent serviceIntent = new Intent(this, DiscordRPCService.class);
            stopService(serviceIntent);
            
            updateRpcStatus(false);
            
            Toast.makeText(this, "discord rpc остановлен", Toast.LENGTH_SHORT).show();
            Log.d("StormRPC", "Discord RPC stopped successfully");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in stopDiscordRPC: " + e.getMessage(), e);
            Toast.makeText(this, "Ошибка остановки: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void updateRpcStatus(boolean isRunning) {
        try {
            if (rpcStatusText != null) {
                if (isRunning) {
                    rpcStatusText.setText("✅ RPC запущен");
                    rpcStatusText.setTextColor(0xFF4CAF50); // зеленый
                } else {
                    rpcStatusText.setText("❌ RPC не запущен");
                    rpcStatusText.setTextColor(0xFFE94560); // красный
                }
            }
        } catch (Exception e) {
            Log.e("StormRPC", "Error updating RPC status: " + e.getMessage(), e);
        }
    }

    private void openSettings() {
        try {
            Log.d("StormRPC", "Opening settings");
            Intent intent = new Intent(this, SettingsActivity.class);
            startActivity(intent);
        } catch (Exception e) {
            Log.e("StormRPC", "Error opening settings: " + e.getMessage(), e);
            Toast.makeText(this, "Ошибка открытия настроек: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }
}
