# Storm Discord RPC

Кроссплатформенное приложение для управления Discord Rich Presence с поддержкой Linux, Windows и Android.

## Особенности

- **Кроссплатформенность**: Работает на Linux, Windows и Android
- **Современный интерфейс**: GTKmm 4.0 для Linux, WinForms для Windows, Kotlin для Android
- **Python интеграция**: Использует pypresence для надежной работы с Discord
- **Сохранение конфигураций**: Автоматически сохраняет Client ID и последний статус
- **Очистка статуса**: Корректно отключает статус при выходе из программы
- **Пресеты**: Быстрая установка сохраненных статусов
- **Детальная отладка**: Полная информация о работе с Discord IPC

## Установка

### Linux

#### Требования
- GTKmm 4.0
- CMake 3.16+
- GCC 9.0+
- Python 3.8+ с pypresence

#### Сборка из исходников

```bash
# Клонирование репозитория
git clone https://github.com/StormDevzz/Storm-Discord-RPC.git
cd Storm-Discord-RPC

# Установка зависимостей
sudo apt update
sudo apt install libgtkmm-4.0-dev cmake build-essential python3-pip

# Установка pypresence
pip3 install pypresence --break-system-packages

# Сборка
cd Linux
mkdir build
cd build
cmake ..
make

# Запуск
./storm-discord-rpc
```

### Windows

#### Требования
- .NET 6.0
- Visual Studio 2019+
- Python 3.8+ с pypresence

#### Сборка

```cmd
# Клонирование репозитория
git clone https://github.com/StormDevzz/Storm-Discord-RPC.git
cd Storm-Discord-RPC

# Сборка через Visual Studio
cd Windows
dotnet build
```

### Android

#### Требования
- Android Studio Arctic Fox+
- Kotlin 1.6+
- Android SDK 30+

#### Сборка

```bash
# Открыть в Android Studio
cd Android
./gradlew assembleDebug
```

## Использование

1. **Получите Application ID**:
   - Перейдите на [Discord Developer Portal](https://discord.com/developers/applications)
   - Создайте новое приложение
   - Скопируйте Application ID (17-19 цифр)

2. **Запустите приложение**:
   - Введите Client ID в поле подключения
   - Нажмите "Подключиться"

3. **Настройте статус**:
   - Введите состояние и детали
   - Добавьте изображения при необходимости
   - Нажмите "Обновить статус"

4. **Управление**:
   - "Очистить поля" - очищает все поля ввода
   - "Очистить статус Discord" - удаляет текущий статус в Discord
   - "Отключиться" - разрывает соединение с Discord

## Структура проекта

```
Storm-Discord-RPC/
├── Linux/                 # Linux версия (C++/GTKmm)
├── Windows/               # Windows версия (C#/WinForms)
├── Android/               # Android версия (Kotlin)
├── shared/                # Общие компоненты
│   ├── include/           # Заголовочные файлы
│   ├── src/              # Исходные коды
│   └── python/            # Python скрипты
├── external/              # Внешние библиотеки
└── docs/                 # Документация
```

## Конфигурация

Конфигурационные файлы сохраняются в:
- **Linux**: `~/.config/storm-discord-rpc/config.json`
- **Windows**: `%APPDATA%/Storm-Discord-RPC/config.json`
- **Android**: `/data/data/com.storm.discordrpc/config.json`

## Отладка

Для включения детальной отладки установите переменную окружения:

```bash
export STORM_DISCORD_DEBUG=1
```

## Траблшутинг

### Статус не обновляется
1. Проверьте что Discord запущен
2. Убедитесь что Client ID корректный (17-19 цифр)
3. Проверьте наличие pypresence: `pip3 show pypresence`
4. Проверьте права доступа к IPC сокетам Discord

### Интерфейс не запускается
1. Установите все зависимости для вашей платформы
2. Проверьте версию GTKmm (требуется 4.0+)
3. Убедитесь что Python 3.8+ установлен

## Вклад в проект

1. Форкните репозиторий
2. Создайте ветку: `git checkout -b feature/amazing-feature`
3. Внесите изменения
4. Отправьте пулл-реквест

## Лицензия

Этот проект лицензирован под MIT License - см. файл [LICENSE](LICENSE).

# Storm Discord RPC

Cross-platform Discord Rich Presence management application with support for Linux, Windows and Android.

## Features

- **Cross-platform**: Works on Linux, Windows and Android
- **Modern interface**: GTKmm 4.0 for Linux, WinForms for Windows, Kotlin for Android
- **Python integration**: Uses pypresence for reliable Discord communication
- **Configuration saving**: Automatically saves Client ID and last status
- **Status clearing**: Properly clears status on application exit
- **Presets**: Quick setup of saved statuses
- **Detailed debugging**: Full information about Discord IPC operations

## Installation

### Linux

#### Requirements
- GTKmm 4.0
- CMake 3.16+
- GCC 9.0+
- Python 3.8+ with pypresence

#### Build from source

```bash
# Clone repository
git clone https://github.com/StormDevzz/Storm-Discord-RPC.git
cd Storm-Discord-RPC

# Install dependencies
sudo apt update
sudo apt install libgtkmm-4.0-dev cmake build-essential python3-pip

# Install pypresence
pip3 install pypresence --break-system-packages

# Build
cd Linux
mkdir build
cd build
cmake ..
make

# Run
./storm-discord-rpc
```

### Windows

#### Requirements
- .NET 6.0
- Visual Studio 2019+
- Python 3.8+ with pypresence

#### Build

```cmd
# Clone repository
git clone https://github.com/StormDevzz/Storm-Discord-RPC.git
cd Storm-Discord-RPC

# Build with Visual Studio
cd Windows
dotnet build
```

### Android

#### Requirements
- Android Studio Arctic Fox+
- Kotlin 1.6+
- Android SDK 30+

#### Build

```bash
# Open in Android Studio
cd Android
./gradlew assembleDebug
```

## Usage

1. **Get Application ID**:
   - Go to [Discord Developer Portal](https://discord.com/developers/applications)
   - Create new application
   - Copy Application ID (17-19 digits)

2. **Run application**:
   - Enter Client ID in connection field
   - Click "Connect"

3. **Configure status**:
   - Enter state and details
   - Add images if needed
   - Click "Update Status"

4. **Management**:
   - "Clear Fields" - clears all input fields
   - "Clear Discord Status" - removes current status in Discord
   - "Disconnect" - breaks connection with Discord

## Project Structure

```
Storm-Discord-RPC/
├── Linux/                 # Linux version (C++/GTKmm)
├── Windows/               # Windows version (C#/WinForms)
├── Android/               # Android version (Kotlin)
├── shared/                # Shared components
│   ├── include/           # Header files
│   ├── src/              # Source codes
│   └── python/            # Python scripts
├── external/              # External libraries
└── docs/                 # Documentation
```

## Configuration

Configuration files are saved in:
- **Linux**: `~/.config/storm-discord-rpc/config.json`
- **Windows**: `%APPDATA%/Storm-Discord-RPC/config.json`
- **Android**: `/data/data/com.storm.discordrpc/config.json`

## Debugging

To enable detailed debugging, set environment variable:

```bash
export STORM_DISCORD_DEBUG=1
```

## Troubleshooting

### Status not updating
1. Check that Discord is running
2. Ensure Client ID is correct (17-19 digits)
3. Check pypresence installation: `pip3 show pypresence`
4. Check Discord IPC socket access permissions

### Interface not starting
1. Install all dependencies for your platform
2. Check GTKmm version (requires 4.0+)
3. Ensure Python 3.8+ is installed

## Contributing

1. Fork the repository
2. Create branch: `git checkout -b feature/amazing-feature`
3. Make changes
4. Submit pull request

## License

This project is licensed under MIT License - see [LICENSE](LICENSE) file for details.
```
Storm Discord RPC/
├── Linux/                 # версия для Linux
│   ├── src/main.cpp      # основной файл на C++
│   ├── DiscordRPC.cs     # версия на C#
│   └── CMakeLists.txt    # файл сборки
├── Windows/              # версия для Windows
│   ├── src/main.cpp      # основной файл на C++
│   ├── DiscordRPC.cs     # версия на C#
│   └── CMakeLists.txt    # файл сборки
├── Android/              # версия для Android
│   └── app/src/main/
│       ├── kotlin/       # код на Kotlin
│       ├── cpp/          # нативный код на C++
│       └── python/       # код на Python
├── shared/               # общие файлы
│   ├── include/          # заголовочные файлы
│   ├── src/              # исходные файлы
│   └── python/           # Python модуль
└── README.md             # этот файл
```

## как получить Client ID

1. перейдите на [Discord Developer Portal](https://discord.com/developers/applications)
2. создайте новое приложение
3. перейдите в раздел "Rich Presence"
4. скопируйте "Application ID" - это и есть ваш Client ID

## инструкции по установке

выберите вашу платформу:

- [Linux Instructions](#linux-instructions)
- [Windows Instructions](#windows-instructions)
- [Android Instructions](#android-instructions)

---

## Linux Instructions

### требования

- cmake 3.10+
- gtkmm-4.0
- g++ (поддержка C++17)
- python3 (опционально)
- mono-complete (для C# версии)

### компиляция C++ версии

```bash
# переход в папку Linux
cd Linux

# создание папки сборки
mkdir build && cd build

# конфигурация
cmake ..

# компиляция
make

# запуск
./storm-discord-rpc
```

### запуск Python версии

```bash
# установка зависимостей
pip3 install ctypes

# запуск
python3 ../shared/python/discord_rpc.py
```

### запуск C# версии

```bash
# компиляция
mcs -target:exe -out:DiscordRPC.exe DiscordRPC.cs

# запуск
mono DiscordRPC.exe
```

### установка зависимостей

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake libgtkmm-4.0-dev python3 mono-complete

# Fedora
sudo dnf install cmake gtkmm4.0-devel python3 mono-devel

# Arch Linux
sudo pacman -S cmake gtkmm4 python mono
```

---

## Windows Instructions

### требования

- Visual Studio 2019+ или MinGW
- cmake 3.10+
- .NET Framework 4.7.2+ (для C# версии)
- Python 3.6+ (опционально)

### компиляция C++ версии (Visual Studio)

```cmd
# переход в папку Windows
cd Windows

# создание папки сборки
mkdir build && cd build

# конфигурация
cmake .. -G "Visual Studio 16 2019"

# компиляция
cmake --build . --config Release

# запуск
Release\storm-discord-rpc.exe
```

### компиляция C++ версии (MinGW)

```cmd
# переход в папку Windows
cd Windows

# создание папки сборки
mkdir build && cd build

# конфигурация
cmake .. -G "MinGW Makefiles"

# компиляция
mingw32-make

# запуск
storm-discord-rpc.exe
```

### запуск Python версии

```cmd
# установка зависимостей
pip install ctypes

# запуск
python ..\shared\python\discord_rpc.py
```

### запуск C# версии

```cmd
# компиляция
csc /target:exe /out:DiscordRPC.exe DiscordRPC.cs

# запуск
DiscordRPC.exe
```

---

## Android Instructions

### требования

- Android Studio
- Android SDK API 21+
- NDK (для нативного кода)
- Chaquopy (для Python поддержки)

### настройка проекта

1. откройте Android Studio
2. импортируйте проект из папки `Android`
3. настройте NDK в SDK Manager
4. установите Chaquopy plugin для поддержки Python

### сборка APK

```bash
# переход в папку Android
cd Android

# сборка debug версии
./gradlew assembleDebug

# сборка release версии
./gradlew assembleRelease
```

### установка APK

```bash
# установка debug версии
adb install app/build/outputs/apk/debug/app-debug.apk

# установка release версии
adb install app/build/outputs/apk/release/app-release.apk
```

### особенности Android версии

- использует нативный C++ код через JNI
- поддерживает Python через Chaquopy
- основной интерфейс на Kotlin
- требует разрешения на интернет

## использование

1. запустите приложение для вашей платформы
2. введите ваш Client ID
3. нажмите "Подключиться"
4. заполните поля для статуса:
   - **Состояние**: основной статус (например: "Играю в игру")
   - **Детали**: дополнительная информация (например: "Уровень 5")
   - **Изображения**: ключи изображений из Discord Developer Portal
5. нажмите "Обновить статус"

## troubleshooting

### Linux
- если не находится gtkmm: `sudo apt install libgtkmm-4.0-dev`
- ошибки компиляции: проверьте версию g++ и cmake

### Windows
- ошибки с Visual C++: установите Microsoft Visual C++ Redistributable
- проблемы с C#: проверьте версию .NET Framework

### Android
- ошибки сборки: проверьте настройки NDK и SDK
- проблемы с Python: убедитесь что Chaquopy правильно настроен

## лицензия

MIT License - можете использовать код в своих проектах.

## вклад

присылайте pull requests с улучшениями и исправлениями.
