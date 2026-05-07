# Сборка для Linux

## Требования

### Системные требования
- Ubuntu 20.04+ / Debian 11+ / Fedora 34+
- GCC 9.0+
- CMake 3.16+
- Python 3.8+

### Библиотеки
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install libgtkmm-4.0-dev cmake build-essential python3-pip

# Fedora
sudo dnf install gtkmm4-devel cmake gcc-c++ python3-pip

# Arch Linux
sudo pacman -S gtkmm4 cmake gcc python-pip
```

### Python зависимости
```bash
pip3 install pypresence --break-system-packages
```

## Сборка из исходников

1. **Клонирование репозитория**
```bash
git clone https://github.com/StormDevzz/Storm-Discord-RPC.git
cd Storm-Discord-RPC
```

2. **Сборка**
```bash
cd Linux
mkdir build
cd build
cmake ..
make -j$(nproc)
```

3. **Запуск**
```bash
./storm-discord-rpc
```

## Установка в систему

Для установки в систему используйте:
```bash
sudo make install
```

Для удаления:
```bash
sudo make uninstall
```

## Траблшутинг

### Ошибки при сборке

**Ошибка: GTKmm не найден**
```bash
# Ubuntu/Debian
sudo apt install libgtkmm-4.0-dev

# Проверка версии
pkg-config --modversion gtkmm-4.0
```

**Ошибка: CMake устарел**
```bash
# Установка последней версии CMake
sudo apt install cmake
cmake --version  # должна быть 3.16+
```

### Ошибки при запуске

**Ошибка: pypresence не найден**
```bash
pip3 install pypresence --break-system-packages
pip3 show pypresence  # проверка установки
```

**Ошибка: Библиотеки не найдены**
```bash
# Добавление путей к библиотекам
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

**Ошибка: Discord не отвечает**
1. Убедитесь что Discord запущен
2. Проверьте права доступа к IPC сокетам:
```bash
ls -la /run/user/$UID/discord-ipc-*
```

## Отладка

Для включения детальной отладки:
```bash
export STORM_DISCORD_DEBUG=1
./storm-discord-rpc
```

## Пакетирование

### Создание deb пакета
```bash
cd Linux
dpkg-buildpackage -us -uc
```

### Создание AppImage
```bash
cd Linux
mkdir AppImage
cp build/storm-discord-rpc AppImage/
# Создайте AppImage с помощью linuxdeploy
```

## Разработка

### Структура исходников
```
Linux/
├── src/
│   ├── main_working.cpp    # главный файл
│   └── CMakeLists.txt      # конфигурация сборки
├── build/                   # директория сборки
└── storm-discord-rpc       # исполняемый файл
```

### Зависимости проекта
- GTKmm 4.0 (GUI)
- CMake (система сборки)
- pypresence (Discord RPC)
- pthread (многопоточность)

### Компиляция с отладкой
```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
gdb ./storm-discord-rpc
```

## Поддерживаемые дистрибутивы

- Ubuntu 20.04 LTS+
- Debian 11+
- Fedora 34+
- Arch Linux
- openSUSE Leap 15.3+

Для других дистрибутивов могут потребоваться дополнительные настройки путей к библиотекам.
