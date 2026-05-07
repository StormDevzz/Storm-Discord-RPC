cmake_minimum_required(VERSION 3.10)
project(StormDiscordRPC)

set(CMAKE_CXX_STANDARD 17)

# поиск GTKmm
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-4.0 REQUIRED)

# подключение общих файлов
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../shared/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/discord-rpc/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../external/discord-rpc/src)

# исходные файлы
set(SOURCES
    src/main_working.cpp
    ../shared/src/discord_rpc_python.cpp
    ../shared/src/discord_rpc_real_impl.cpp
    ../shared/src/config_manager.cpp
)

# исполняемый файл
add_executable(storm-discord-rpc ${SOURCES})

# компоновка с GTKmm
target_include_directories(storm-discord-rpc PRIVATE ${GTKMM_INCLUDE_DIRS})
target_link_libraries(storm-discord-rpc ${GTKMM_LIBRARIES})

# флаги компилятора
target_compile_options(storm-discord-rpc PRIVATE ${GTKMM_CFLAGS_OTHER})

# копирование shared файлов в build
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../shared DESTINATION ${CMAKE_BINARY_DIR})
