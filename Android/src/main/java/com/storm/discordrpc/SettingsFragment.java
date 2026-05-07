package com.storm.discordrpc;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import androidx.fragment.app.Fragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

// фрагмент настроек приложения
public class SettingsFragment extends Fragment {
    private Switch mainNotificationsSwitch;
    private RecyclerView notificationSettingsList;
    private NotificationSettingsAdapter adapter;
    private StormNotificationManager notificationManager;
    private TextView currentClientId;
    private Button changeClientIdButton;
    private Button testNotificationButton;
    private Button backButton;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        Log.d("StormRPC", "SettingsFragment onCreateView started");
        
        View view = inflater.inflate(R.layout.fragment_settings, container, false);
        
        initializeViews(view);
        setupClickListeners();
        loadSettings();
        updateClientIdDisplay();
        
        Log.d("StormRPC", "SettingsFragment onCreateView completed");
        return view;
    }

    private void initializeViews(View view) {
        try {
            Log.d("StormRPC", "Initializing views in SettingsFragment");
            
            mainNotificationsSwitch = view.findViewById(R.id.mainNotificationsSwitch);
            notificationSettingsList = view.findViewById(R.id.notificationSettingsList);
            currentClientId = view.findViewById(R.id.currentClientId);
            changeClientIdButton = view.findViewById(R.id.changeClientIdButton);
            testNotificationButton = view.findViewById(R.id.testNotificationButton);
            backButton = view.findViewById(R.id.backButton);
            
            // настройка RecyclerView
            if (notificationSettingsList != null) {
                notificationSettingsList.setLayoutManager(new LinearLayoutManager(requireContext()));
            }
            
            Log.d("StormRPC", "All views initialized successfully in SettingsFragment");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in initializeViews: " + e.getMessage(), e);
            Toast.makeText(requireContext(), "Ошибка инициализации: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void setupClickListeners() {
        try {
            Log.d("StormRPC", "Setting up click listeners in SettingsFragment");
            
            if (mainNotificationsSwitch != null) {
                mainNotificationsSwitch.setOnCheckedChangeListener((buttonView, isChecked) -> {
                    if (notificationManager != null) {
                        notificationManager.setNotificationsEnabled(isChecked);
                        if (adapter != null) {
                            adapter.notifyDataSetChanged();
                        }
                        
                        if (isChecked && !notificationManager.hasNotificationPermission()) {
                            // запрос разрешений
                            notificationManager.requestNotificationPermission();
                            Toast.makeText(requireContext(), "Включите уведомления в настройках", Toast.LENGTH_LONG).show();
                        } else {
                            Toast.makeText(requireContext(), isChecked ? "Уведомления включены" : "Уведомления отключены", 
                                Toast.LENGTH_SHORT).show();
                        }
                    }
                });
            }

            // кнопка тестового уведомления
            if (testNotificationButton != null) {
                testNotificationButton.setOnClickListener(v -> {
                    if (notificationManager != null && notificationManager.hasNotificationPermission()) {
                        notificationManager.sendMotivationNotification();
                        Toast.makeText(requireContext(), "Тестовое уведомление отправлено", Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(requireContext(), "Сначала включите уведомления", Toast.LENGTH_SHORT).show();
                    }
                });
            }

            // кнопка изменения Client ID
            if (changeClientIdButton != null) {
                changeClientIdButton.setOnClickListener(v -> {
                    Intent intent = new Intent(requireContext(), ClientIDActivity.class);
                    startActivity(intent);
                    if (getActivity() != null) {
                        getActivity().finish();
                    }
                });
            }

            // кнопка возврата
            if (backButton != null) {
                backButton.setOnClickListener(v -> {
                    if (getActivity() != null) {
                        getActivity().finish();
                    }
                });
            }
            
            Log.d("StormRPC", "Click listeners setup completed in SettingsFragment");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in setupClickListeners: " + e.getMessage(), e);
            Toast.makeText(requireContext(), "Ошибка настройки кнопок: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void loadSettings() {
        try {
            Log.d("StormRPC", "Loading settings in SettingsFragment");
            
            if (notificationManager != null) {
                if (mainNotificationsSwitch != null) {
                    mainNotificationsSwitch.setChecked(notificationManager.isNotificationsEnabled());
                }
                
                // создание адаптера с настройками уведомлений
                adapter = new NotificationSettingsAdapter(requireContext(), notificationManager);
                if (notificationSettingsList != null) {
                    notificationSettingsList.setAdapter(adapter);
                }
            }
            
            Log.d("StormRPC", "Settings loaded successfully in SettingsFragment");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in loadSettings: " + e.getMessage(), e);
            Toast.makeText(requireContext(), "Ошибка загрузки настроек: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    private void updateClientIdDisplay() {
        try {
            if (currentClientId != null) {
                SharedPreferences prefs = requireContext().getSharedPreferences("storm_prefs", 0);
                String clientId = prefs.getString("client_id", null);
                
                if (clientId != null && !clientId.isEmpty()) {
                    currentClientId.setText("Текущий Client ID: " + clientId);
                    Log.d("StormRPC", "Client ID displayed in settings: " + clientId);
                } else {
                    currentClientId.setText("Текущий Client ID: Не установлен");
                    Log.w("StormRPC", "No client ID to display in settings");
                }
            }
        } catch (Exception e) {
            Log.e("StormRPC", "Error updating client ID display: " + e.getMessage(), e);
        }
    }

    public void setNotificationManager(StormNotificationManager manager) {
        this.notificationManager = manager;
        loadSettings();
    }
}
