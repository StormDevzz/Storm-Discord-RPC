package com.storm.discordrpc;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;

// простой главный актив без фрагментов для отладки
public class MainActivitySimple extends AppCompatActivity {
    private String clientId;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        try {
            Log.d("StormRPC", "MainActivitySimple onCreate started");
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_main);
            Log.d("StormRPC", "MainActivitySimple layout set");

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

            Log.d("StormRPC", "MainActivitySimple onCreate completed successfully");
            
        } catch (Exception e) {
            Log.e("StormRPC", "Error in MainActivitySimple onCreate: " + e.getMessage(), e);
            Toast.makeText(this, "Ошибка загрузки главного экрана: " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }
}
