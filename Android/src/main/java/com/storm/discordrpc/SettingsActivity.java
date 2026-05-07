package com.storm.discordrpc;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.storm.discordrpc.StormNotificationManager;

// актив настроек приложения
public class SettingsActivity extends AppCompatActivity {
    private Switch mainNotificationsSwitch;
    private RecyclerView notificationSettingsList;
    private NotificationSettingsAdapter adapter;
    private StormNotificationManager notificationManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);

        notificationManager = new StormNotificationManager(this);
        initializeViews();
        setupClickListeners();
        loadSettings();
    }

    private void initializeViews() {
        mainNotificationsSwitch = findViewById(R.id.mainNotificationsSwitch);
        notificationSettingsList = findViewById(R.id.notificationSettingsList);
        
        // настройка RecyclerView
        notificationSettingsList.setLayoutManager(new LinearLayoutManager(this));
        
        // создание адаптера с настройками уведомлений
        adapter = new NotificationSettingsAdapter(this, notificationManager);
        notificationSettingsList.setAdapter(adapter);
    }

    private void setupClickListeners() {
        mainNotificationsSwitch.setOnCheckedChangeListener((buttonView, isChecked) -> {
            notificationManager.setNotificationsEnabled(isChecked);
            adapter.notifyDataSetChanged();
            
            if (isChecked && !notificationManager.hasNotificationPermission()) {
                // запрос разрешений
                notificationManager.requestNotificationPermission();
                Toast.makeText(this, "Включите уведомления в настройках", Toast.LENGTH_LONG).show();
            } else {
                Toast.makeText(this, isChecked ? "Уведомления включены" : "Уведомления отключены", 
                    Toast.LENGTH_SHORT).show();
            }
        });

        // кнопка тестового уведомления
        Button testNotificationButton = findViewById(R.id.testNotificationButton);
        testNotificationButton.setOnClickListener(v -> {
            if (notificationManager.hasNotificationPermission()) {
                notificationManager.sendMotivationNotification();
                Toast.makeText(this, "Тестовое уведомление отправлено", Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(this, "Сначала включите уведомления", Toast.LENGTH_SHORT).show();
            }
        });

        // кнопка возврата
        Button backButton = findViewById(R.id.backButton);
        backButton.setOnClickListener(v -> finish());
    }

    private void loadSettings() {
        mainNotificationsSwitch.setChecked(notificationManager.isNotificationsEnabled());
        adapter.notifyDataSetChanged();
    }

    @Override
    protected void onResume() {
        super.onResume();
        // обновляем состояние переключателя при возврате
        mainNotificationsSwitch.setChecked(notificationManager.isNotificationsEnabled());
    }
}
