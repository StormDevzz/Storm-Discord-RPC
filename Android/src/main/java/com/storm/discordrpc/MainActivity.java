package com.storm.discordrpc;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import androidx.viewpager.widget.ViewPager;
import com.google.android.material.tabs.TabLayout;

// главный актив приложения с вкладками
public class MainActivity extends AppCompatActivity {
    private String clientId;
    
    private ViewPager viewPager;
    private TabLayout tabLayout;
    private MainPagerAdapter pagerAdapter;
    private StormNotificationManager notificationManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        try {
            Log.d("StormRPC", "MainActivity onCreate started");
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_main_with_tabs);
            Log.d("StormRPC", "MainActivity layout with tabs set");

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
            
            if (clientId == null) {
                // если client id нет, перенаправляем на экран ввода
                Log.d("StormRPC", "No client ID, redirecting to ClientIDActivity");
                Intent intent = new Intent(this, ClientIDActivity.class);
                startActivity(intent);
                finish();
                return;
            }

            // инициализация элементов
            initializeViews();
            Log.d("StormRPC", "Main views initialized");
            
            setupViewPager();
            Log.d("StormRPC", "ViewPager setup completed");
            
            initializeNotificationManager();
            Log.d("StormRPC", "Notification manager initialized");
            
            Log.d("StormRPC", "MainActivity onCreate completed successfully");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in MainActivity onCreate: " + e.getMessage(), e);
            Toast.makeText(this, "Ошибка загрузки главного экрана: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    // инициализация view элементов для вкладок
    private void initializeViews() {
        try {
            Log.d("StormRPC", "Finding ViewPager and TabLayout in MainActivity");
            
            viewPager = findViewById(R.id.viewPager);
            Log.d("StormRPC", "viewPager found: " + (viewPager != null));
            
            tabLayout = findViewById(R.id.tabLayout);
            Log.d("StormRPC", "tabLayout found: " + (tabLayout != null));
            
            // проверка что все view найдены
            if (viewPager == null || tabLayout == null) {
                Log.e("StormRPC", "ViewPager or TabLayout is null in MainActivity!");
                Toast.makeText(this, "Ошибка загрузки интерфейса вкладок", Toast.LENGTH_LONG).show();
                return;
            }
            
            Log.d("StormRPC", "All views initialized successfully");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in initializeViews: " + e.getMessage(), e);
            Toast.makeText(this, "Ошибка инициализации интерфейса: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    // настройка ViewPager и вкладок
    private void setupViewPager() {
        try {
            Log.d("StormRPC", "Setting up ViewPager");
            
            if (viewPager != null && tabLayout != null) {
                // создание адаптера
                pagerAdapter = new MainPagerAdapter(getSupportFragmentManager(), clientId);
                viewPager.setAdapter(pagerAdapter);
                
                // подключение TabLayout к ViewPager
                tabLayout.setupWithViewPager(viewPager);
                
                // настройка notification manager для фрагментов
                if (notificationManager != null) {
                    pagerAdapter.setNotificationManager(notificationManager);
                }
                
                Log.d("StormRPC", "ViewPager setup completed successfully");
            } else {
                Log.e("StormRPC", "ViewPager or TabLayout is null in setupViewPager");
            }
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in setupViewPager: " + e.getMessage(), e);
            Toast.makeText(this, "Ошибка настройки вкладок: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }

    // инициализация менеджера уведомлений
    private void initializeNotificationManager() {
        try {
            Log.d("StormRPC", "Initializing notification manager in MainActivity");
            notificationManager = new StormNotificationManager(this);
            
            // настройка notification manager для фрагментов
            if (pagerAdapter != null) {
                pagerAdapter.setNotificationManager(notificationManager);
            }
            
            Log.d("StormRPC", "Notification manager initialized successfully in MainActivity");
        } catch (Exception e) {
            Log.e("StormRPC", "Error initializing notification manager in MainActivity: " + e.getMessage(), e);
            notificationManager = null; // не критично, продолжаем без уведомлений
        }
    }
}
