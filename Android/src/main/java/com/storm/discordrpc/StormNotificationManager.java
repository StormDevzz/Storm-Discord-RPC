package com.storm.discordrpc;

import android.app.NotificationChannel;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Build;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

// менеджер уведомлений для storm discord rpc
public class StormNotificationManager {
    private static final String CHANNEL_ID = "storm_discord_rpc_channel";
    private static final String CHANNEL_NAME = "Storm Discord RPC";
    private static final String CHANNEL_DESC = "Уведомления для Discord Rich Presence";
    
    private Context context;
    private android.app.NotificationManager systemNotificationManager;
    private SharedPreferences preferences;
    
    // типы уведомлений
    public enum NotificationType {
        STATUS_ENABLED("Твой статус успешно включен! ⚡"),
        STATUS_DISABLED("Статус отключен. Включи снова! 🔥"),
        MOTIVATION_DAILY("Скорее зайди в приложение! 🚀"),
        MOTIVATION_FUN("Твой Discord ждет тебя! ⚡"),
        ERROR_PROBLEM("Что-то пошло не так... 😢"),
        SUCCESS_SETUP("Отличная настройка! 🎉"),
        REMINDER_RPC("Не забудь про Rich Presence! 📱");
        
        private final String defaultMessage;
        
        NotificationType(String defaultMessage) {
            this.defaultMessage = defaultMessage;
        }
        
        public String getDefaultMessage() {
            return defaultMessage;
        }
    }
    
    public StormNotificationManager(Context context) {
        this.context = context;
        this.systemNotificationManager = (android.app.NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        this.preferences = context.getSharedPreferences("storm_notifications", Context.MODE_PRIVATE);
        
        createNotificationChannel();
    }
    
    // создание канала уведомлений
    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                CHANNEL_ID,
                CHANNEL_NAME,
                android.app.NotificationManager.IMPORTANCE_DEFAULT
            );
            channel.setDescription(CHANNEL_DESC);
            channel.enableLights(true);
            channel.setLightColor(0xFF533483);
            channel.enableVibration(true);
            
            systemNotificationManager.createNotificationChannel(channel);
        }
    }
    
    // проверка разрешений на уведомления
    public boolean hasNotificationPermission() {
        return NotificationManagerCompat.from(context).areNotificationsEnabled();
    }
    
    // запрос разрешений на уведомления
    public void requestNotificationPermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            Intent intent = new Intent(android.provider.Settings.ACTION_APP_NOTIFICATION_SETTINGS);
            intent.putExtra(android.provider.Settings.EXTRA_APP_PACKAGE, context.getPackageName());
            context.startActivity(intent);
        }
    }
    
    // отправка уведомления
    public void sendNotification(NotificationType type, String customMessage) {
        if (!isNotificationsEnabled(type)) {
            return;
        }
        
        String message = customMessage != null ? customMessage : type.getDefaultMessage();
        
        // intent для открытия приложения
        Intent intent = new Intent(context, com.storm.discordrpc.ClientIDActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        
        PendingIntent pendingIntent = PendingIntent.getActivity(
            context, 
            0, 
            intent, 
            PendingIntent.FLAG_UPDATE_CURRENT | PendingIntent.FLAG_IMMUTABLE
        );
        
        // создание уведомления
        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, CHANNEL_ID)
            .setSmallIcon(R.drawable.ic_notification)
            .setContentTitle(getTitleForType(type))
            .setContentText(message)
            .setPriority(NotificationCompat.PRIORITY_DEFAULT)
            .setContentIntent(pendingIntent)
            .setAutoCancel(true)
            .setColor(0xFF533483)
            .setLights(0xFF533483, 1000, 500)
            .setVibrate(new long[]{0, 300, 200, 300});
        
        // добавляем стиль для мотивирующих уведомлений
        if (type.name().startsWith("MOTIVATION_")) {
            builder.setStyle(new NotificationCompat.BigTextStyle()
                .bigText(message + "\n\n⚡ Storm Discord RPC ждет тебя!"));
        }
        
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);
        notificationManager.notify(type.ordinal(), builder.build());
    }
    
    // получение заголовка для типа уведомления
    private String getTitleForType(NotificationType type) {
        switch (type) {
            case STATUS_ENABLED:
                return "🎉 Статус активен!";
            case STATUS_DISABLED:
                return "😔 Статус отключен";
            case MOTIVATION_DAILY:
            case MOTIVATION_FUN:
                return "⚡ Storm Discord RPC";
            case ERROR_PROBLEM:
                return "❌ Ошибка";
            case SUCCESS_SETUP:
                return "✅ Успешно!";
            case REMINDER_RPC:
                return "📱 Напоминание";
            default:
                return "Storm Discord RPC";
        }
    }
    
    // проверка включенности уведомлений
    private boolean isNotificationsEnabled(NotificationType type) {
        if (!hasNotificationPermission()) {
            return false;
        }
        
        return preferences.getBoolean("notifications_enabled", true) &&
               preferences.getBoolean(getPreferenceKey(type), true);
    }
    
    // получение ключа для настроек
    private String getPreferenceKey(NotificationType type) {
        return "notification_" + type.name().toLowerCase();
    }
    
    // включение/выключение всех уведомлений
    public void setNotificationsEnabled(boolean enabled) {
        preferences.edit().putBoolean("notifications_enabled", enabled).apply();
    }
    
    // включение/выключение конкретного типа уведомлений
    public void setNotificationTypeEnabled(NotificationType type, boolean enabled) {
        preferences.edit().putBoolean(getPreferenceKey(type), enabled).apply();
    }
    
    // проверка включенности всех уведомлений
    public boolean isNotificationsEnabled() {
        return preferences.getBoolean("notifications_enabled", true);
    }
    
    // проверка включенности конкретного типа
    public boolean isNotificationTypeEnabled(NotificationType type) {
        return preferences.getBoolean(getPreferenceKey(type), true);
    }
    
    // отправка мотивирующего уведомления
    public void sendMotivationNotification() {
        String[] motivationalMessages = {
            "Твой Discord ждет тебя! ⚡",
            "Время показать свой статус! 🚀",
            "Скорее зайди в приложение! 🔥",
            "Твой статус крутой! ⭐",
            "Discord RPC ждет тебя! 💫",
            "Покажи всем свой статус! 🎯",
            "Время для Discord! 🎮",
            "Твой профиль ждет обновления! 📱"
        };
        
        String randomMessage = motivationalMessages[(int) (Math.random() * motivationalMessages.length)];
        sendNotification(NotificationType.MOTIVATION_FUN, randomMessage);
    }
    
    // отправка уведомления о статусе
    public void sendStatusNotification(boolean enabled) {
        if (enabled) {
            sendNotification(NotificationType.STATUS_ENABLED, null);
        } else {
            sendNotification(NotificationType.STATUS_DISABLED, null);
        }
    }
    
    // отправка уведомления об ошибке
    public void sendErrorNotification(String error) {
        sendNotification(NotificationType.ERROR_PROBLEM, error);
    }
    
    // отправка уведомления об успешной настройке
    public void sendSuccessNotification() {
        sendNotification(NotificationType.SUCCESS_SETUP, null);
    }
    
    // очистка всех уведомлений
    public void clearAllNotifications() {
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);
        notificationManager.cancelAll();
    }
}
