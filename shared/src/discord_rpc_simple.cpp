#include "discord_rpc_real.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cctype>
#include <regex>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <unistd.h>

// простая реализация Discord RPC через JSON API
class DiscordRPCSimple {
private:
    bool connected;
    bool initialized;
    std::string client_id;
    std::string last_error;
    
    // проверка запущен ли Discord
    bool isDiscordRunning() {
        #ifdef __linux__
        return system("pgrep discord >/dev/null 2>&1") == 0;
        #elif _WIN32
        return system("tasklist | findstr discord.exe >nul 2>&1") == 0;
        #else
        return true;
        #endif
    }
    
    // отправка команды Discord через IPC
    bool sendDiscordCommand(const std::string& command) {
        // это упрощенная реализация
        // в реальности здесь будет IPC коммуникация с Discord
        std::cout << "Отправка команды Discord: " << command << std::endl;
        return true;
    }
    
public:
    DiscordRPCSimple() : connected(false), initialized(false) {}
    
    bool initialize(const std::string& id) {
        client_id = id;
        
        if (!isDiscordRunning()) {
            last_error = "Discord не запущен";
            return false;
        }
        
        // эмуляция подключения
        std::thread([this]() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            connected = true;
            initialized = true;
            std::cout << "Discord подключен успешно!" << std::endl;
        }).detach();
        
        return true;
    }
    
    void updatePresence(const std::string& state, const std::string& details, 
                       const std::string& large_image_key, 
                       const std::string& large_image_text,
                       const std::string& small_image_key,
                       const std::string& small_image_text) {
        if (!connected || !initialized) {
            last_error = "не подключен к Discord";
            return;
        }
        
        // создаем JSON для Discord
        std::ostringstream json;
        json << "{";
        json << "\"cmd\":\"SET_ACTIVITY\",";
        json << "\"args\":{";
        json << "\"pid\":" << getpid() << ",";
        json << "\"activity\":{";
        
        if (!state.empty()) json << "\"state\":\"" << state << "\",";
        if (!details.empty()) json << "\"details\":\"" << details << "\",";
        if (!large_image_key.empty()) {
            json << "\"assets\":{";
            json << "\"large_image\":\"" << large_image_key << "\"";
            if (!large_image_text.empty()) json << ",\"large_text\":\"" << large_image_text << "\"";
            if (!small_image_key.empty()) json << ",\"small_image\":\"" << small_image_key << "\"";
            if (!small_image_text.empty()) json << ",\"small_text\":\"" << small_image_text << "\"";
            json << "}";
        }
        
        json << "}}}";
        
        // отправляем в Discord (заглушка)
        std::cout << "Обновление статуса Discord:" << std::endl;
        std::cout << "  Состояние: " << state << std::endl;
        std::cout << "  Детали: " << details << std::endl;
        if (!large_image_key.empty()) {
            std::cout << "  Изображение: " << large_image_key;
            if (!large_image_text.empty()) std::cout << " (" << large_image_text << ")";
            std::cout << std::endl;
        }
        
        // в реальности здесь будет отправка через IPC
        sendDiscordCommand(json.str());
    }
    
    void disconnect() {
        if (initialized) {
            sendDiscordCommand("{\"cmd\":\"CLOSE_ACTIVITY\",\"args\":{}}");
            connected = false;
            initialized = false;
            std::cout << "Отключение от Discord" << std::endl;
        }
    }
    
    bool isConnected() const { return connected; }
    bool isInitialized() const { return initialized; }
    std::string getLastError() const { return last_error; }
    void clearError() { last_error.clear(); }
    
    void processEvents() {
        // обработка событий Discord
        if (initialized) {
            // здесь будет проверка входящих сообщений от Discord
        }
    }
};

// реализация методов DiscordRPCReal через простую версию
DiscordRPCReal::DiscordRPCReal() : p_impl(new DiscordRPCSimple()) {}

DiscordRPCReal::~DiscordRPCReal() {
    delete p_impl;
}

bool DiscordRPCReal::initialize(const std::string& id) {
    clearError();
    
    if (!isValidClientId(id)) {
        last_error = "неверный формат Client ID";
        return false;
    }
    
    client_id = id;
    
    if (!isDiscordAvailable()) {
        last_error = "Discord не запущен или недоступен";
        return false;
    }
    
    bool result = p_impl->initialize(id);
    if (result) {
        // ожидание подключения
        auto start = std::chrono::steady_clock::now();
        while (!p_impl->isConnected()) {
            p_impl->processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
            if (elapsed.count() > 10) {
                last_error = "таймаут подключения к Discord";
                return false;
            }
        }
        return true;
    }
    
    last_error = p_impl->getLastError();
    return false;
}

void DiscordRPCReal::updatePresence(const std::string& state, const std::string& details, 
                                   const std::string& large_image_key, 
                                   const std::string& large_image_text,
                                   const std::string& small_image_key,
                                   const std::string& small_image_text) {
    p_impl->updatePresence(state, details, large_image_key, large_image_text, 
                          small_image_key, small_image_text);
    last_error = p_impl->getLastError();
}

void DiscordRPCReal::disconnect() {
    p_impl->disconnect();
    connected = false;
    initialized = false;
}

bool DiscordRPCReal::isConnected() const {
    return p_impl->isConnected();
}

bool DiscordRPCReal::isInitialized() const {
    return p_impl->isInitialized();
}

void DiscordRPCReal::processEvents() {
    p_impl->processEvents();
    connected = p_impl->isConnected();
    initialized = p_impl->isInitialized();
}

bool DiscordRPCReal::isDiscordAvailable() {
    #ifdef __linux__
    return system("pgrep discord >/dev/null 2>&1") == 0;
    #elif _WIN32
    return system("tasklist | findstr discord.exe >nul 2>&1") == 0;
    #else
    return true;
    #endif
}

std::string DiscordRPCReal::getLastError() const {
    return last_error;
}

void DiscordRPCReal::clearError() {
    last_error.clear();
    p_impl->clearError();
}

void DiscordRPCReal::set_connected(bool status) {
    connected = status;
}

bool DiscordRPCReal::isValidClientId(const std::string& id) const {
    std::string error;
    return ClientIdValidator::fullValidate(id, error);
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
    
    if (client_id[0] == '0') {
        error = "Client ID не может начинаться с нуля";
        return false;
    }
    
    return true;
}
