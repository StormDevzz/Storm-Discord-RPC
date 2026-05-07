#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
модуль для работы с Discord Rich Presence на Python
поддерживает интеграцию с C++ бэкендом
"""

import ctypes
import os
import sys
import time
from typing import Optional

class DiscordRPC:
    """класс для управления Discord Rich Presence"""
    
    def __init__(self):
        self.lib_path = None
        self.lib = None
        self.connected = False
        self.client_id = ""
        
        # поиск библиотеки
        self._find_library()
    
    def _find_library(self):
        """поиск скомпилированной библиотеки"""
        # пути для поиска библиотеки
        possible_paths = [
            # для Linux
            "../Linux/build/libdiscord_rpc.so",
            "./libdiscord_rpc.so",
            # для Windows
            "../Windows/build/discord_rpc.dll",
            "./discord_rpc.dll",
            # для Android
            "../Android/build/libdiscord_rpc.so",
        ]
        
        for path in possible_paths:
            if os.path.exists(path):
                self.lib_path = path
                break
        
        if self.lib_path:
            try:
                self.lib = ctypes.CDLL(self.lib_path)
                self._setup_functions()
                print(f"библиотека загружена: {self.lib_path}")
            except Exception as e:
                print(f"ошибка загрузки библиотеки: {e}")
                self.lib = None
        else:
            print("библиотека не найдена, используем заглушку")
    
    def _setup_functions(self):
        """настройка функций из C++ библиотеки"""
        if not self.lib:
            return
        
        # объявление функций из C++
        self.lib.discord_initialize.argtypes = [ctypes.c_char_p]
        self.lib.discord_initialize.restype = ctypes.c_bool
        
        self.lib.discord_update_presence.argtypes = [
            ctypes.c_char_p, ctypes.c_char_p,
            ctypes.c_char_p, ctypes.c_char_p,
            ctypes.c_char_p, ctypes.c_char_p
        ]
        self.lib.discord_update_presence.restype = None
        
        self.lib.discord_shutdown.argtypes = []
        self.lib.discord_shutdown.restype = None
        
        self.lib.discord_run_callbacks.argtypes = []
        self.lib.discord_run_callbacks.restype = None
    
    def initialize(self, client_id: str) -> bool:
        """инициализация Discord RPC"""
        self.client_id = client_id
        
        if self.lib:
            try:
                result = self.lib.discord_initialize(client_id.encode('utf-8'))
                self.connected = result
                return result
            except Exception as e:
                print(f"ошибка инициализации: {e}")
                return False
        else:
            # заглушка
            print(f"подключение к Discord с Client ID: {client_id}")
            self.connected = True
            return True
    
    def update_presence(self, state: str = "", details: str = "", 
                        large_image_key: str = "", large_image_text: str = "",
                        small_image_key: str = "", small_image_text: str = "") -> None:
        """обновление статуса Discord"""
        if not self.connected:
            print("ошибка: не подключен к Discord")
            return
        
        if self.lib:
            try:
                self.lib.discord_update_presence(
                    state.encode('utf-8'),
                    details.encode('utf-8'),
                    large_image_key.encode('utf-8'),
                    large_image_text.encode('utf-8'),
                    small_image_key.encode('utf-8'),
                    small_image_text.encode('utf-8')
                )
            except Exception as e:
                print(f"ошибка обновления статуса: {e}")
        else:
            # заглушка
            print("обновление статуса:")
            if state:
                print(f"  состояние: {state}")
            if details:
                print(f"  детали: {details}")
            if large_image_key:
                print(f"  большое изображение: {large_image_key}")
                if large_image_text:
                    print(f"    ({large_image_text})")
            if small_image_key:
                print(f"  маленькое изображение: {small_image_key}")
                if small_image_text:
                    print(f"    ({small_image_text})")
    
    def disconnect(self) -> None:
        """отключение от Discord"""
        if not self.connected:
            return
        
        if self.lib:
            try:
                self.lib.discord_shutdown()
            except Exception as e:
                print(f"ошибка отключения: {e}")
        else:
            print("отключение от Discord")
        
        self.connected = False
    
    def process_events(self) -> None:
        """обработка событий Discord"""
        if self.connected and self.lib:
            try:
                self.lib.discord_run_callbacks()
            except Exception as e:
                print(f"ошибка обработки событий: {e}")
    
    def is_connected(self) -> bool:
        """проверка подключения"""
        return self.connected


# пример использования
if __name__ == "__main__":
    discord = DiscordRPC()
    
    # ввод client id
    client_id = input("введите Client ID: ").strip()
    
    if discord.initialize(client_id):
        print("успешное подключение!")
        
        # обновление статуса
        discord.update_presence(
            state="Тестирование Python модуля",
            details="Написание кода",
            large_image_key="logo",
            large_image_text="Storm Discord RPC",
            small_image_key="python",
            small_image_text="Python"
        )
        
        print("статус обновлен. нажмите Ctrl+C для выхода...")
        
        try:
            while True:
                discord.process_events()
                time.sleep(1)
        except KeyboardInterrupt:
            print("\nзавершение работы...")
            discord.disconnect()
    else:
        print("ошибка подключения")
