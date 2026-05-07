package com.storm.discordrpc;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;
import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.storm.discordrpc.StormNotificationManager;

// адаптер для настроек уведомлений
public class NotificationSettingsAdapter extends RecyclerView.Adapter<NotificationSettingsAdapter.ViewHolder> {
    
    private Context context;
    private StormNotificationManager notificationManager;
    
    // типы уведомлений для настроек
    private final StormNotificationManager.NotificationType[] notificationTypes = {
        StormNotificationManager.NotificationType.STATUS_ENABLED,
        StormNotificationManager.NotificationType.STATUS_DISABLED,
        StormNotificationManager.NotificationType.MOTIVATION_DAILY,
        StormNotificationManager.NotificationType.MOTIVATION_FUN,
        StormNotificationManager.NotificationType.ERROR_PROBLEM,
        StormNotificationManager.NotificationType.SUCCESS_SETUP,
        StormNotificationManager.NotificationType.REMINDER_RPC
    };
    
    // названия типов уведомлений
    private final String[] notificationNames = {
        "Уведомления о статусе",
        "Уведомления об отключении",
        "Ежедневная мотивация",
        "Развлекательные уведомления",
        "Уведомления об ошибках",
        "Уведомления об успехе",
        "Напоминания о RPC"
    };
    
    // описания типов уведомлений
    private final String[] notificationDescriptions = {
        "Уведомления при включении статуса",
        "Уведомления при отключении статуса",
        "Мотивирующие сообщения каждый день",
        "Веселые уведомления для вовлеченности",
        "Сообщения о проблемах и ошибках",
        "Уведомления об успешных действиях",
        "Напоминания о Rich Presence"
    };

    public NotificationSettingsAdapter(Context context, StormNotificationManager notificationManager) {
        this.context = context;
        this.notificationManager = notificationManager;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(context).inflate(R.layout.item_notification_setting, parent, false);
        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        StormNotificationManager.NotificationType type = notificationTypes[position];
        String name = notificationNames[position];
        String description = notificationDescriptions[position];
        
        holder.titleTextView.setText(name);
        holder.descriptionTextView.setText(description);
        
        // устанавливаем состояние переключателя
        boolean isEnabled = notificationManager.isNotificationTypeEnabled(type) && 
                           notificationManager.isNotificationsEnabled();
        holder.switchView.setChecked(isEnabled);
        holder.switchView.setEnabled(notificationManager.isNotificationsEnabled());
        
        // обработчик переключения
        holder.switchView.setOnCheckedChangeListener((buttonView, isChecked) -> {
            notificationManager.setNotificationTypeEnabled(type, isChecked);
            
            // тестовое уведомление
            if (isChecked && notificationManager.hasNotificationPermission()) {
                switch (type) {
                    case MOTIVATION_FUN:
                        notificationManager.sendMotivationNotification();
                        break;
                    case STATUS_ENABLED:
                        notificationManager.sendNotification(type, "Тестовое уведомление о статусе!");
                        break;
                    case SUCCESS_SETUP:
                        notificationManager.sendNotification(type, "Тестовое уведомление об успехе!");
                        break;
                    default:
                        break;
                }
            }
        });
    }

    @Override
    public int getItemCount() {
        return notificationTypes.length;
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {
        TextView titleTextView;
        TextView descriptionTextView;
        Switch switchView;

        public ViewHolder(@NonNull View itemView) {
            super(itemView);
            titleTextView = itemView.findViewById(R.id.notificationTitle);
            descriptionTextView = itemView.findViewById(R.id.notificationDescription);
            switchView = itemView.findViewById(R.id.notificationSwitch);
        }
    }
}
