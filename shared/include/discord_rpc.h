#ifndef DISCORD_RPC_H
#define DISCORD_RPC_H

#include <string>
#include <functional>

// класс для работы с Discord Rich Presence
class DiscordRPC {
private:
    std::string client_id;
    bool connected;
    
public:
    DiscordRPC();
    ~DiscordRPC();
    
    // инициализация с client id
    bool initialize(const std::string& id);
    
    // обновление статуса
    void updatePresence(const std::string& state, const std::string& details, 
                       const std::string& large_image_key = "", 
                       const std::string& large_image_text = "",
                       const std::string& small_image_key = "",
                       const std::string& small_image_text = "");
    
    // отключение
    void disconnect();
    
    // проверка подключения
    bool isConnected() const;
    
    // обработка событий (нужно вызывать в главном цикле)
    void processEvents();
};

#endif
