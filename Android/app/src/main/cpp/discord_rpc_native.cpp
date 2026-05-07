#include <jni.h>
#include <string>
#include <android/log.h>
#include "../../shared/include/discord_rpc.h"

#define LOG_TAG "DiscordRPC"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// глобальный экземпляр Discord RPC
static DiscordRPC* g_discord_rpc = nullptr;

extern "C" {

// инициализация Discord RPC
JNIEXPORT jboolean JNICALL
Java_com_storm_discordrpc_DiscordRPCNative_initialize(JNIEnv *env, jobject thiz, jstring client_id) {
    if (!g_discord_rpc) {
        g_discord_rpc = new DiscordRPC();
    }
    
    const char* client_id_str = env->GetStringUTFChars(client_id, nullptr);
    bool result = g_discord_rpc->initialize(std::string(client_id_str));
    env->ReleaseStringUTFChars(client_id, client_id_str);
    
    LOGI("инициализация Discord RPC: %s", result ? "успешно" : "ошибка");
    return result;
}

// обновление статуса
JNIEXPORT void JNICALL
Java_com_storm_discordrpc_DiscordRPCNative_updatePresence(JNIEnv *env, jobject thiz,
                                                         jstring state, jstring details,
                                                         jstring large_image_key, jstring large_image_text,
                                                         jstring small_image_key, jstring small_image_text) {
    if (!g_discord_rpc) {
        LOGE("discord rpc не инициализирован");
        return;
    }
    
    std::string state_str = state ? env->GetStringUTFChars(state, nullptr) : "";
    std::string details_str = details ? env->GetStringUTFChars(details, nullptr) : "";
    std::string large_image_key_str = large_image_key ? env->GetStringUTFChars(large_image_key, nullptr) : "";
    std::string large_image_text_str = large_image_text ? env->GetStringUTFChars(large_image_text, nullptr) : "";
    std::string small_image_key_str = small_image_key ? env->GetStringUTFChars(small_image_key, nullptr) : "";
    std::string small_image_text_str = small_image_text ? env->GetStringUTFChars(small_image_text, nullptr) : "";
    
    g_discord_rpc->updatePresence(state_str, details_str, 
                                  large_image_key_str, large_image_text_str,
                                  small_image_key_str, small_image_text_str);
    
    if (state) env->ReleaseStringUTFChars(state, state_str.c_str());
    if (details) env->ReleaseStringUTFChars(details, details_str.c_str());
    if (large_image_key) env->ReleaseStringUTFChars(large_image_key, large_image_key_str.c_str());
    if (large_image_text) env->ReleaseStringUTFChars(large_image_text, large_image_text_str.c_str());
    if (small_image_key) env->ReleaseStringUTFChars(small_image_key, small_image_key_str.c_str());
    if (small_image_text) env->ReleaseStringUTFChars(small_image_text, small_image_text_str.c_str());
    
    LOGI("статус обновлен");
}

// отключение
JNIEXPORT void JNICALL
Java_com_storm_discordrpc_DiscordRPCNative_disconnect(JNIEnv *env, jobject thiz) {
    if (g_discord_rpc) {
        g_discord_rpc->disconnect();
        delete g_discord_rpc;
        g_discord_rpc = nullptr;
        LOGI("discord rpc отключен");
    }
}

// обработка событий
JNIEXPORT void JNICALL
Java_com_storm_discordrpc_DiscordRPCNative_processEvents(JNIEnv *env, jobject thiz) {
    if (g_discord_rpc) {
        g_discord_rpc->processEvents();
    }
}

} // extern "C"
