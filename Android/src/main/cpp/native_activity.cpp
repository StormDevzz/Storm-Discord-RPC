#include <android/native_activity.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <string.h>
#include <stdlib.h>

#define LOG_TAG "NativeActivity_CPP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// структура для хранения данных активности
typedef struct {
    ANativeActivity* activity;
    JavaVM* javaVM;
    jobject discordRPCManager;
    bool initialized;
} ActivityData;

static ActivityData g_activityData = {0};

// обработчик запуска активности
static void onStart(ANativeActivity* activity) {
    LOGI("Native activity запущена");
}

// обработчик возобновления активности
static void onResume(ANativeActivity* activity) {
    LOGI("Native activity возобновлена");
}

// обработчик паузы активности
static void onPause(ANativeActivity* activity) {
    LOGI("Native activity приостановлена");
}

// обработчик остановки активности
static void onStop(ANativeActivity* activity) {
    LOGI("Native activity остановлена");
}

// обработчик уничтожения активности
static void onDestroy(ANativeActivity* activity) {
    LOGI("Native activity уничтожена");
    
    // очищаем ресурсы
    if (g_activityData.discordRPCManager) {
        // здесь можно добавить код для корректного завершения discord rpc
        JNIEnv* env;
        if (activity->vm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_OK) {
            if (g_activityData.discordRPCManager) {
                env->DeleteGlobalRef(g_activityData.discordRPCManager);
            }
        }
    }
    
    memset(&g_activityData, 0, sizeof(g_activityData));
}

// обработчик изменения конфигурации
static void onConfigurationChanged(ANativeActivity* activity) {
    LOGI("Конфигурация native activity изменена");
}

// обработчик низкой памяти
static void onLowMemory(ANativeActivity* activity) {
    LOGI("Низкая память в native activity");
}

// обработчик сохранения состояния
static void* onSaveInstanceState(ANativeActivity* activity, size_t* outLen) {
    LOGI("Сохранение состояния native activity");
    *outLen = 0;
    return NULL;
}

// обработчик восстановления состояния
static void onRestoreInstanceState(ANativeActivity* activity, void* state, size_t len) {
    LOGI("Восстановление состояния native activity");
}

// обработчик фокуса окна
static void onWindowFocusChanged(ANativeActivity* activity, bool hasFocus) {
    LOGI("Фокус окна изменен: %s", hasFocus ? "получен" : "потерян");
}

// обработчик входных событий
static void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("Native окно создано");
}

static void onNativeWindowResized(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("Native окно изменено");
}

static void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("Native окно уничтожено");
}

// главная функция native activity
void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    LOGI("Создание native activity");
    
    // сохраняем указатель на активность
    g_activityData.activity = activity;
    g_activityData.javaVM = activity->vm;
    g_activityData.initialized = false;
    
    // устанавливаем обработчики событий
    activity->callbacks->onStart = onStart;
    activity->callbacks->onResume = onResume;
    activity->callbacks->onPause = onPause;
    activity->callbacks->onStop = onStop;
    activity->callbacks->onDestroy = onDestroy;
    activity->callbacks->onConfigurationChanged = onConfigurationChanged;
    activity->callbacks->onLowMemory = onLowMemory;
    activity->callbacks->onSaveInstanceState = onSaveInstanceState;
    activity->callbacks->onRestoreInstanceState = onRestoreInstanceState;
    activity->callbacks->onWindowFocusChanged = onWindowFocusChanged;
    activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
    activity->callbacks->onNativeWindowResized = onNativeWindowResized;
    activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
    
    LOGI("Native activity успешно создана");
}

// функция для получения JNIEnv
JNIEnv* getJNIEnv() {
    JNIEnv* env = NULL;
    if (g_activityData.javaVM) {
        g_activityData.javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
    }
    return env;
}

// функция для инициализации discord rpc из native кода
int initializeDiscordRPCFromNative(const char* appId) {
    JNIEnv* env = getJNIEnv();
    if (!env) {
        LOGE("Не удалось получить JNIEnv");
        return -1;
    }
    
    // здесь можно добавить код для вызова Java методов из native кода
    LOGI("Инициализация Discord RPC из native кода: %s", appId);
    
    return 0;
}

// функция для обновления presence из native кода
int updatePresenceFromNative(const char* state, const char* details) {
    JNIEnv* env = getJNIEnv();
    if (!env) {
        LOGE("Не удалось получить JNIEnv");
        return -1;
    }
    
    LOGI("Обновление presence из native кода: %s - %s", state, details);
    
    return 0;
}
