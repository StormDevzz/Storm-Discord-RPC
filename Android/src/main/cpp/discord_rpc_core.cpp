#include <discord_rpc.h>
#include <chrono>
#include <thread>
#include <memory>
#include <android/log.h>

#define LOG_TAG "DiscordCore_CPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// класс для управления discord rpc
class DiscordRPCWrapper {
private:
    bool initialized;
    std::string applicationId;
    std::thread callbackThread;
    std::atomic<bool> running;
    
public:
    DiscordRPCWrapper() : initialized(false), running(false) {
        LOGI("DiscordRPCWrapper создан");
    }
    
    ~DiscordRPCWrapper() {
        Shutdown();
    }
    
    // инициализация
    bool Initialize(const std::string& appId) {
        if (initialized) {
            LOGI("Discord RPC уже инициализирован");
            return true;
        }
        
        applicationId = appId;
        
        DiscordEventHandlers handlers;
        memset(&handlers, 0, sizeof(handlers));
        
        // обработчики событий
        handlers.ready = [](const DiscordUser* user) {
            LOGI("Discord готов! Пользователь: %s#%s", user->username, user->discriminator);
        };
        
        handlers.disconnected = [](int errcode, const char* message) {
            LOGI("Discord отключен: %d - %s", errcode, message);
        };
        
        handlers.errored = [](int errcode, const char* message) {
            LOGE("Discord ошибка: %d - %s", errcode, message);
        };
        
        Discord_Initialize(appId.c_str(), &handlers, 1, NULL);
        initialized = true;
        
        // запускаем поток для обработки callback'ов
        running = true;
        callbackThread = std::thread([this]() {
            while (running) {
                if (initialized) {
                    Discord_RunCallbacks();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
        
        LOGI("Discord RPC инициализирован: %s", appId.c_str());
        return true;
    }
    
    // обновление presence
    void UpdatePresence(const std::string& state, const std::string& details, 
                       const std::string& largeImageKey = "", 
                       const std::string& largeImageText = "",
                       const std::string& smallImageKey = "",
                       const std::string& smallImageText = "") {
        
        if (!initialized) {
            LOGE("Discord RPC не инициализирован!");
            return;
        }
        
        DiscordRichPresence presence;
        memset(&presence, 0, sizeof(presence));
        
        presence.state = state.c_str();
        presence.details = details.c_str();
        presence.startTimestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
            
        if (!largeImageKey.empty()) {
            presence.largeImageKey = largeImageKey.c_str();
            presence.largeImageText = largeImageText.c_str();
        }
        
        if (!smallImageKey.empty()) {
            presence.smallImageKey = smallImageKey.c_str();
            presence.smallImageText = smallImageText.c_str();
        }
        
        Discord_UpdatePresence(&presence);
        LOGI("Presence обновлен: %s - %s", state.c_str(), details.c_str());
    }
    
    // очистка presence
    void ClearPresence() {
        if (initialized) {
            Discord_ClearPresence();
            LOGI("Discord presence очищен");
        }
    }
    
    // выключение
    void Shutdown() {
        if (initialized) {
            running = false;
            if (callbackThread.joinable()) {
                callbackThread.join();
            }
            
            Discord_Shutdown();
            initialized = false;
            LOGI("Discord RPC выключен");
        }
    }
    
    // проверка инициализации
    bool IsInitialized() const {
        return initialized;
    }
    
    // получение application id
    std::string GetApplicationId() const {
        return applicationId;
    }
};

// глобальный экземпляр
static std::unique_ptr<DiscordRPCWrapper> g_discordWrapper;

// c функции для экспорта
extern "C" {
    
    // создание экземпляра
    void discord_create() {
        if (!g_discordWrapper) {
            g_discordWrapper = std::make_unique<DiscordRPCWrapper>();
        }
    }
    
    // инициализация
    int discord_initialize(const char* appId) {
        if (!g_discordWrapper) {
            discord_create();
        }
        return g_discordWrapper->Initialize(std::string(appId)) ? 1 : 0;
    }
    
    // обновление presence
    void discord_update_presence(const char* state, const char* details) {
        if (g_discordWrapper) {
            g_discordWrapper->UpdatePresence(
                std::string(state ? state : ""), 
                std::string(details ? details : "")
            );
        }
    }
    
    // обновление presence с изображениями
    void discord_update_presence_full(const char* state, const char* details,
                                    const char* largeImageKey, const char* largeImageText,
                                    const char* smallImageKey, const char* smallImageText) {
        if (g_discordWrapper) {
            g_discordWrapper->UpdatePresence(
                std::string(state ? state : ""), 
                std::string(details ? details : ""),
                std::string(largeImageKey ? largeImageKey : ""),
                std::string(largeImageText ? largeImageText : ""),
                std::string(smallImageKey ? smallImageKey : ""),
                std::string(smallImageText ? smallImageText : "")
            );
        }
    }
    
    // очистка presence
    void discord_clear_presence() {
        if (g_discordWrapper) {
            g_discordWrapper->ClearPresence();
        }
    }
    
    // выключение
    void discord_shutdown() {
        if (g_discordWrapper) {
            g_discordWrapper->Shutdown();
        }
    }
    
    // проверка инициализации
    int discord_is_initialized() {
        return (g_discordWrapper && g_discordWrapper->IsInitialized()) ? 1 : 0;
    }
    
    // уничтожение экземпляра
    void discord_destroy() {
        if (g_discordWrapper) {
            g_discordWrapper->Shutdown();
            g_discordWrapper.reset();
        }
    }
}
