package com.storm.discordrpc;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import androidx.fragment.app.Fragment;

// фрагмент для управления статусом Discord RPC
public class StatusFragment extends Fragment {
    private String clientId;
    private EditText statusEditText;
    private EditText detailsEditText;
    private EditText stateEditText;
    private Button startButton;
    private Button stopButton;
    private Switch autoStartSwitch;
    private TextView clientIdDisplay;
    private TextView rpcStatusText;
    
    private StormNotificationManager notificationManager;
    private boolean isRpcRunning = false;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        Log.d("StormRPC", "StatusFragment onCreateView started");
        
        View view = inflater.inflate(R.layout.fragment_status, container, false);
        
        // получаем client ID из аргументов или preferences
        Bundle args = getArguments();
        if (args != null) {
            clientId = args.getString("client_id");
        }
        
        if (clientId == null) {
            // пробуем получить из preferences
            try {
                android.content.SharedPreferences prefs = requireContext().getSharedPreferences("storm_prefs", 0);
                clientId = prefs.getString("client_id", null);
            } catch (Exception e) {
                Log.e("StormRPC", "Error getting client ID from prefs: " + e.getMessage());
            }
        }
        
        initializeViews(view);
        setupClickListeners();
        updateClientIdDisplay();
        updateRpcStatus();
        
        Log.d("StormRPC", "StatusFragment onCreateView completed");
        return view;
    }

    private void initializeViews(View view) {
        try {
            Log.d("StormRPC", "Initializing views in StatusFragment");
            
            statusEditText = view.findViewById(R.id.statusEditText);
            detailsEditText = view.findViewById(R.id.detailsEditText);
            stateEditText = view.findViewById(R.id.stateEditText);
            startButton = view.findViewById(R.id.startButton);
            stopButton = view.findViewById(R.id.stopButton);
            autoStartSwitch = view.findViewById(R.id.autoStartSwitch);
            clientIdDisplay = view.findViewById(R.id.clientIdDisplay);
            rpcStatusText = view.findViewById(R.id.rpcStatusText);
            
            // проверка что все view найдены
            if (statusEditText == null || detailsEditText == null || stateEditText == null ||
                startButton == null || stopButton == null || autoStartSwitch == null ||
                clientIdDisplay == null || rpcStatusText == null) {
                Log.e("StormRPC", "Some views are null in StatusFragment!");
                Toast.makeText(requireContext(), "Ошибка загрузки интерфейса", Toast.LENGTH_LONG).show();
                return;
            }
            
            // изначально кнопка остановки неактивна
            stopButton.setEnabled(false);
            stopButton.setAlpha(0.5f);
            
            Log.d("StormRPC", "All views initialized successfully in StatusFragment");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in initializeViews: " + e.getMessage(), e);
            Toast.makeText(requireContext(), "Ошибка инициализации: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void setupClickListeners() {
        try {
            Log.d("StormRPC", "Setting up click listeners in StatusFragment");
            
            startButton.setOnClickListener(v -> startDiscordRPC());
            stopButton.setOnClickListener(v -> stopDiscordRPC());
            
            Log.d("StormRPC", "Click listeners setup completed in StatusFragment");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in setupClickListeners: " + e.getMessage(), e);
            Toast.makeText(requireContext(), "Ошибка настройки кнопок: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void updateClientIdDisplay() {
        if (clientIdDisplay != null && clientId != null) {
            clientIdDisplay.setText("Client ID: " + clientId);
            Log.d("StormRPC", "Client ID displayed: " + clientId);
        } else if (clientIdDisplay != null) {
            clientIdDisplay.setText("Client ID: Не установлен");
            Log.w("StormRPC", "No client ID to display");
        }
    }

    private void updateRpcStatus() {
        if (rpcStatusText != null) {
            if (isRpcRunning) {
                rpcStatusText.setText("✅ RPC запущен");
                rpcStatusText.setTextColor(0xFF4CAF50); // зеленый
            } else {
                rpcStatusText.setText("❌ RPC не запущен");
                rpcStatusText.setTextColor(0xFFE94560); // красный
            }
        }
    }

    private void startDiscordRPC() {
        try {
            Log.d("StormRPC", "startDiscordRPC called in StatusFragment");
            
            if (clientId == null || clientId.isEmpty()) {
                Log.e("StormRPC", "No client ID available in StatusFragment");
                Toast.makeText(requireContext(), "Ошибка: Client ID не установлен", Toast.LENGTH_SHORT).show();
                return;
            }
            
            if (statusEditText == null || detailsEditText == null || stateEditText == null) {
                Log.e("StormRPC", "Some EditText views are null in startDiscordRPC");
                Toast.makeText(requireContext(), "Ошибка: поля ввода не найдены", Toast.LENGTH_SHORT).show();
                return;
            }
            
            String status = statusEditText.getText().toString().trim();
            String details = detailsEditText.getText().toString().trim();
            String state = stateEditText.getText().toString().trim();

            Log.d("StormRPC", "Status: " + status);
            Log.d("StormRPC", "Client ID: " + clientId);

            if (status.isEmpty()) {
                Toast.makeText(requireContext(), "введите статус", Toast.LENGTH_SHORT).show();
                return;
            }

            // запуск сервиса
            Intent serviceIntent = new Intent(requireContext(), DiscordRPCService.class);
            serviceIntent.putExtra("client_id", clientId);
            serviceIntent.putExtra("status", status);
            serviceIntent.putExtra("details", details);
            serviceIntent.putExtra("state", state);

            requireContext().startForegroundService(serviceIntent);

            // обновление ui
            if (startButton != null && stopButton != null) {
                startButton.setEnabled(false);
                startButton.setAlpha(0.5f);
                stopButton.setEnabled(true);
                stopButton.setAlpha(1.0f);
            }

            isRpcRunning = true;
            updateRpcStatus();

            // отправка уведомления о запуске
            if (notificationManager != null) {
                notificationManager.sendStatusNotification(true);
            }

            Toast.makeText(requireContext(), "discord rpc запущен", Toast.LENGTH_SHORT).show();
            Log.d("StormRPC", "Discord RPC started successfully");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in startDiscordRPC: " + e.getMessage(), e);
            Toast.makeText(requireContext(), "Ошибка запуска: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void stopDiscordRPC() {
        try {
            Log.d("StormRPC", "stopDiscordRPC called in StatusFragment");
            
            Intent serviceIntent = new Intent(requireContext(), DiscordRPCService.class);
            requireContext().stopService(serviceIntent);
            
            isRpcRunning = false;
            updateRpcStatus();
            
            // отправка уведомления об остановке
            if (notificationManager != null) {
                notificationManager.sendStatusNotification(false);
            }
            
            Toast.makeText(requireContext(), "discord rpc остановлен", Toast.LENGTH_SHORT).show();
            Log.d("StormRPC", "Discord RPC stopped successfully");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in stopDiscordRPC: " + e.getMessage(), e);
            Toast.makeText(requireContext(), "Ошибка остановки: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    public void setNotificationManager(StormNotificationManager manager) {
        this.notificationManager = manager;
    }

    public void setClientId(String clientId) {
        this.clientId = clientId;
        updateClientIdDisplay();
    }
}
