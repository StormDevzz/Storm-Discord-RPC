#!/usr/bin/env python3
"""
Продвинутый Discord RPC модуль с поддержкой получения информации о пользователе
"""

import json
import socket
import struct
import os
import time
import uuid
import subprocess
import threading
from typing import Optional, Dict, Any

class DiscordRPCAdvanced:
    def __init__(self, client_id: str):
        self.client_id = client_id
        self.socket = None
        self.connected = False
        self.user_info = None
        
    def find_discord_socket(self) -> Optional[str]:
        """Поиск Discord IPC сокета"""
        # Проверяем стандартные пути в /run/user
        uid = os.getuid()
        run_path = f"/run/user/{uid}"
        
        for i in range(10):
            ipc_path = f"{run_path}/discord-ipc-{i}"
            if os.path.exists(ipc_path):
                return ipc_path
        
        # Проверяем старые пути
        home = os.path.expanduser("~")
        paths = [
            f"{home}/.config/discord/",
            f"{home}/.config/discordcanary/",
            f"{home}/.config/discordptb/",
            "/tmp/",
        ]
        
        for base_path in paths:
            try:
                for filename in os.listdir(base_path):
                    if filename.startswith("discord-ipc-"):
                        return os.path.join(base_path, filename)
            except (OSError, PermissionError):
                continue
        
        return None
    
    def connect(self) -> bool:
        """Подключение к Discord IPC"""
        ipc_path = self.find_discord_socket()
        if not ipc_path:
            print("ERROR: Discord IPC сокет не найден")
            return False
        
        try:
            self.socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            self.socket.settimeout(5.0)  # 5 секунд таймаут
            self.socket.connect(ipc_path)
            self.connected = True
            print(f"Подключено к Discord IPC: {ipc_path}")
            
            # Отправляем рукопожатие
            if self.send_handshake():
                return True
            else:
                self.disconnect()
                return False
                
        except Exception as e:
            print(f"ERROR: Ошибка подключения: {e}")
            return False
    
    def send_handshake(self) -> bool:
        """Отправка рукопожатия"""
        handshake = {
            "cmd": "HANDSHAKE",
            "args": {
                "v": 1,
                "client_id": self.client_id
            },
            "nonce": str(uuid.uuid4())
        }
        
        if self.send_command(handshake):
            response = self.read_response()
            if response:
                print(f"Ответ на рукопожатие: {response}")
                
                # Извлекаем информацию о пользователе из READY ответа
                if response.get("evt") == "READY":
                    self.user_info = response.get("data", {}).get("user", {})
                    print(f"Пользователь: {self.user_info.get('username')}#{self.user_info.get('discriminator')}")
                    return True
                elif response.get("evt") == "ERROR":
                    print(f"ERROR: Discord вернул ошибку: {response}")
                    return False
                else:
                    print("Рукопожатие прошло успешно (без READY)")
                    return True
            else:
                print("ERROR: Нет ответа на рукопожатие")
                return False
        else:
            print("ERROR: Не удалось отправить рукопожатие")
            return False
    
    def send_command(self, command: Dict[str, Any]) -> bool:
        """Отправка команды в Discord"""
        if not self.connected or not self.socket:
            return False
        
        try:
            data = json.dumps(command).encode('utf-8')
            length = struct.pack('<I', len(data))
            
            self.socket.send(length + data)
            return True
        except Exception as e:
            print(f"ERROR: Ошибка отправки команды: {e}")
            return False
    
    def read_response(self) -> Optional[Dict[str, Any]]:
        """Чтение ответа от Discord"""
        if not self.connected or not self.socket:
            return None
        
        try:
            # Читаем длину
            length_data = self.socket.recv(4)
            if not length_data:
                return None
            
            length = struct.unpack('<I', length_data)[0]
            
            # Читаем данные
            data = b""
            while len(data) < length:
                chunk = self.socket.recv(length - len(data))
                if not chunk:
                    break
                data += chunk
            
            if len(data) == length:
                return json.loads(data.decode('utf-8'))
            else:
                print(f"ERROR: Получено {len(data)} из {length} байт")
                return None
                
        except Exception as e:
            print(f"ERROR: Ошибка чтения ответа: {e}")
            return None
    
    def set_activity(self, state: str = "", details: str = "", 
                    large_image: str = "", large_text: str = "",
                    small_image: str = "", small_text: str = "") -> bool:
        """Установка статуса активности"""
        if not self.connected:
            print("ERROR: Не подключен к Discord")
            return False
        
        activity = {}
        
        if state:
            activity["state"] = state
        if details:
            activity["details"] = details
        
        # Всегда добавляем timestamps
        activity["timestamps"] = {"start": int(time.time())}
        
        if large_image:
            assets = {"large_image": large_image}
            if large_text:
                assets["large_text"] = large_text
            if small_image:
                assets["small_image"] = small_image
            if small_text:
                assets["small_text"] = small_text
            activity["assets"] = assets
        
        command = {
            "cmd": "SET_ACTIVITY",
            "args": {
                "pid": os.getpid(),
                "activity": activity
            },
            "nonce": str(uuid.uuid4())
        }
        
        print(f"Отправка статуса для пользователя: {self.get_user_display_name()}")
        print(f"JSON: {json.dumps(command, indent=2)}")
        
        if self.send_command(command):
            response = self.read_response()
            if response:
                print(f"Ответ на SET_ACTIVITY: {response}")
                return response.get("evt") != "ERROR"
            else:
                print("Нет ответа на SET_ACTIVITY")
                return False
        else:
            print("Не удалось отправить SET_ACTIVITY")
            return False
    
    def get_user_display_name(self) -> str:
        """Получение отображаемого имени пользователя"""
        if self.user_info:
            username = self.user_info.get("username", "Unknown")
            discriminator = self.user_info.get("discriminator", "0000")
            return f"{username}#{discriminator}"
        else:
            return "Unknown User"
    
    def get_user_id(self) -> str:
        """Получение ID пользователя"""
        if self.user_info:
            return self.user_info.get("id", "")
        else:
            return ""
    
    def disconnect(self):
        """Отключение от Discord"""
        if self.socket:
            try:
                # Отправляем команду закрытия
                self.send_command({
                    "cmd": "CLOSE_ACTIVITY",
                    "args": {},
                    "nonce": str(uuid.uuid4())
                })
                self.socket.close()
            except:
                pass
            finally:
                self.socket = None
                self.connected = False
                print("Отключено от Discord")

def test_discord_rpc():
    """Тестирование Discord RPC"""
    # Замените на ваш Client ID
    client_id = "1347485958282674227"
    
    rpc = DiscordRPCAdvanced(client_id)
    
    if rpc.connect():
        print(f"Успешное подключение! Пользователь: {rpc.get_user_display_name()}")
        
        # Тестовый статус
        rpc.set_activity(
            state="Тестирование Python Discord RPC",
            details="Написание кода для Rich Presence",
            large_image="storm_logo",
            large_text="Storm Discord RPC"
        )
        
        # Ждем немного
        time.sleep(5)
        
        rpc.disconnect()
    else:
        print("Не удалось подключиться к Discord")

if __name__ == "__main__":
    test_discord_rpc()
