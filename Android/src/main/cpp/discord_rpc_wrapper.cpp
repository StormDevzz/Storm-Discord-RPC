#include <jni.h>
#include <string>
#include <discord_rpc.h>
#include <android/log.h>

#define LOG_TAG "DiscordRPC_CPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// глобальные переменные для discord rpc
static bool initialized = false;
static std::string currentApplicationId;

// обработчики событий discord rpc
void onReady(const DiscordUser* connectedUser) {
    LOGI("Discord RPC готов! Пользователь: %s#%s", 
         connectedUser->username, connectedUser->discriminator);
}

void onDisconnected(int errcode, const char* message) {
    LOGI("Discord RPC отключен: код %d, сообщение: %s", errcode, message);
}

void onError(int errcode, const char* message) {
    LOGE("Discord RPC ошибка: код %d, сообщение: %s", errcode, message);
}

// инициализация discord rpc
extern "C" JNIEXPORT void JNICALL
Java_com_storm_discordrpc_DiscordRPCManager_Discord_1Initialize(JNIEnv* env, jobject obj, jstring applicationId) {
    
    const char* appId = env->GetStringUTFChars(applicationId, 0);
    currentApplicationId = std::string(appId);
    
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = onReady;
    handlers.disconnected = onDisconnected;
    handlers.errored = onError;
    
    Discord_Initialize(appId, &handlers, 1, NULL);
    initialized = true;
    
    LOGI("Discord RPC инициализирован с app id: %s", appId);
    env->ReleaseStringUTFChars(applicationId, appId);
}

// обновление presence
extern "C" JNIEXPORT void JNICALL
Java_com_storm_discordrpc_DiscordRPCManager_Discord_1UpdatePresence(JNIEnv* env, jobject obj, jstring state, jstring details) {
    
    if (!initialized) {
        LOGE("Discord RPC не инициализирован!");
        return;
    }
    
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    
    const char* stateStr = env->GetStringUTFChars(state, 0);
    const char* detailsStr = env->GetStringUTFChars(details, 0);
    
    discordPresence.state = stateStr;
    discordPresence.details = detailsStr;
    discordPresence.startTimestamp = time(0);
    
    Discord_UpdatePresence(&discordPresence);
    
    LOGI("Discord presence обновлен: state=%s, details=%s", stateStr, detailsStr);
    
    env->ReleaseStringUTFChars(state, stateStr);
    env->ReleaseStringUTFChars(details, detailsStr);
}

// выключение discord rpc
extern "C" JNIEXPORT void JNICALL
Java_com_storm_discordrpc_DiscordRPCManager_Discord_1Shutdown(JNIEnv* env, jobject obj) {
    
    if (initialized) {
        Discord_Shutdown();
        initialized = false;
        LOGI("Discord RPC выключен");
    }
}

// обработка callback'ов discord rpc
extern "C" JNIEXPORT void JNICALL
Java_com_storm_discordrpc_DiscordRPCManager_Discord_1RunCallbacks(JNIEnv* env, jobject obj) {
    if (initialized) {
        Discord_RunCallbacks();
    }
}

// проверка инициализации
extern "C" JNIEXPORT jboolean JNICALL
Java_com_storm_discordrpc_DiscordRPCManager_IsInitialized(JNIEnv* env, jobject obj) {
    return initialized ? JNI_TRUE : JNI_FALSE;
}
