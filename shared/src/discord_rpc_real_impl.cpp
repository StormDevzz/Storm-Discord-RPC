#include "discord_rpc_real.h"
#include <iostream>
#include <regex>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <algorithm>

DiscordRPCReal::DiscordRPCReal() : client_id(""), connected(false), initialized(false), p_impl(nullptr) {
    // Создаем Python реализацию по умолчанию
    use_python = true;
}

DiscordRPCReal::~DiscordRPCReal() {
    // p_impl - это forward declaration, не удаляем его здесь
    p_impl = nullptr;
}

bool DiscordRPCReal::initialize(const std::string& id) {
    if (!isValidClientId(id)) {
        last_error = "Invalid Client ID format";
        return false;
    }
    
    client_id = id;
    
    if (use_python) {
        // Используем Python реализацию
        std::cout << "Using Python Discord RPC implementation" << std::endl;
        
        // Проверяем доступность Discord
        if (!isDiscordAvailable()) {
            last_error = "Discord is not running";
            return false;
        }
        
        // Запускаем Python скрипт для теста
        std::string python_script = "/home/abrakadam/Storm Discord RPC/shared/python/discord_rpc_simple.py";
        std::string cmd = "python3 '" + python_script + "' " + id + " > /dev/null 2>&1";
        int result = system(cmd.c_str());
        
        connected = (result == 0);
        initialized = connected;
        
        if (connected) {
            std::cout << "Python Discord RPC initialized successfully" << std::endl;
        } else {
            std::cout << "Python Discord RPC initialization failed" << std::endl;
        }
        
        return connected;
    } else {
        // Используем базовую реализацию
        std::cout << "Using base Discord RPC implementation" << std::endl;
        
        // Проверяем доступность Discord
        if (!isDiscordAvailable()) {
            last_error = "Discord is not running";
            return false;
        }
        
        connected = true;
        initialized = true;
        return true;
    }
}

void DiscordRPCReal::updatePresence(const std::string& state, const std::string& details, 
                                   const std::string& large_image_key, 
                                   const std::string& large_image_text,
                                   const std::string& small_image_key,
                                   const std::string& small_image_text) {
    if (!connected || !initialized) {
        last_error = "Not connected to Discord";
        return;
    }
    
    if (use_python) {
        // Используем Python реализацию
        std::cout << "Updating presence via Python:" << std::endl;
        std::cout << "  State: " << state << std::endl;
        std::cout << "  Details: " << details << std::endl;
        
        // Создаем временный Python скрипт
        std::string temp_script = "/tmp/discord_update_" + std::to_string(getpid()) + ".py";
        std::ofstream script(temp_script);
        
        script << "#!/usr/bin/env python3\n";
        script << "import sys\n";
        script << "sys.path.append('/home/abrakadam/Storm Discord RPC/shared/python')\n";
        script << "from discord_rpc_simple import set_discord_status\n";
        script << "import time\n\n";
        script << "rpc = set_discord_status(\n";
        script << "    '" << client_id << "',\n";
        script << "    state='" << escape_python_string(state) << "',\n";
        script << "    details='" << escape_python_string(details) << "',\n";
        script << "    large_image='" << escape_python_string(large_image_key) << "',\n";
        script << "    large_text='" << escape_python_string(large_image_text) << "'\n";
        script << ")\n";
        script << "if rpc:\n";
        script << "    time.sleep(0.5)\n";
        script << "    rpc.close()\n";
        
        script.close();
        
        // Запускаем скрипт
        std::string cmd = "python3 '" + temp_script + "'";
        int result = system(cmd.c_str());
        
        // Удаляем временный скрипт
        unlink(temp_script.c_str());
        
        std::cout << "Python status update result: " << result << std::endl;
    } else {
        // Базовая реализация
        std::cout << "Updating presence with state: " << state << ", details: " << details << std::endl;
    }
}

void DiscordRPCReal::disconnect() {
    if (connected) {
        // очищаем статус Discord перед отключением
        clear_status();
    }
    connected = false;
    initialized = false;
}

void DiscordRPCReal::clear_status() {
    if (use_python) {
        // Используем Python для очистки статуса
        std::string temp_script = "/tmp/discord_clear_" + std::to_string(getpid()) + ".py";
        std::ofstream script(temp_script);
        
        script << "#!/usr/bin/env python3\n";
        script << "import sys\n";
        script << "sys.path.append('/home/abrakadam/Storm Discord RPC/shared/python')\n";
        script << "from discord_rpc_simple import clear_discord_status\n";
        script << "clear_discord_status('" << client_id << "')\n";
        
        script.close();
        
        // Запускаем скрипт
        std::string cmd = "python3 '" + temp_script + "'";
        system(cmd.c_str());
        
        // Удаляем временный скрипт
        unlink(temp_script.c_str());
        
        std::cout << "Статус Discord очищен через Python" << std::endl;
    } else {
        // Базовая реализация - просто выводим сообщение
        std::cout << "Очистка статуса Discord (базовая реализация)" << std::endl;
    }
}

bool DiscordRPCReal::isConnected() const {
    return connected;
}

bool DiscordRPCReal::isInitialized() const {
    return initialized;
}

void DiscordRPCReal::set_connected(bool status) {
    connected = status;
}

void DiscordRPCReal::processEvents() {
    // Обработка событий Discord
}

bool DiscordRPCReal::isDiscordAvailable() {
    // Проверяем наличие Discord IPC сокетов
    const char* run_user = getenv("USER");
    if (run_user) {
        char uid_str[32];
        snprintf(uid_str, sizeof(uid_str), "%d", getuid());
        
        std::string run_path = "/run/user/" + std::string(uid_str);
        
        for (int i = 0; i < 10; i++) {
            std::string ipc_path = run_path + "/discord-ipc-" + std::to_string(i);
            if (access(ipc_path.c_str(), R_OK | W_OK) == 0) {
                return true;
            }
        }
    }
    
    return false;
}

std::string DiscordRPCReal::getLastError() const {
    return last_error;
}

void DiscordRPCReal::clearError() {
    last_error.clear();
}

bool DiscordRPCReal::isValidClientId(const std::string& id) const {
    return ClientIdValidator::fullValidate(id, const_cast<std::string&>(last_error));
}

void DiscordRPCReal::discordInitialize(const char* applicationId) {
    client_id = applicationId;
    connected = true;
    initialized = true;
}

void DiscordRPCReal::discordShutdown() {
    connected = false;
    initialized = false;
}

void DiscordRPCReal::discordRunCallbacks() {
    processEvents();
}

void DiscordRPCReal::onDiscordReady() {
    std::cout << "Discord Ready" << std::endl;
}

void DiscordRPCReal::onDiscordDisconnected(int errorCode, const char* message) {
    std::cout << "Discord Disconnected: " << errorCode << " - " << message << std::endl;
    // connected - это member variable, не можем изменять в static методе
}

void DiscordRPCReal::onDiscordError(int errorCode, const char* message) {
    std::cout << "Discord Error: " << errorCode << " - " << message << std::endl;
}

// Реализация ClientIdValidator
bool ClientIdValidator::validate(const std::string& client_id) {
    return validateLength(client_id) && validateDigits(client_id);
}

bool ClientIdValidator::validateLength(const std::string& client_id) {
    return client_id.length() >= 17 && client_id.length() <= 19;
}

bool ClientIdValidator::validateDigits(const std::string& client_id) {
    return std::all_of(client_id.begin(), client_id.end(), ::isdigit);
}

bool ClientIdValidator::validateWithAPI(const std::string& client_id) {
    // В реальном приложении здесь была бы проверка через API Discord
    // Для примера всегда возвращаем true
    return true;
}

bool ClientIdValidator::fullValidate(const std::string& client_id, std::string& error) {
    if (!validateLength(client_id)) {
        error = "Client ID must be 17-19 digits long";
        return false;
    }
    
    if (!validateDigits(client_id)) {
        error = "Client ID must contain only digits";
        return false;
    }
    
    if (client_id[0] == '0') {
        error = "Client ID cannot start with 0";
        return false;
    }
    
    return true;
}

// Вспомогательная функция для экранирования Python строк
std::string escape_python_string(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == '\\') {
            result += "\\\\";
        } else if (c == '\'') {
            result += "\\'";
        } else if (c == '\n') {
            result += "\\n";
        } else if (c == '\r') {
            result += "\\r";
        } else if (c == '\t') {
            result += "\\t";
        } else {
            result += c;
        }
    }
    return result;
}
