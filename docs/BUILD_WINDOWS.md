# Сборка для Windows

## Требования

### Системные требования
- Windows 10 1903+ (64-bit)
- Visual Studio 2019+ или Visual Studio 2022
- .NET 6.0 SDK
- Python 3.8+

### Установка Visual Studio

1. Скачайте [Visual Studio Community](https://visualstudio.microsoft.com/downloads/)
2. При установке выберите компоненты:
   - Разработка классических приложений C++
   - Разработка кроссплатформенных приложений .NET
   - Пакет SDK для Windows 10/11

### Установка Python

1. Скачайте Python с [python.org](https://www.python.org/downloads/)
2. При установке отметьте "Add Python to PATH"
3. Проверьте установку:
```cmd
python --version
pip --version
```

### Установка зависимостей Python
```cmd
pip install pypresence
```

## Сборка через Visual Studio

1. **Клонирование репозитория**
```cmd
git clone https://github.com/StormDevzz/Storm-Discord-RPC.git
cd Storm-Discord-RPC
```

2. **Открытие проекта**
   - Откройте `Windows/StormDiscordRPC.sln` в Visual Studio
   - Выберите конфигурацию Release/x64

3. **Сборка**
   - Нажмите F7 или выберите "Сборка" → "Собрать решение"

4. **Запуск**
   - Исполняемый файл будет в `x64/Release/StormDiscordRPC.exe`

## Сборка через .NET CLI

1. **Клонирование и сборка**
```cmd
git clone https://github.com/StormDevzz/Storm-Discord-RPC.git
cd Storm-Discord-RPC/Windows
dotnet build --configuration Release --verbosity minimal
```

2. **Запуск**
```cmd
cd bin/Release/net6.0-windows
StormDiscordRPC.exe
```

## Создание установщика

### С помощью WiX Toolset

1. **Установка WiX**
```cmd
# Установка через Chocolatey
choco install wixtoolset

# Или скачайте с https://wixtoolset.org/
```

2. **Сборка MSI**
```cmd
cd Windows
dotnet build --configuration Release
candle.exe -out obj/Release/ Installer.wxs
light.exe -out bin/Release/StormDiscordRPC.msi obj/Release/Installer.wixobj
```

### С помощью Inno Setup

1. Создайте файл `setup.iss` в директории Windows
2. Настройте параметры установки
3. Скомпилируйте через Inno Setup Compiler

## Траблшутинг

### Ошибки при сборке

**Ошибка: .NET 6.0 не найден**
```cmd
# Установка .NET 6.0 SDK
dotnet --list-sdks
dotnet --install-sdk 6.0.x
```

**Ошибка: Visual C++ компоненты**
- Установите "Разработка классических приложений C++" через Visual Studio Installer
- Или установите [Build Tools for Visual Studio](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio)

**Ошибка: pypresence не найден**
```cmd
pip install pypresence
pip show pypresence
```

### Ошибки при запуске

**Ошибка: Библиотеки не найдены**
- Убедитесь что Visual C++ Redistributable установлен
- Проверьте наличие `vcruntime140.dll`

**Ошибка: Discord не отвечает**
1. Убедитесь что Discord запущен
2. Перезапустите Discord
3. Проверьте антивирус на блокировку приложения

### Ошибки с путями

**Ошибка: Python не найден**
```cmd
# Добавьте Python в PATH или используйте полный путь
where python
```

**Ошибка: Модули Python не найдены**
```cmd
# Проверьте установку pypresence
python -c "import pypresence; print('pypresence installed')"
```

## Отладка

### Отладка в Visual Studio
1. Установите точки останова
2. Нажмите F5 для запуска с отладкой
3. Используйте окно "Вывод" для просмотра логов

### Отладка через командную строку
```cmd
# Сборка с отладочной информацией
dotnet build --configuration Debug

# Запуск с отладкой
cd bin/Debug/net6.0-windows
StormDiscordRPC.exe
```

## Пакетирование

### Создание портативной версии
```cmd
# Копирование зависимостей
dotnet publish --configuration Release --self-contained false --output portable
```

### Создание самодостаточной версии
```cmd
dotnet publish --configuration Release --self-contained true --runtime win-x64 --output standalone
```

## Разработка

### Структура проекта
```
Windows/
├── StormDiscordRPC.sln      # решение Visual Studio
├── StormDiscordRPC.csproj    # проект C#
├── DiscordRPC.cs             # основной класс
├── Form1.Designer.cs         # дизайн формы
├── Form1.cs                  # логика формы
├── Program.cs                # точка входа
└── bin/Release/              # собранный исполняемый файл
```

### Добавление зависимостей
```xml
<!-- В StormDiscordRPC.csproj -->
<ItemGroup>
  <PackageReference Include="pypresence" Version="4.2.0" />
</ItemGroup>
```

### Настройка конфигурации
```xml
<PropertyGroup>
  <OutputType>WinExe</OutputType>
  <TargetFramework>net6.0-windows</TargetFramework>
  <UseWindowsForms>true</UseWindowsForms>
  <PublishSingleFile>true</PublishSingleFile>
</PropertyGroup>
```

## Тестирование

### Автоматические тесты
```cmd
dotnet test
```

### Ручное тестирование
1. Проверьте подключение к Discord
2. Проверьте обновление статуса
3. Проверьте сохранение конфигурации
4. Проверьте очистку статуса

## Поддерживаемые версии Windows

- Windows 10 1903+
- Windows 11
- Windows Server 2019+
- Windows Server 2022+

Для Windows 7/8 используйте .NET Framework 4.7.2 вместо .NET 6.0.
