#include "discord_rpc_real.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cctype>
#include <regex>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <random>
#include <iomanip>

// реальная реализация Discord RPC через IPC сокеты
class DiscordIPC {
private:
    int socket_fd;
    bool connected;
    bool initialized;
    std::string client_id;
    std::string last_error;
    std::string ipc_path;
    
    // функция для экранирования JSON
    std::string escape_json(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
    
    // функция для генерации nonce
    std::string generate_nonce() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (int i = 0; i < 16; i++) {
            oss << std::setw(2) << dis(gen);
        }
        return oss.str();
    }
    
    // поиск IPC сокета Discord
    std::string findDiscordSocket() {
        // сначала проверяем стандартные пути в /run/user
        const char* run_user = getenv("USER");
        if (run_user) {
            char uid_str[32];
            snprintf(uid_str, sizeof(uid_str), "%d", getuid());
            
            std::string run_path = "/run/user/" + std::string(uid_str);
            
            // проверяем discord-ipc-0, discord-ipc-1, etc.
            for (int i = 0; i < 10; i++) {
                std::string ipc_path = run_path + "/discord-ipc-" + std::to_string(i);
                if (access(ipc_path.c_str(), R_OK | W_OK) == 0) {
                    return ipc_path;
                }
            }
        }
        
        // пути для поиска Discord IPC сокетов в старых версиях
        const char* home = getenv("HOME");
        if (home) {
            std::vector<std::string> paths = {
                std::string(home) + "/.config/discord/",
                std::string(home) + "/.config/discordcanary/",
                std::string(home) + "/.config/discordptb/",
                "/tmp/",
                "/var/tmp/"
            };
            
            for (const auto& base_path : paths) {
                DIR* dir = opendir(base_path.c_str());
                if (!dir) continue;
                
                struct dirent* entry;
                while ((entry = readdir(dir)) != nullptr) {
                    if (strncmp(entry->d_name, "discord-ipc-", 12) == 0) {
                        std::string full_path = base_path + entry->d_name;
                        if (access(full_path.c_str(), R_OK | W_OK) == 0) {
                            closedir(dir);
                            return full_path;
                        }
                    }
                }
                closedir(dir);
            }
        }
        
        return "";
    }
    
    // подключение к IPC сокету
    bool connectToSocket(const std::string& path) {
        socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            last_error = "не удалось создать сокет";
            return false;
        }
        
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);
        
        if (connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(socket_fd);
            last_error = "не удалось подключиться к сокету: " + std::string(strerror(errno));
            return false;
        }
        
        // установка неблокирующего режима
        int flags = fcntl(socket_fd, F_GETFL, 0);
        fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
        
        return true;
    }
    
    // отправка JSON команды в Discord
    bool sendCommand(const std::string& command) {
        if (!connected || socket_fd < 0) {
            last_error = "не подключен к Discord";
            return false;
        }
        
        // Discord ожидает little-endian длину сообщения
        uint32_t length = command.length();
        uint32_t length_le = htole32(length);
        
        // отправляем длину
        ssize_t sent = send(socket_fd, &length_le, sizeof(length_le), 0);
        if (sent < 0) {
            last_error = "ошибка отправки длины: " + std::string(strerror(errno));
            return false;
        }
        
        // отправляем данные
        sent = send(socket_fd, command.c_str(), length, 0);
        if (sent < 0) {
            last_error = "ошибка отправки данных: " + std::string(strerror(errno));
            return false;
        }
        
        return true;
    }
    
    // чтение ответа от Discord
    std::string readResponse() {
        if (!connected || socket_fd < 0) return "";
        
        // читаем длину с неблокирующим чтением
        uint32_t length_le = 0;
        ssize_t total_received = 0;
        
        while (total_received < sizeof(length_le)) {
            ssize_t received = recv(socket_fd, ((char*)&length_le) + total_received, sizeof(length_le) - total_received, MSG_DONTWAIT);
            if (received <= 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    return ""; // нет данных, выходим
                }
                return "";
            }
            total_received += received;
        }
        
        uint32_t length = le32toh(length_le);
        std::cout << "DEBUG: Получена длина сообщения: " << length << std::endl;
        
        if (length == 0 || length > 8192) {
            std::cout << "DEBUG: Неверная длина сообщения" << std::endl;
            return "";
        }
        
        // читаем данные с неблокирующим чтением
        std::string buffer(length, '\0');
        total_received = 0;
        
        while (total_received < length) {
            ssize_t received = recv(socket_fd, &buffer[0] + total_received, length - total_received, MSG_DONTWAIT);
            if (received <= 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    return ""; // нет данных, выходим
                }
                std::cout << "DEBUG: Ошибка чтения данных: " << strerror(errno) << std::endl;
                return "";
            }
            total_received += received;
        }
        
        std::cout << "DEBUG: Прочитано байт: " << total_received << std::endl;
        return buffer;
    }
    
public:
    DiscordIPC() : socket_fd(-1), connected(false), initialized(false) {}
    
    ~DiscordIPC() {
        disconnect();
    }
    
    bool initialize(const std::string& id) {
        client_id = id;
        
        // поиск IPC сокета
        ipc_path = findDiscordSocket();
        if (ipc_path.empty()) {
            last_error = "Discord IPC сокет не найден. Убедитесь что Discord запущен.";
            return false;
        }
        
        // подключение к сокету
        if (!connectToSocket(ipc_path)) {
            return false;
        }
        
        connected = true;
        
        // отправка команды рукопожатия
        std::ostringstream handshake;
        handshake << "{\"cmd\":\"HANDSHAKE\",\"args\":{\"v\":1,\"client_id\":\"" << client_id << "\"}}";
        
        std::cout << "DEBUG: Отправляем рукопожатие: " << handshake.str() << std::endl;
        std::cout << "DEBUG: Длина рукопожатия: " << handshake.str().length() << " байт" << std::endl;
        
        if (!sendCommand(handshake.str())) {
            std::cout << "DEBUG: Ошибка отправки рукопожатия: " << last_error << std::endl;
            close(socket_fd);
            connected = false;
            return false;
        }
        
        std::cout << "DEBUG: Рукопожатие отправлено, ждем ответ..." << std::endl;
        
        // неблокирующее ожидание ответа
        std::string response = readResponse();
        if (!response.empty()) {
            std::cout << "DEBUG: Получен ответ на рукопожатие: '" << response << "'" << std::endl;
            
            // выводим байты в hex формате
            std::cout << "DEBUG: Байты ответа: ";
            for (size_t i = 0; i < response.length(); i++) {
                printf("%02x ", (unsigned char)response[i]);
            }
            std::cout << std::endl;
            
            if (response.find("\"evt\":\"READY\"") != std::string::npos) {
                std::cout << "DEBUG: Discord готов к работе!" << std::endl;
                initialized = true;
                return true;
            } else if (response.find("\"evt\":\"ERROR\"") != std::string::npos) {
                std::cout << "DEBUG: Ошибка в ответе на рукопожатие!" << std::endl;
                last_error = "Discord вернул ошибку при рукопожатии";
                close(socket_fd);
                connected = false;
                return false;
            } else if (response.length() > 0) {
                std::cout << "DEBUG: Ответ содержит данные, но не READY или ERROR" << std::endl;
                // пробуем продолжить - возможно это нормально
                initialized = true;
                return true;
            } else {
                std::cout << "DEBUG: Пустой ответ на рукопожатие" << std::endl;
            }
        } else {
            std::cout << "DEBUG: Нет ответа на рукопожатие, продолжаем..." << std::endl;
            // считаем что рукопожатие прошло успешно
            initialized = true;
            return true;
        }
        
        last_error = "таймаут рукопожатия с Discord";
        close(socket_fd);
        connected = false;
        return false;
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
        
        // создаем JSON для SET_ACTIVITY согласно документации Discord
        std::ostringstream json;
        json << "{\"cmd\":\"SET_ACTIVITY\",\"args\":{\"pid\":" << getpid() << ",\"activity\":{";
        
        bool has_content = false;
        
        // добавляем state если есть
        if (!state.empty()) {
            json << "\"state\":\"" << escape_json(state) << "\"";
            has_content = true;
        }
        
        // добавляем details если есть
        if (!details.empty()) {
            if (has_content) json << ",";
            json << "\"details\":\"" << escape_json(details) << "\"";
            has_content = true;
        }
        
        // добавляем timestamps
        if (has_content) json << ",";
        json << "\"timestamps\":{\"start\":" << time(nullptr) << "}";
        
        // добавляем assets если есть изображения
        if (!large_image_key.empty()) {
            json << ",\"assets\":{";
            json << "\"large_image\":\"" << escape_json(large_image_key) << "\"";
            
            if (!large_image_text.empty()) {
                json << ",\"large_text\":\"" << escape_json(large_image_text) << "\"";
            }
            
            if (!small_image_key.empty()) {
                json << ",\"small_image\":\"" << escape_json(small_image_key) << "\"";
            }
            
            if (!small_image_text.empty()) {
                json << ",\"small_text\":\"" << escape_json(small_image_text) << "\"";
            }
            
            json << "}";
        }
        
        json << "}}";
        
        // добавляем nonce для отслеживания
        json << ",\"nonce\":\"" << generate_nonce() << "\"}";
        
        std::string command = json.str();
        
        std::cout << "DEBUG: Отправляем JSON: " << command << std::endl;
        
        if (sendCommand(command)) {
            std::cout << "DEBUG: Команда отправлена успешно" << std::endl;
            
            // неблокирующее чтение подтверждения
            std::string response = readResponse();
            if (!response.empty()) {
                std::cout << "DEBUG: Получен ответ: " << response << std::endl;
                
                // проверяем успешность ответа
                if (response.find("\"evt\":\"SET_ACTIVITY\"") != std::string::npos) {
                    std::cout << "DEBUG: Discord подтвердил получение статуса!" << std::endl;
                } else if (response.find("\"evt\":\"ERROR\"") != std::string::npos) {
                    std::cout << "DEBUG: Discord вернул ошибку!" << std::endl;
                } else {
                    std::cout << "DEBUG: Неизвестный ответ от Discord" << std::endl;
                }
            } else {
                std::cout << "DEBUG: Нет ответа от Discord, но продолжаем..." << std::endl;
            }
        } else {
            std::cout << "DEBUG: Ошибка отправки команды: " << last_error << std::endl;
        }
    }
    
    void disconnect() {
        if (connected && socket_fd >= 0) {
            // отправка команды закрытия
            sendCommand("{\"cmd\":\"CLOSE_ACTIVITY\",\"args\":{}}");
            
            close(socket_fd);
            socket_fd = -1;
            connected = false;
            initialized = false;
        }
    }
    
    bool isConnected() const { return connected; }
    bool isInitialized() const { return initialized; }
    std::string getLastError() const { return last_error; }
    void clearError() { last_error.clear(); }
    
    void processEvents() {
        // обработка входящих сообщений от Discord
        if (connected && socket_fd >= 0) {
            readResponse();
        }
    }
};

// реализация методов DiscordRPCReal через IPC
DiscordRPCReal::DiscordRPCReal() : p_impl(new DiscordIPC()) {}

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
        connected = true;
        initialized = true;
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
    // проверка запущен ли Discord
    return system("pgrep discord >/dev/null 2>&1") == 0;
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
