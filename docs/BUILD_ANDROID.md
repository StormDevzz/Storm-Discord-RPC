# Сборка для Android

## Требования

### Системные требования
- Android Studio Arctic Fox+ (2020.3.1+)
- Android SDK 30 (API 30)+
- Kotlin 1.6+
- Java 11+
- Gradle 7.0+

### Установка Android Studio

1. Скачайте [Android Studio](https://developer.android.com/studio)
2. Установите и запустите
3. В настройках установите:
   - Android SDK Platform 30+
   - Android SDK Build-Tools 30.0.3+
   - Android NDK (Side by side) 21+
   - Android SDK Command-line Tools

## Сборка через Android Studio

1. **Клонирование репозитория**
```bash
git clone https://github.com/StormDevzz/Storm-Discord-RPC.git
cd Storm-Discord-RPC
```

2. **Открытие проекта**
   - Откройте Android Studio
   - Выберите "Open an existing project"
   - Выберите директорию `Android/`

3. **Настройка проекта**
   - Дождитесь завершения Gradle sync
   - Выберите build variant: `debug` или `release`

4. **Сборка**
   - Выберите `Build` → `Build Bundle(s) / APK(s)` → `Build APK(s)`
   - APK будет в `Android/app/build/outputs/apk/debug/`

## Сборка через командную строку

1. **Настройка окружения**
```bash
export ANDROID_HOME=/path/to/android/sdk
export PATH=$PATH:$ANDROID_HOME/tools:$ANDROID_HOME/platform-tools
```

2. **Сборка**
```bash
cd Android
./gradlew assembleDebug
```

3. **Установка на устройство**
```bash
./gradlew installDebug
```

## Создание релизной версии

### Генерация подписанного APK

1. **Создание хранилища ключей**
```bash
keytool -genkey -v -keystore release-key.keystore -alias release -keyalg RSA -keysize 2048 -validity 10000
```

2. **Настройка подписи**
   - Создайте файл `Android/app/release-keystore.properties`:
```properties
storeFile=../release-key.keystore
storePassword=your_password
keyAlias=release
keyPassword=your_password
```

3. **Сборка релизного APK**
```bash
./gradlew assembleRelease
```

### Создание App Bundle

```bash
./gradlew bundleRelease
```

App Bundle будет в `Android/app/build/outputs/bundle/release/`

## Траблшутинг

### Ошибки Gradle

**Ошибка: SDK не найден**
```bash
# Проверьте ANDROID_HOME
echo $ANDROID_HOME
ls $ANDROID_HOME/platforms/
```

**Ошибка: NDK не найден**
- Установите NDK через Android Studio SDK Manager
- Или скачайте вручную с [Android NDK downloads](https://developer.android.com/ndk/downloads)

**Ошибка: Дублирование зависимостей**
```bash
./gradlew clean
./gradlew build --refresh-dependencies
```

### Ошибки сборки

**Ошибка: Kotlin версия**
```bash
# Обновите Kotlin в build.gradle
ext.kotlin_version = '1.7.20'
```

**Ошибка: Compile SDK**
```bash
# Установите необходимый SDK
sdkmanager "platforms;android-33"
```

### Ошибки при запуске

**Ошибка: Разрешения**
- Убедитесь что в `AndroidManifest.xml` есть необходимые разрешения:
```xml
<uses-permission android:name="android.permission.INTERNET" />
<uses-permission android:name="android.permission.FOREGROUND_SERVICE" />
```

**Ошибка: Python модуль**
- Убедитесь что Python установлен на устройстве
- Используйте Termux для запуска Python скриптов

## Отладка

### Отладка через Android Studio
1. Подключите устройство с включенной отладкой по USB
2. Нажмите Shift+F9 или выберите "Run" → "Debug"
3. Используйте Logcat для просмотра логов

### Отладка через ADB
```bash
# Просмотр логов
adb logcat -s StormDiscordRPC

# Просмотр ошибок
adb logcat -s AndroidRuntime:E

# Установка APK
adb install app-debug.apk
```

### Удаленная отладка
```bash
# Подключение к удаленному устройству
adb connect 192.168.1.100:5555
```

## Структура проекта

```
Android/
├── app/
│   ├── src/main/
│   │   ├── java/com/storm/discordrpc/
│   │   │   ├── MainActivity.kt
│   │   │   ├── DiscordRPC.kt
│   │   │   └── PythonBridge.kt
│   │   ├── res/
│   │   │   ├── layout/
│   │   │   ├── values/
│   │   │   └── drawable/
│   │   └── AndroidManifest.xml
│   ├── build.gradle
│   └── proguard-rules.pro
├── build.gradle
├── gradle.properties
└── settings.gradle
```

## Настройка проекта

### build.gradle (Module)
```kotlin
android {
    compileSdk 33
    
    defaultConfig {
        applicationId "com.storm.discordrpc"
        minSdk 30
        targetSdk 33
        versionCode 1
        versionName "1.0"
    }
    
    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt'), 'proguard-rules.pro'
        }
    }
    
    compileOptions {
        sourceCompatibility JavaVersion.VERSION_11
        targetCompatibility JavaVersion.VERSION_11
    }
    
    kotlinOptions {
        jvmTarget = '11'
    }
}

dependencies {
    implementation 'androidx.core:core-ktx:1.9.0'
    implementation 'androidx.appcompat:appcompat:1.6.1'
    implementation 'com.google.android.material:material:1.8.0'
    implementation 'androidx.constraintlayout:constraintlayout:2.1.4'
}
```

### Настройка Python интеграции

Для работы с Python на Android:
1. Установите [Termux](https://play.google.com/store/apps/details?id=com.termux)
2. Установите Python в Termux:
```bash
pkg install python
pip install pypresence
```

3. Используйте PythonBridge для вызова Python скриптов

## Тестирование

### Юнит-тесты
```bash
./gradlew test
```

### Инструментальные тесты
```bash
./gradlew connectedAndroidTest
```

### Тестирование на разных устройствах
```bash
# Список подключенных устройств
adb devices

# Установка на все устройства
for device in $(adb devices | grep -v "List" | cut -f1); do
    adb -s $device install app-debug.apk
done
```

## Публикация в Google Play

### Подготовка релиза
1. Соберите подписанный App Bundle
2. Проверьте через [Play Console](https://play.google.com/console)
3. Загрузите Bundle
4. Заполните информацию о приложении
5. Отправьте на проверку

### Требования Google Play
- API Level 30+
- 64-bit ARMv8 или x86_64
- Подписанный APK/App Bundle
- Соответствие политикам контента

## Поддерживаемые устройства

- Android 11+ (API 30+)
- ARMv8 или x86_64 архитектура
- 2GB+ RAM рекомендуется
- Поддержка Python (через Termux)

## Альтернативные способы сборки

### Сборка через GitHub Actions
Создайте `.github/workflows/build.yml` для автоматической сборки.

### Сборка Docker
```bash
docker build -t storm-discord-rpc-android .
docker run -v $(pwd)/Android:/app storm-discord-rpc-android
```

## Оптимизация

### Уменьшение размера APK
```gradle
android {
    buildTypes {
        release {
            shrinkResources true
            minifyEnabled true
        }
    }
}
```

### Оптимизация производительности
- Используйте ProGuard/R8
- Оптимизируйте изображения
- Используйте code shrinking
