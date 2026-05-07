#!/usr/bin/env python3
"""
Простой Discord RPC модуль с использованием pypresence
"""

import time
import sys
import os
from pypresence import Presence

def test_discord_rpc(client_id: str):
    """Тестирование Discord RPC с pypresence"""
    try:
        # Инициализация RPC
        RPC = Presence(client_id)
        RPC.connect()
        print("Подключено к Discord!")
        
        # Получаем информацию о пользователе (если возможно)
        # pypresence не предоставляет прямой доступ к информации о пользователе
        # но мы можем показать успешное подключение
        
        # Устанавливаем статус
        RPC.update(
            state="Использует Storm Discord RPC",
            details="Python версия с pypresence",
            large_image="storm_logo",
            large_text="Storm Discord RPC",
            start=int(time.time())
        )
        print("Статус обновлен!")
        print("Проверьте Discord - должен появиться статус 'Использует Storm Discord RPC'")
        
        # Ждем 10 секунд
        time.sleep(10)
        
        # Очищаем статус
        RPC.clear()
        print("Статус очищен")
        
        # Закрываем соединение
        RPC.close()
        print("Отключено от Discord")
        
        return True
        
    except Exception as e:
        print(f"ERROR: {e}")
        return False

def set_discord_status(client_id: str, state: str = "", details: str = "", 
                      large_image: str = "", large_text: str = "",
                      small_image: str = "", small_text: str = ""):
    """Установка статуса Discord"""
    try:
        RPC = Presence(client_id)
        RPC.connect()
        
        activity = {}
        if state:
            activity["state"] = state
        if details:
            activity["details"] = details
        if large_image:
            activity["large_image"] = large_image
        if large_text:
            activity["large_text"] = large_text
        if small_image:
            activity["small_image"] = small_image
        if small_text:
            activity["small_text"] = small_text
        
        activity["start"] = int(time.time())
        
        RPC.update(**activity)
        print(f"Статус обновлен: {state or details or 'Базовый статус'}")
        
        return RPC
        
    except Exception as e:
        print(f"ERROR: {e}")
        return None

def clear_discord_status(client_id: str):
    """Очистка статуса Discord"""
    try:
        RPC = Presence(client_id)
        RPC.connect()
        
        # Очищаем статус - отправляем пустой activity
        RPC.clear()
        print("Статус Discord очищен")
        
        # Закрываем соединение
        time.sleep(0.5)
        RPC.close()
        
        return True
        
    except Exception as e:
        print(f"ERROR при очистке статуса: {e}")
        return False

if __name__ == "__main__":
    if len(sys.argv) > 1:
        client_id = sys.argv[1]
    else:
        # Тестовый Client ID
        client_id = "1347485958282674227"
    
    print(f"Тестирование Discord RPC с Client ID: {client_id}")
    
    if test_discord_rpc(client_id):
        print("Тест успешно пройден!")
    else:
        print("Тест не удался!")
        
        # Попробуем альтернативный подход
        print("\nПробуем альтернативный подход...")
        rpc = set_discord_status(client_id, "Тестовый статус", "Python pypresence")
        if rpc:
            time.sleep(5)
            rpc.close()
