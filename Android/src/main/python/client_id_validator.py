#!/usr/bin/env python3
"""
валидатор discord client id
проверка корректности введенного client id через python
"""

import re
import json
import requests
from typing import Tuple, Optional, Dict, Any

class ClientIDValidator:
    """валидатор discord client id"""
    
    def __init__(self):
        self.logger = self._get_logger()
        
    def _get_logger(self):
        """простой логгер"""
        import sys
        class SimpleLogger:
            def info(self, msg):
                print(f"[INFO] ClientIDValidator: {msg}")
            def error(self, msg):
                print(f"[ERROR] ClientIDValidator: {msg}")
            def warning(self, msg):
                print(f"[WARNING] ClientIDValidator: {msg}")
        return SimpleLogger()
    
    def validate_format(self, client_id: str) -> Tuple[bool, str]:
        """
        проверка формата client id
        
        Args:
            client_id: введенный client id
            
        Returns:
            Tuple[bool, str]: (валиден, сообщение об ошибке)
        """
        if not client_id:
            return False, "client id не может быть пустым"
        
        # удаляем пробелы и другие символы
        clean_id = re.sub(r'[^\d]', '', client_id)
        
        if len(clean_id) != 18:
            return False, f"client id должен содержать ровно 18 цифр, сейчас: {len(clean_id)}"
        
        if clean_id != client_id:
            return False, "client id должен содержать только цифры"
        
        # проверка на начало с нуля (discord id не начинаются с нуля)
        if clean_id.startswith('0'):
            return False, "client id не может начинаться с нуля"
        
        return True, "формат client id корректен"
    
    def validate_discord_api(self, client_id: str) -> Tuple[bool, str]:
        """
        проверка client id через discord api
        
        Args:
            client_id: client id для проверки
            
        Returns:
            Tuple[bool, str]: (валиден, сообщение)
        """
        try:
            # пробуем получить информацию о приложении через discord api
            url = f"https://discord.com/api/v10/applications/{client_id}/rpc"
            
            headers = {
                'User-Agent': 'StormDiscordRPC/1.0'
            }
            
            response = requests.get(url, headers=headers, timeout=10)
            
            if response.status_code == 200:
                app_info = response.json()
                app_name = app_info.get('name', 'Unknown')
                return True, f"приложение найдено: {app_name}"
            elif response.status_code == 404:
                return False, "приложение с таким client id не найдено"
            else:
                return False, f"ошибка проверки: HTTP {response.status_code}"
                
        except requests.exceptions.Timeout:
            return False, "тайм-аут при проверке client id"
        except requests.exceptions.ConnectionError:
            return False, "ошибка подключения при проверке client id"
        except Exception as e:
            return False, f"неизвестная ошибка: {str(e)}"
    
    def get_application_info(self, client_id: str) -> Optional[Dict[str, Any]]:
        """
        получение подробной информации о приложении
        
        Args:
            client_id: client id приложения
            
        Returns:
            Optional[Dict[str, Any]]: информация о приложении или None
        """
        try:
            url = f"https://discord.com/api/v10/applications/{client_id}"
            
            headers = {
                'User-Agent': 'StormDiscordRPC/1.0'
            }
            
            response = requests.get(url, headers=headers, timeout=10)
            
            if response.status_code == 200:
                return response.json()
            else:
                return None
                
        except Exception as e:
            self.logger.error(f"ошибка получения информации о приложении: {e}")
            return None
    
    def validate_complete(self, client_id: str) -> Dict[str, Any]:
        """
        полная проверка client id
        
        Args:
            client_id: client id для проверки
            
        Returns:
            Dict[str, Any]: результат проверки
        """
        result = {
            'client_id': client_id,
            'valid': False,
            'format_valid': False,
            'api_valid': False,
            'app_info': None,
            'errors': [],
            'messages': []
        }
        
        # проверка формата
        format_valid, format_msg = self.validate_format(client_id)
        result['format_valid'] = format_valid
        
        if not format_valid:
            result['errors'].append(format_msg)
            return result
        else:
            result['messages'].append(format_msg)
        
        # проверка через api
        api_valid, api_msg = self.validate_discord_api(client_id)
        result['api_valid'] = api_valid
        
        if api_valid:
            result['messages'].append(api_msg)
            # получаем информацию о приложении
            app_info = self.get_application_info(client_id)
            result['app_info'] = app_info
        else:
            result['errors'].append(api_msg)
        
        result['valid'] = format_valid and api_valid
        
        return result
    
    def generate_test_ids(self) -> list:
        """
        генерация тестовых client id для демонстрации
        
        Returns:
            list: список тестовых client id
        """
        # реальные client id не публикуются, это просто примеры формата
        test_ids = [
            "123456789012345678",  # правильный формат
            "987654321098765432",  # правильный формат
            "111111111111111111",  # правильный формат
            "12345",               # слишком короткий
            "1234567890123456789", # слишком длинный
            "abcdefghijk",         # не цифры
            "012345678901234567", # начинается с нуля
            "123 456 789 012 345", # с пробелами
        ]
        
        return test_ids

# глобальный экземпляр
_validator = None

def get_validator() -> ClientIDValidator:
    """получение глобального экземпляра валидатора"""
    global _validator
    if _validator is None:
        _validator = ClientIDValidator()
    return _validator

# функции для вызова из java
def java_validate_format(client_id: str) -> str:
    """проверка формата client id из java"""
    validator = get_validator()
    valid, message = validator.validate_format(client_id)
    return json.dumps({"valid": valid, "message": message})

def java_validate_complete(client_id: str) -> str:
    """полная проверка client id из java"""
    validator = get_validator()
    result = validator.validate_complete(client_id)
    return json.dumps(result)

def java_get_app_info(client_id: str) -> str:
    """получение информации о приложении из java"""
    validator = get_validator()
    app_info = validator.get_application_info(client_id)
    return json.dumps({"success": app_info is not None, "app_info": app_info})

def java_test_validation() -> str:
    """тестирование валидации из java"""
    validator = get_validator()
    test_ids = validator.generate_test_ids()
    results = []
    
    for test_id in test_ids:
        result = validator.validate_complete(test_id)
        results.append(result)
    
    return json.dumps({"test_results": results})

if __name__ == "__main__":
    # тестовый код
    print("тест валидатора discord client id")
    
    validator = get_validator()
    
    # тестовые client id
    test_ids = [
        "123456789012345678",  # правильный формат (но может не существовать)
        "987654321098765432",  # правильный формат
        "12345",               # слишком короткий
        "abcdefghijk",         # не цифры
    ]
    
    for test_id in test_ids:
        print(f"\nпроверка: {test_id}")
        result = validator.validate_complete(test_id)
        
        print(f"валиден: {result['valid']}")
        print(f"формат: {result['format_valid']}")
        print(f"api: {result['api_valid']}")
        
        if result['errors']:
            print(f"ошибки: {result['errors']}")
        if result['messages']:
            print(f"сообщения: {result['messages']}")
        
        if result['app_info']:
            print(f"инфо о приложении: {result['app_info'].get('name', 'Unknown')}")
