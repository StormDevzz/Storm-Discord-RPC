cmake_minimum_required(VERSION 3.10)
project(StormDiscordRPC)

set(CMAKE_CXX_STANDARD 17)

# подключение общих файлов
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../shared/include)

# исходные файлы
set(SOURCES
    src/main.cpp
    ../shared/src/discord_rpc.cpp
)

# исполняемый файл для Windows
add_executable(storm-discord-rpc.exe ${SOURCES})

# линковка с Windows библиотеками
target_link_libraries(storm-discord-rpc 
    comctl32
    user32
    gdi32
    kernel32
)

# установка подсистемы Windows
set_target_properties(storm-discord-rpc PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# копирование shared файлов в build
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../shared DESTINATION ${CMAKE_BINARY_DIR})
