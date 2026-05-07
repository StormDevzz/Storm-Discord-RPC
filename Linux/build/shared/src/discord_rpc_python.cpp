#include "discord_rpc_real.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <chrono>
#include <memory>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>

// C++ обертка для Python Discord RPC
class DiscordRPCPython : public DiscordRPCReal {
private:
    std::string client_id;
    bool connected;
    std::string python_script_path;
    
    std::string escape_shell_arg(const std::string& arg) {
        std::string result = "'";
        for (char c : arg) {
            if (c == '\'') {
                result += "'\"'\"'";
            } else {
                result += c;
            }
        }
        result += "'";
        return result;
    }
    
public:
    DiscordRPCPython() : connected(false) {
        // Путь к Python скрипту
        python_script_path = "/home/abrakadam/Storm Discord RPC/shared/python/discord_rpc_simple.py";
    }
    
    ~DiscordRPCPython() {
        disconnect();
    }
    
    bool initialize(const std::string& client_id_str) {
        client_id = client_id_str;
        
        std::cout << "DEBUG: Инициализация Python Discord RPC с Client ID: " << client_id << std::endl;
        
        // Проверяем существование Python скрипта
        if (access(python_script_path.c_str(), R_OK | X_OK) != 0) {
            // Пробуем сделать скрипт исполняемым
            std::string chmod_cmd = "chmod +x '" + python_script_path + "'";
            system(chmod_cmd.c_str());
        }
        
        // Тестируем подключение
        std::string cmd = "python3 '" + python_script_path + "' " + client_id;
        int result = system(cmd.c_str());
        
        connected = (result == 0);
        if (connected) {
            std::cout << "DEBUG: Python Discord RPC успешно инициализирован" << std::endl;
        } else {
            std::cout << "DEBUG: Python Discord RPC инициализация не удалась" << std::endl;
        }
        
        return connected;
    }
    
    void updatePresence(const std::string& state, const std::string& details, 
                       const std::string& large_image_key = "", 
                       const std::string& large_image_text = "",
                       const std::string& small_image_key = "",
                       const std::string& small_image_text = "") {
        if (!connected) {
            std::cout << "DEBUG: Python Discord RPC не подключен" << std::endl;
            return;
        }
        
        std::cout << "DEBUG: Обновление статуса через Python:" << std::endl;
        std::cout << "  State: " << state << std::endl;
        std::cout << "  Details: " << details << std::endl;
        std::cout << "  Large Image: " << large_image_key << std::endl;
        std::cout << "  Large Text: " << large_image_text << std::endl;
        
        // Создаем временную функцию для обновления статуса
        std::string temp_script = python_script_path + "_temp.py";
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
        
        std::cout << "DEBUG: Статус обновлен (результат: " << result << ")" << std::endl;
    }
    
    void disconnect() {
        if (connected) {
            std::cout << "DEBUG: Отключение Python Discord RPC" << std::endl;
            connected = false;
        }
    }
    
    bool isConnected() const {
        return connected;
    }
    
    bool isInitialized() const {
        return connected;
    }
    
    std::string getLastError() const {
        return "";
    }
    
    void clearError() {
        // Нечего очищать в Python реализации
    }
    
    void processEvents() {
        // Python pypresence обрабатывает события автоматически
    }
    
private:
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
};

// Фабричная функция для создания Python реализации
extern "C" {
    DiscordRPCReal* create_discord_rpc_python() {
        return new DiscordRPCPython();
    }
    
    void destroy_discord_rpc_python(DiscordRPCReal* rpc) {
        delete rpc;
    }
}
