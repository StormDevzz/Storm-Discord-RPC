package com.storm.discordrpc;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import androidx.core.app.NotificationCompat;

// фоновый сервис для работы с discord rpc
public class DiscordRPCService extends Service {
    private static final String CHANNEL_ID = "DiscordRPCChannel";
    private static final int NOTIFICATION_ID = 1;
    
    private DiscordRPCManager discordRPCManager;
    private boolean isRunning = false;

    @Override
    public void onCreate() {
        super.onCreate();
        createNotificationChannel();
        discordRPCManager = new DiscordRPCManager();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (intent != null && !isRunning) {
            // получаем данные из интента
            String status = intent.getStringExtra("status");
            String details = intent.getStringExtra("details");
            String state = intent.getStringExtra("state");

            // запускаем сервис в foreground
            startForeground(NOTIFICATION_ID, createNotification());
            
            // инициализируем discord rpc
            discordRPCManager.initialize("1234567890123456789"); // твой discord app id
            discordRPCManager.updatePresence(status, details, state);
            isRunning = true;
        }
        
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (discordRPCManager != null && isRunning) {
            discordRPCManager.shutdown();
            isRunning = false;
        }
    }

    // создаем канал уведомлений для android 8+
    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel serviceChannel = new NotificationChannel(
                    CHANNEL_ID,
                    "Discord RPC Service",
                    NotificationManager.IMPORTANCE_DEFAULT
            );
            serviceChannel.setDescription("управляет discord rich presence");
            
            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(serviceChannel);
        }
    }

    // создаем уведомление для foreground сервиса
    private Notification createNotification() {
        return new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle("Storm Discord RPC")
                .setContentText("discord rich presence активен")
                .setSmallIcon(R.drawable.ic_notification)
                .setOngoing(true)
                .setColor(0xFF533483) // фиолетовый цвет
                .build();
    }
}
