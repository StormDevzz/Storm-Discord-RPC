#!/usr/bin/env python3
"""
discord rpc менеджер для android
используется с chaquopy для интеграции python в android
"""

import threading
import time
import json
from typing import Optional, Dict, Any

class DiscordRPCManager:
    """менеджер для работы с discord rpc на python"""
    
    def __init__(self):
        self.initialized = False
        self.application_id = ""
        self.running = False
        self.callback_thread = None
        self.current_presence = {}
        
        print("discord rpc менеджер создан")
    
    def initialize(self, app_id: str) -> bool:
        """инициализация discord rpc"""
        try:
            self.application_id = app_id
            
            # здесь будет реальная инициализация через discord rpc библиотеку
            # пока что симулируем
            self.initialized = True
            
            # запускаем поток для обработки callback'ов
            self.running = True
            self.callback_thread = threading.Thread(target=self._run_callbacks, daemon=True)
            self.callback_thread.start()
            
            print(f"discord rpc инициализирован с app id: {app_id}")
            return True
            
        except Exception as e:
            print(f"ошибка инициализации discord rpc: {e}")
            self.initialized = False
            return False
    
    def update_presence(self, state: str, details: str, 
                       large_image_key: str = "", large_image_text: str = "",
                       small_image_key: str = "", small_image_text: str = "") -> bool:
        """обновление presence в discord"""
        if not self.initialized:
            print("discord rpc не инициализирован!")
            return False
        
        try:
            # сохраняем текущий presence
            self.current_presence = {
                "state": state,
                "details": details,
                "large_image_key": large_image_key,
                "large_image_text": large_image_text,
                "small_image_key": small_image_key,
                "small_image_text": small_image_text,
                "start_timestamp": int(time.time())
            }
            
            # здесь будет реальное обновление через discord rpc библиотеку
            # пока что симулируем
            print(f"presence обновлен: {state} - {details}")
            
            # отправляем данные в java层 через chaquopy
            self._send_to_java("update_presence", self.current_presence)
            
            return True
            
        except Exception as e:
            print(f"ошибка обновления presence: {e}")
            return False
    
    def clear_presence(self) -> bool:
        """очистка presence"""
        if not self.initialized:
            return False
        
        try:
            self.current_presence = {}
            # здесь будет реальная очистка
            print("discord presence очищен")
            
            # отправляем команду в java层
            self._send_to_java("clear_presence", {})
            
            return True
            
        except Exception as e:
            print(f"ошибка очистки presence: {e}")
            return False
    
    def shutdown(self) -> None:
        """выключение discord rpc"""
        if self.initialized:
            self.running = False
            
            if self.callback_thread and self.callback_thread.is_alive():
                self.callback_thread.join(timeout=1.0)
            
            # здесь будет реальное выключение
            print("discord rpc выключен")
            
            # отправляем команду в java层
            self._send_to_java("shutdown", {})
            
            self.initialized = False
    
    def is_initialized(self) -> bool:
        """проверка инициализации"""
        return self.initialized
    
    def get_application_id(self) -> str:
        """получение application id"""
        return self.application_id
    
    def get_current_presence(self) -> Dict[str, Any]:
        """получение текущего presence"""
        return self.current_presence.copy()
    
    def _run_callbacks(self) -> None:
        """поток для обработки callback'ов"""
        while self.running:
            try:
                # здесь будет обработка callback'ов от discord rpc
                # пока что симулируем
                time.sleep(0.1)
                
            except Exception as e:
                print(f"ошибка в callback потоке: {e}")
                break
    
    def _send_to_java(self, action: str, data: Dict[str, Any]) -> None:
        """отправка данных в java层 через chaquopy"""
        try:
            # импортируем chaquopy api для отправки данных в java
            import com.chaquo.python.PyObject as PyObject
            
            # создаем pyobject с данными
            py_data = PyObject.fromJava(data)
            
            # здесь будет вызов java методов
            # пока что просто логируем
            print(f"отправка в java: {action} -> {data}")
            
        except ImportError:
            # если chaquopy недоступен, просто логируем
            print(f"chaquopy недоступен, отправка в java: {action} -> {data}")
        except Exception as e:
            print(f"ошибка отправки в java: {e}")

# глобальный экземпляр
_discord_manager = None

def get_discord_manager() -> DiscordRPCManager:
    """получение глобального экземпляра discord manager"""
    global _discord_manager
    if _discord_manager is None:
        _discord_manager = DiscordRPCManager()
    return _discord_manager

def initialize_discord_rpc(app_id: str) -> bool:
    """инициализация discord rpc"""
    manager = get_discord_manager()
    return manager.initialize(app_id)

def update_presence(state: str, details: str, **kwargs) -> bool:
    """обновление presence"""
    manager = get_discord_manager()
    return manager.update_presence(state, details, **kwargs)

def clear_presence() -> bool:
    """очистка presence"""
    manager = get_discord_manager()
    return manager.clear_presence()

def shutdown_discord_rpc() -> None:
    """выключение discord rpc"""
    manager = get_discord_manager()
    manager.shutdown()

def is_discord_initialized() -> bool:
    """проверка инициализации"""
    manager = get_discord_manager()
    return manager.is_initialized()

# функции для вызова из java
def java_initialize(app_id: str) -> str:
    """инициализация из java"""
    success = initialize_discord_rpc(app_id)
    return json.dumps({"success": success, "app_id": app_id})

def java_update_presence(state: str, details: str, **kwargs) -> str:
    """обновление presence из java"""
    success = update_presence(state, details, **kwargs)
    return json.dumps({"success": success, "state": state, "details": details})

def java_clear_presence() -> str:
    """очистка presence из java"""
    success = clear_presence()
    return json.dumps({"success": success})

def java_shutdown() -> str:
    """выключение из java"""
    shutdown_discord_rpc()
    return json.dumps({"success": True})

if __name__ == "__main__":
    # тестовый код
    print("тест discord rpc manager")
    
    manager = get_discord_manager()
    
    if manager.initialize("1234567890123456789"):
        print("успешная инициализация")
        
        manager.update_presence("тестовый статус", "тестовые детали")
        time.sleep(2)
        
        manager.clear_presence()
        manager.shutdown()
    else:
        print("ошибка инициализации")
