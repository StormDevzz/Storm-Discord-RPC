#!/usr/bin/env python3
"""
утилиты для discord rpc android приложения
"""

import time
import json
import hashlib
import threading
from typing import Dict, Any, Optional, List
from datetime import datetime

class Logger:
    """простой логгер для android"""
    
    def __init__(self, tag: str = "DiscordRPC"):
        self.tag = tag
    
    def info(self, message: str) -> None:
        """информационное сообщение"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        print(f"[{timestamp}] [INFO] {self.tag}: {message}")
    
    def error(self, message: str) -> None:
        """сообщение об ошибке"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        print(f"[{timestamp}] [ERROR] {self.tag}: {message}")
    
    def debug(self, message: str) -> None:
        """отладочное сообщение"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        print(f"[{timestamp}] [DEBUG] {self.tag}: {message}")

class ConfigManager:
    """менеджер конфигурации"""
    
    def __init__(self):
        self.config = {}
        self.config_file = "/data/data/com.storm.discordrpc/config.json"
        self.logger = Logger("Config")
        self.load_config()
    
    def load_config(self) -> None:
        """загрузка конфигурации"""
        try:
            with open(self.config_file, 'r', encoding='utf-8') as f:
                self.config = json.load(f)
            self.logger.info("конфигурация загружена")
        except FileNotFoundError:
            self.logger.info("файл конфигурации не найден, используем значения по умолчанию")
            self.config = self.get_default_config()
            self.save_config()
        except Exception as e:
            self.logger.error(f"ошибка загрузки конфигурации: {e}")
            self.config = self.get_default_config()
    
    def save_config(self) -> None:
        """сохранение конфигурации"""
        try:
            with open(self.config_file, 'w', encoding='utf-8') as f:
                json.dump(self.config, f, indent=2, ensure_ascii=False)
            self.logger.info("конфигурация сохранена")
        except Exception as e:
            self.logger.error(f"ошибка сохранения конфигурации: {e}")
    
    def get_default_config(self) -> Dict[str, Any]:
        """конфигурация по умолчанию"""
        return {
            "discord_app_id": "1234567890123456789",
            "auto_start": False,
            "default_status": "Storm Discord RPC",
            "update_interval": 5,
            "enable_notifications": True,
            "theme": "dark"
        }
    
    def get(self, key: str, default: Any = None) -> Any:
        """получение значения конфигурации"""
        return self.config.get(key, default)
    
    def set(self, key: str, value: Any) -> None:
        """установка значения конфигурации"""
        self.config[key] = value
        self.save_config()
    
    def get_all(self) -> Dict[str, Any]:
        """получение всей конфигурации"""
        return self.config.copy()

class PresenceValidator:
    """валидатор данных для discord presence"""
    
    def __init__(self):
        self.logger = Logger("Validator")
        self.max_length = 128
    
    def validate_state(self, state: str) -> bool:
        """валидация состояния"""
        if not isinstance(state, str):
            self.logger.error("состояние должно быть строкой")
            return False
        
        if len(state) > self.max_length:
            self.logger.error(f"состояние слишком длинное (макс: {self.max_length})")
            return False
        
        return True
    
    def validate_details(self, details: str) -> bool:
        """валидация деталей"""
        if not isinstance(details, str):
            self.logger.error("детали должны быть строкой")
            return False
        
        if len(details) > self.max_length:
            self.logger.error(f"детали слишком длинные (макс: {self.max_length})")
            return False
        
        return True
    
    def validate_presence_data(self, state: str, details: str) -> bool:
        """валидация всех данных presence"""
        return self.validate_state(state) and self.validate_details(details)
    
    def sanitize_text(self, text: str) -> str:
        """очистка текста от недопустимых символов"""
        if not isinstance(text, str):
            return ""
        
        # удаляем null байты и другие недопустимые символы
        sanitized = text.replace('\x00', '').strip()
        
        # ограничиваем длину
        if len(sanitized) > self.max_length:
            sanitized = sanitized[:self.max_length]
        
        return sanitized

class ThreadSafeCounter:
    """потокобезопасный счетчик"""
    
    def __init__(self, initial_value: int = 0):
        self._value = initial_value
        self._lock = threading.Lock()
    
    def increment(self) -> int:
        """увеличение счетчика"""
        with self._lock:
            self._value += 1
            return self._value
    
    def decrement(self) -> int:
        """уменьшение счетчика"""
        with self._lock:
            self._value -= 1
            return self._value
    
    def get(self) -> int:
        """получение значения"""
        with self._lock:
            return self._value
    
    def set(self, value: int) -> None:
        """установка значения"""
        with self._lock:
            self._value = value

class RateLimiter:
    """ограничитель частоты запросов"""
    
    def __init__(self, max_requests: int, time_window: int):
        self.max_requests = max_requests
        self.time_window = time_window
        self.requests = []
        self.lock = threading.Lock()
        self.logger = Logger("RateLimiter")
    
    def can_make_request(self) -> bool:
        """проверка возможности сделать запрос"""
        current_time = time.time()
        
        with self.lock:
            # удаляем старые запросы
            self.requests = [req_time for req_time in self.requests 
                           if current_time - req_time < self.time_window]
            
            # проверяем лимит
            if len(self.requests) < self.max_requests:
                self.requests.append(current_time)
                return True
            else:
                self.logger.warning("превышен лимит запросов")
                return False
    
    def get_remaining_requests(self) -> int:
        """получение оставшихся запросов"""
        current_time = time.time()
        
        with self.lock:
            self.requests = [req_time for req_time in self.requests 
                           if current_time - req_time < self.time_window]
            return self.max_requests - len(self.requests)

def generate_unique_id() -> str:
    """генерация уникального id"""
    timestamp = str(int(time.time()))
    random_data = str(time.time()).encode('utf-8')
    hash_obj = hashlib.md5(random_data)
    return f"{timestamp}_{hash_obj.hexdigest()[:8]}"

def format_duration(seconds: int) -> str:
    """форматирование длительности в читаемый вид"""
    if seconds < 60:
        return f"{seconds}с"
    elif seconds < 3600:
        minutes = seconds // 60
        remaining_seconds = seconds % 60
        return f"{minutes}м {remaining_seconds}с"
    else:
        hours = seconds // 3600
        remaining_minutes = (seconds % 3600) // 60
        return f"{hours}ч {remaining_minutes}м"

def safe_json_loads(json_str: str, default: Any = None) -> Any:
    """безопасная загрузка json"""
    try:
        return json.loads(json_str)
    except (json.JSONDecodeError, TypeError) as e:
        Logger("Utils").error(f"ошибка парсинга json: {e}")
        return default

def safe_json_dumps(obj: Any, default: str = "{}") -> str:
    """безопасное сохранение в json"""
    try:
        return json.dumps(obj, ensure_ascii=False)
    except (TypeError, ValueError) as e:
        Logger("Utils").error(f"ошибка сериализации в json: {e}")
        return default

# глобальные экземпляры
_logger = Logger("DiscordRPC")
_config_manager = ConfigManager()
_validator = PresenceValidator()

def get_logger() -> Logger:
    """получение логгера"""
    return _logger

def get_config() -> ConfigManager:
    """получение менеджера конфигурации"""
    return _config_manager

def get_validator() -> PresenceValidator:
    """получение валидатора"""
    return _validator

if __name__ == "__main__":
    # тестовый код
    print("тест утилит")
    
    # тест логгера
    logger = get_logger()
    logger.info("тест логгера")
    logger.error("тест ошибки")
    
    # тест конфигурации
    config = get_config()
    print(f"app id: {config.get('discord_app_id')}")
    
    # тест валидатора
    validator = get_validator()
    print(f"валидация 'тест': {validator.validate_state('тест')}")
    print(f"валидация длинной строки: {validator.validate_state('a' * 200)}")
    
    # тест счетчика
    counter = ThreadSafeCounter()
    print(f"счетчик: {counter.increment()}")
    print(f"счетчик: {counter.increment()}")
    print(f"счетчик: {counter.get()}")
    
    # тест ограничителя
    limiter = RateLimiter(5, 10)
    for i in range(7):
        print(f"запрос {i}: {limiter.can_make_request()}")
    
    print(f"уникальный id: {generate_unique_id()}")
    print(f"форматирование 65с: {format_duration(65)}")
    print(f"форматирование 3665с: {format_duration(3665)}")
