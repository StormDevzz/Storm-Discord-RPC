cmake_minimum_required(VERSION 3.4.1)

# название проекта
project(StormDiscordRPC)

# установка C++ стандарта
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# поиск библиотек
find_library(log-lib log)
find_library(android-lib android)

# добавление библиотеки discord rpc
add_library(discord-rpc SHARED
    cpp/discord_rpc_wrapper.cpp
    cpp/discord_rpc_core.cpp
    cpp/native_activity.cpp
)

# подключение заголовочных файлов
target_include_directories(discord-rpc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp
)

# линковка библиотек
target_link_libraries(discord-rpc
    ${log-lib}
    ${android-lib}
    # здесь нужно добавить реальную discord rpc библиотеку
    # ${CMAKE_CURRENT_SOURCE_DIR}/libs/libdiscord-rpc.so
)

# компиляция как C++
set_target_properties(discord-rpc PROPERTIES
    LANGUAGE CXX
)

# установка флагов компиляции
target_compile_options(discord-rpc PRIVATE
    -Wall
    -Wextra
    -O2
    -fPIC
)

# определение для android
target_compile_definitions(discord-rpc PRIVATE
    ANDROID
    DISCORD_RPC_IMPL
)
