#ifndef DISCORD_RPC_REAL_H
#define DISCORD_RPC_REAL_H

#include <string>
#include <functional>
#include <cstdint>

// класс для реализации Discord RPC через IPC
class DiscordIPC;

// обработчики событий
typedef void (*DiscordReadyHandler)();
typedef void (*DiscordDisconnectedHandler)(int errorCode, const char* message);
typedef void (*DiscordErroredHandler)(int errorCode, const char* message);
typedef void (*DiscordJoinGameHandler)(const char* joinSecret);
typedef void (*DiscordSpectateGameHandler)(const char* spectateSecret);
typedef void (*DiscordJoinRequestHandler)(void* request);

// класс для работы с реальным Discord Rich Presence
class DiscordRPCReal {
private:
    std::string client_id;
    bool connected;
    bool initialized;
    bool use_python;
    
    // проверка Client ID
    bool isValidClientId(const std::string& id) const;
    
private:
    DiscordIPC* p_impl;
    
public:
    DiscordRPCReal();
    ~DiscordRPCReal();
    
    // инициализация с client id и проверкой
    bool initialize(const std::string& id);
    
    // обновление статуса
    void updatePresence(const std::string& state, const std::string& details, 
                       const std::string& large_image_key = "", 
                       const std::string& large_image_text = "",
                       const std::string& small_image_key = "",
                       const std::string& small_image_text = "");
    
    // отключение
    void disconnect();
    
    // очистка статуса
    void clear_status();
    
    // проверка подключения
    bool isConnected() const;
    
    // проверка инициализации
    bool isInitialized() const;
    
    // установка статуса подключения (для обработчиков)
    void set_connected(bool status);
    
    // обработка событий (нужно вызывать в главном цикле)
    void processEvents();
    
    // проверка доступности Discord
    static bool isDiscordAvailable();
    
    // получение ошибки
    std::string getLastError() const;
    
    // очистка ошибок
    void clearError();
    
private:
    std::string last_error;
    
    // реальные функции Discord RPC
    void discordInitialize(const char* applicationId);
    void discordShutdown();
    void discordRunCallbacks();
    
    // обработчики событий
    static void onDiscordReady();
    static void onDiscordDisconnected(int errorCode, const char* message);
    static void onDiscordError(int errorCode, const char* message);
};

// валидатор Client ID
class ClientIdValidator {
public:
    // проверка формата Client ID
    static bool validate(const std::string& client_id);
    
    // проверка длины (должен быть 17-19 цифр)
    static bool validateLength(const std::string& client_id);
    
    // проверка что это только цифры
    static bool validateDigits(const std::string& client_id);
    
    // проверка через API Discord (если доступно)
    static bool validateWithAPI(const std::string& client_id);
    
    // полная проверка
    static bool fullValidate(const std::string& client_id, std::string& error);
};

// вспомогательная функция для экранирования Python строк
std::string escape_python_string(const std::string& str);

#endif
