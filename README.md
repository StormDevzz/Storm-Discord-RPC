# Storm Discord RPC - Android

Android приложение для установки кастомного статуса в Discord Rich Presence.

## Особенности

- Установка кастомного статуса Discord
- Поддержка деталей и состояния
- Фоновая работа через сервис
- Современный Material Design интерфейс

## Сборка APK

1. Убедитесь, что у вас установлен Android SDK
2. Откройте терминал в папке проекта
3. Выполните команду:

```bash
./gradlew assembleDebug
```

APK файл будет создан в `app/build/outputs/apk/debug/app-debug.apk`

## Установка

```bash
adb install app/build/outputs/apk/debug/app-debug.apk
```

## Использование

1. Откройте приложение StormDiscordRPC
2. Введите желаемый статус в поле "Status"
3. Опционально добавьте детали и состояние
4. Нажмите "Start RPC" для запуска
5. Для остановки нажмите "Stop RPC"

## Требования

- Android 5.0 (API 21) и выше
- Разрешение на использование интернета
- Разрешение на фоновые сервисы

## Структура проекта

- `MainActivity.java` - Основной экран с UI
- `DiscordRPCService.java` - Фоновый сервис для Discord RPC
- `DiscordRPCManager.java` - Менеджер для работы с Discord RPC

## Примечание

Для полноценной работы Discord RPC требуется интеграция с нативной библиотекой Discord RPC. В текущей версии реализована заглушка для демонстрации функционала.
