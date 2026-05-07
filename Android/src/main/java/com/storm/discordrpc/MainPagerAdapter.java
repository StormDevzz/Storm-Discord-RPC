package com.storm.discordrpc;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentPagerAdapter;

// адаптер для вкладок главного экрана
public class MainPagerAdapter extends FragmentPagerAdapter {
    
    private static final int NUM_TABS = 2;
    private StatusFragment statusFragment;
    private SettingsFragment settingsFragment;
    private String clientId;

    public MainPagerAdapter(@NonNull FragmentManager fm, String clientId) {
        super(fm, BEHAVIOR_RESUME_ONLY_CURRENT_FRAGMENT);
        this.clientId = clientId;
    }

    @NonNull
    @Override
    public Fragment getItem(int position) {
        switch (position) {
            case 0:
                if (statusFragment == null) {
                    statusFragment = new StatusFragment();
                    Bundle args = new Bundle();
                    args.putString("client_id", clientId);
                    statusFragment.setArguments(args);
                }
                return statusFragment;
            case 1:
                if (settingsFragment == null) {
                    settingsFragment = new SettingsFragment();
                }
                return settingsFragment;
            default:
                return new StatusFragment();
        }
    }

    @Override
    public int getCount() {
        return NUM_TABS;
    }

    @Override
    public CharSequence getPageTitle(int position) {
        switch (position) {
            case 0:
                return "📱 Статус";
            case 1:
                return "⚙️ Настройки";
            default:
                return "Статус";
        }
    }

    public void setNotificationManager(StormNotificationManager manager) {
        if (statusFragment != null) {
            statusFragment.setNotificationManager(manager);
        }
        if (settingsFragment != null) {
            settingsFragment.setNotificationManager(manager);
        }
    }

    public void updateClientId(String clientId) {
        this.clientId = clientId;
        if (statusFragment != null) {
            statusFragment.setClientId(clientId);
        }
    }
}
