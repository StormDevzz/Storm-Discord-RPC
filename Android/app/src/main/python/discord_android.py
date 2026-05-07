#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
модуль Discord RPC для Android
интеграция с Java/Kotlin через Chaquopy
"""

import sys
import time
from typing import Optional

try:
    # импорт Android классов через Chaquopy
    from com.storm.discordrpc import DiscordRPCNative
    from android.util import Log
except ImportError:
    print("chaquopy не доступен, используем заглушку")
    DiscordRPCNative = None
    Log = None

class DiscordRPCAndroid:
    """класс для работы с Discord RPC на Android"""
    
    def __init__(self):
        self.discord_native = None
        self.connected = False
        self.client_id = ""
        
        if DiscordRPCNative:
            try:
                self.discord_native = DiscordRPCNative()
                Log.i("DiscordRPC", "нативная библиотека загружена")
            except Exception as e:
                Log.e("DiscordRPC", f"ошибка загрузки нативной библиотеки: {e}")
                self.discord_native = None
        else:
            print("chaquopy недоступен, работаем в режиме заглушки")
    
    def initialize(self, client_id: str) -> bool:
        """инициализация Discord RPC"""
        self.client_id = client_id
        
        if self.discord_native:
            try:
                result = self.discord_native.initialize(client_id)
                self.connected = result
                if Log:
                    Log.i("DiscordRPC", f"инициализация: {result}")
                return result
            except Exception as e:
                if Log:
                    Log.e("DiscordRPC", f"ошибка инициализации: {e}")
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
            if Log:
                Log.e("DiscordRPC", "не подключен к Discord")
            else:
                print("ошибка: не подключен к Discord")
            return
        
        if self.discord_native:
            try:
                self.discord_native.updatePresence(
                    state, details, large_image_key, large_image_text,
                    small_image_key, small_image_text
                )
                if Log:
                    Log.i("DiscordRPC", "статус обновлен через нативную библиотеку")
            except Exception as e:
                if Log:
                    Log.e("DiscordRPC", f"ошибка обновления статуса: {e}")
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
        
        if self.discord_native:
            try:
                self.discord_native.disconnect()
                if Log:
                    Log.i("DiscordRPC", "отключено через нативную библиотеку")
            except Exception as e:
                if Log:
                    Log.e("DiscordRPC", f"ошибка отключения: {e}")
        else:
            print("отключение от Discord")
        
        self.connected = False
    
    def process_events(self) -> None:
        """обработка событий Discord"""
        if self.connected and self.discord_native:
            try:
                self.discord_native.processEvents()
            except Exception as e:
                if Log:
                    Log.e("DiscordRPC", f"ошибка обработки событий: {e}")
    
    def is_connected(self) -> bool:
        """проверка подключения"""
        return self.connected


# пример использования в Android приложении
def main_example():
    """пример использования модуля"""
    discord = DiscordRPCAndroid()
    
    # здесь должен быть client ID из настроек приложения
    client_id = "1234567890123456789"
    
    if discord.initialize(client_id):
        print("успешное подключение!")
        
        # обновление статуса
        discord.update_presence(
            state="Тестирование Android модуля",
            details="Написание кода на Python",
            large_image_key="logo",
            large_image_text="Storm Discord RPC",
            small_image_key="python",
            small_image_text="Python"
        )
        
        print("статус обновлен")
        
        # в реальном приложении здесь будет цикл обработки событий
        try:
            for i in range(10):
                discord.process_events()
                time.sleep(1)
        except KeyboardInterrupt:
            print("\nзавершение работы...")
            discord.disconnect()
    else:
        print("ошибка подключения")


if __name__ == "__main__":
    main_example()
