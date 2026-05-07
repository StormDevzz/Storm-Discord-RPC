package com.storm.discordrpc

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    
    // ui элементы
    private lateinit var clientIdEditText: EditText
    private lateinit var connectButton: Button
    private lateinit var disconnectButton: Button
    private lateinit var stateEditText: EditText
    private lateinit var detailsEditText: EditText
    private lateinit var largeImageEditText: EditText
    private lateinit var largeTextEditText: EditText
    private lateinit var smallImageEditText: EditText
    private lateinit var smallTextEditText: EditText
    private lateinit var updateButton: Button
    private lateinit var statusTextView: TextView
    
    // discord rpc
    private var discordRPC: DiscordRPCNative? = null
    private var isConnected = false
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        
        initializeViews()
        setupClickListeners()
        loadNativeLibrary()
    }
    
    private fun initializeViews() {
        clientIdEditText = findViewById(R.id.clientIdEditText)
        connectButton = findViewById(R.id.connectButton)
        disconnectButton = findViewById(R.id.disconnectButton)
        stateEditText = findViewById(R.id.stateEditText)
        detailsEditText = findViewById(R.id.detailsEditText)
        largeImageEditText = findViewById(R.id.largeImageEditText)
        largeTextEditText = findViewById(R.id.largeTextEditText)
        smallImageEditText = findViewById(R.id.smallImageEditText)
        smallTextEditText = findViewById(R.id.smallTextEditText)
        updateButton = findViewById(R.id.updateButton)
        statusTextView = findViewById(R.id.statusTextView)
        
        // начальное состояние
        disconnectButton.isEnabled = false
        updateButton.isEnabled = false
        statusTextView.text = "статус: не подключен"
    }
    
    private fun setupClickListeners() {
        connectButton.setOnClickListener {
            connectToDiscord()
        }
        
        disconnectButton.setOnClickListener {
            disconnectFromDiscord()
        }
        
        updateButton.setOnClickListener {
            updatePresence()
        }
    }
    
    private fun loadNativeLibrary() {
        try {
            System.loadLibrary("discord_rpc_native")
            discordRPC = DiscordRPCNative()
            Toast.makeText(this, "библиотека загружена успешно", Toast.LENGTH_SHORT).show()
        } catch (e: UnsatisfiedLinkError) {
            Toast.makeText(this, "ошибка загрузки библиотеки: ${e.message}", Toast.LENGTH_LONG).show()
        }
    }
    
    private fun connectToDiscord() {
        val clientId = clientIdEditText.text.toString().trim()
        
        if (clientId.isEmpty()) {
            Toast.makeText(this, "введите Client ID", Toast.LENGTH_SHORT).show()
            return
        }
        
        discordRPC?.let { rpc ->
            if (rpc.initialize(clientId)) {
                isConnected = true
                connectButton.isEnabled = false
                disconnectButton.isEnabled = true
                updateButton.isEnabled = true
                clientIdEditText.isEnabled = false
                statusTextView.text = "статус: подключено"
                Toast.makeText(this, "подключено к Discord", Toast.LENGTH_SHORT).show()
            } else {
                Toast.makeText(this, "ошибка подключения", Toast.LENGTH_SHORT).show()
            }
        }
    }
    
    private fun disconnectFromDiscord() {
        discordRPC?.disconnect()
        isConnected = false
        connectButton.isEnabled = true
        disconnectButton.isEnabled = false
        updateButton.isEnabled = false
        clientIdEditText.isEnabled = true
        statusTextView.text = "статус: не подключен"
        Toast.makeText(this, "отключено от Discord", Toast.LENGTH_SHORT).show()
    }
    
    private fun updatePresence() {
        if (!isConnected) {
            Toast.makeText(this, "сначала подключитесь к Discord", Toast.LENGTH_SHORT).show()
            return
        }
        
        val state = stateEditText.text.toString()
        val details = detailsEditText.text.toString()
        val largeImage = largeImageEditText.text.toString()
        val largeText = largeTextEditText.text.toString()
        val smallImage = smallImageEditText.text.toString()
        val smallText = smallTextEditText.text.toString()
        
        discordRPC?.updatePresence(state, details, largeImage, largeText, smallImage, smallText)
        Toast.makeText(this, "статус обновлен", Toast.LENGTH_SHORT).show()
    }
    
    override fun onDestroy() {
        super.onDestroy()
        discordRPC?.disconnect()
    }
}
