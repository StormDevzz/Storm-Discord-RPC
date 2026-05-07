#include "discord_rpc_real.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cctype>
#include <regex>
#include <cstdlib>
#include <cstring>

// глобальный экземпляр Discord RPC
static DiscordRPCReal* g_discord_instance = nullptr;

// обработчики событий Discord
static void on_discord_ready(const DiscordUser* user) {
    if (g_discord_instance) {
        g_discord_instance->set_connected(true);
        std::cout << "Discord готов к работе! Пользователь: " << user->username << std::endl;
    }
}

static void on_discord_disconnected(int errorCode, const char* message) {
    if (g_discord_instance) {
        g_discord_instance->set_connected(false);
        std::cout << "Discord отключен: " << message << " (код: " << errorCode << ")" << std::endl;
    }
}

static void on_discord_error(int errorCode, const char* message) {
    std::cout << "ошибка Discord: " << message << " (код: " << errorCode << ")" << std::endl;
}

DiscordRPCReal::DiscordRPCReal() : connected(false), initialized(false) {
    g_discord_instance = this;
}

DiscordRPCReal::~DiscordRPCReal() {
    disconnect();
}

bool DiscordRPCReal::isValidClientId(const std::string& id) const {
    std::string error;
    return ClientIdValidator::fullValidate(id, error);
}

bool DiscordRPCReal::initialize(const std::string& id) {
    clearError();
    
    // строгая проверка Client ID
    if (!isValidClientId(id)) {
        last_error = "неверный формат Client ID";
        return false;
    }
    
    client_id = id;
    
    // проверка доступности Discord
    if (!isDiscordAvailable()) {
        last_error = "Discord не запущен или недоступен";
        return false;
    }
    
    try {
        // настройка обработчиков событий
        DiscordEventHandlers handlers;
        memset(&handlers, 0, sizeof(handlers));
        handlers.ready = on_discord_ready;
        handlers.disconnected = on_discord_disconnected;
        handlers.errored = on_discord_error;
        
        // реальная инициализация Discord
        Discord_Initialize(client_id.c_str(), &handlers, 1, nullptr);
        initialized = true;
        
        // ожидание готовности
        auto start = std::chrono::steady_clock::now();
        bool timeout = false;
        
        while (!connected && !timeout) {
            Discord_RunCallbacks();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // таймаут 10 секунд
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
            if (elapsed.count() > 10) {
                timeout = true;
            }
        }
        
        if (connected) {
            std::cout << "успешное подключение к Discord с Client ID: " << client_id << std::endl;
            return true;
        } else {
            last_error = "таймаут подключения к Discord";
            Discord_Shutdown();
            initialized = false;
            return false;
        }
        
    } catch (const std::exception& e) {
        last_error = std::string("ошибка инициализации: ") + e.what();
        return false;
    }
}

void DiscordRPCReal::updatePresence(const std::string& state, const std::string& details, 
                                   const std::string& large_image_key, 
                                   const std::string& large_image_text,
                                   const std::string& small_image_key,
                                   const std::string& small_image_text) {
    if (!connected || !initialized) {
        last_error = "не подключен к Discord";
        return;
    }
    
    DiscordRichPresence presence = {};
    presence.state = state.empty() ? nullptr : state.c_str();
    presence.details = details.empty() ? nullptr : details.c_str();
    presence.largeImageKey = large_image_key.empty() ? nullptr : large_image_key.c_str();
    presence.largeImageText = large_image_text.empty() ? nullptr : large_image_text.c_str();
    presence.smallImageKey = small_image_key.empty() ? nullptr : small_image_key.c_str();
    presence.smallImageText = small_image_text.empty() ? nullptr : small_image_text.c_str();
    presence.startTimestamp = 0;
    presence.endTimestamp = 0;
    
    try {
        Discord_UpdatePresence(&presence);
        std::cout << "статус успешно обновлен в Discord" << std::endl;
    } catch (const std::exception& e) {
        last_error = std::string("ошибка обновления статуса: ") + e.what();
    }
}

void DiscordRPCReal::disconnect() {
    if (initialized) {
        try {
            Discord_Shutdown();
            std::cout << "отключение от Discord выполнено" << std::endl;
        } catch (const std::exception& e) {
            last_error = std::string("ошибка отключения: ") + e.what();
        }
        
        initialized = false;
        connected = false;
    }
}

bool DiscordRPCReal::isConnected() const {
    return connected;
}

bool DiscordRPCReal::isInitialized() const {
    return initialized;
}

void DiscordRPCReal::processEvents() {
    if (initialized) {
        try {
            Discord_RunCallbacks();
        } catch (const std::exception& e) {
            last_error = std::string("ошибка обработки событий: ") + e.what();
        }
    }
}

bool DiscordRPCReal::isDiscordAvailable() {
    // проверка запущен ли Discord
    #ifdef __linux__
    // для Linux проверяем процесс Discord
    return system("pgrep discord >/dev/null 2>&1") == 0;
    #elif _WIN32
    // для Windows проверяем процесс Discord
    return system("tasklist | findstr discord.exe >nul 2>&1") == 0;
    #else
    // для других платформ считаем что Discord доступен
    return true;
    #endif
}

std::string DiscordRPCReal::getLastError() const {
    return last_error;
}

void DiscordRPCReal::clearError() {
    last_error.clear();
}

void DiscordRPCReal::set_connected(bool status) {
    connected = status;
}

// реализация валидатора Client ID
bool ClientIdValidator::validate(const std::string& client_id) {
    return validateLength(client_id) && validateDigits(client_id);
}

bool ClientIdValidator::validateLength(const std::string& client_id) {
    return client_id.length() >= 17 && client_id.length() <= 19;
}

bool ClientIdValidator::validateDigits(const std::string& client_id) {
    if (client_id.empty()) return false;
    
    for (char c : client_id) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool ClientIdValidator::validateWithAPI(const std::string& client_id) {
    // в реальном проекте здесь можно сделать HTTP запрос к Discord API
    // для проверки существования приложения
    // пока возвращаем true если формат правильный
    return validate(client_id);
}

bool ClientIdValidator::fullValidate(const std::string& client_id, std::string& error) {
    error.clear();
    
    if (client_id.empty()) {
        error = "Client ID не может быть пустым";
        return false;
    }
    
    if (!validateLength(client_id)) {
        error = "Client ID должен содержать от 17 до 19 цифр";
        return false;
    }
    
    if (!validateDigits(client_id)) {
        error = "Client ID должен содержать только цифры";
        return false;
    }
    
    // дополнительная проверка: не должен начинаться с 0
    if (client_id[0] == '0') {
        error = "Client ID не может начинаться с нуля";
        return false;
    }
    
    return true;
}
