#!/usr/bin/env python3
"""
python мост для android приложения
интеграция python кода с java через chaquopy
"""

import json
import threading
from typing import Dict, Any, Callable
from discord_rpc_manager import get_discord_manager

class PythonBridge:
    """мост между python и java для android"""
    
    def __init__(self):
        self.callbacks = {}
        self.event_queue = []
        self.queue_lock = threading.Lock()
        self.running = False
        
        print("python мост инициализирован")
    
    def register_callback(self, event_name: str, callback: Callable) -> None:
        """регистрация callback для событий из java"""
        self.callbacks[event_name] = callback
        print(f"callback зарегистрирован: {event_name}")
    
    def send_event_to_java(self, event_name: str, data: Dict[str, Any]) -> None:
        """отправка события в java"""
        try:
            # импортируем chaquopy api
            import com.chaquo.python.PyObject as PyObject
            
            # конвертируем данные в json
            json_data = json.dumps(data)
            
            # отправляем в java层
            java_bridge = PyObject.fromJava("com.storm.discordrpc.PythonBridge")
            java_bridge.callStatic("receiveEvent", event_name, json_data)
            
            print(f"событие отправлено в java: {event_name}")
            
        except ImportError:
            # если chaquopy недоступен, просто логируем
            print(f"chaquopy недоступен, событие: {event_name} -> {data}")
        except Exception as e:
            print(f"ошибка отправки события в java: {e}")
    
    def receive_event_from_java(self, event_name: str, json_data: str) -> str:
        """получение события из java"""
        try:
            # парсим json данные
            data = json.loads(json_data)
            
            # добавляем в очередь событий
            with self.queue_lock:
                self.event_queue.append((event_name, data))
            
            # вызываем callback если есть
            if event_name in self.callbacks:
                result = self.callbacks[event_name](data)
                return json.dumps({"success": True, "result": result})
            else:
                print(f"нет callback для события: {event_name}")
                return json.dumps({"success": False, "error": "no callback"})
                
        except json.JSONDecodeError as e:
            error_msg = f"ошибка парсинга json: {e}"
            print(error_msg)
            return json.dumps({"success": False, "error": error_msg})
        except Exception as e:
            error_msg = f"ошибка обработки события: {e}"
            print(error_msg)
            return json.dumps({"success": False, "error": error_msg})
    
    def process_events(self) -> None:
        """обработка событий из очереди"""
        while self.running:
            try:
                with self.queue_lock:
                    if self.event_queue:
                        event_name, data = self.event_queue.pop(0)
                        
                        # обрабатываем событие
                        self._handle_event(event_name, data)
                
                # небольшая задержка
                import time
                time.sleep(0.01)
                
            except Exception as e:
                print(f"ошибка обработки событий: {e}")
    
    def _handle_event(self, event_name: str, data: Dict[str, Any]) -> None:
        """внутренняя обработка события"""
        try:
            if event_name == "discord_initialize":
                # инициализация discord rpc
                app_id = data.get("app_id", "")
                manager = get_discord_manager()
                success = manager.initialize(app_id)
                
                # отправляем результат обратно в java
                self.send_event_to_java("discord_initialized", {
                    "success": success,
                    "app_id": app_id
                })
                
            elif event_name == "discord_update_presence":
                # обновление presence
                state = data.get("state", "")
                details = data.get("details", "")
                
                manager = get_discord_manager()
                success = manager.update_presence(state, details, **data)
                
                # отправляем результат обратно в java
                self.send_event_to_java("discord_presence_updated", {
                    "success": success,
                    "state": state,
                    "details": details
                })
                
            elif event_name == "discord_clear_presence":
                # очистка presence
                manager = get_discord_manager()
                success = manager.clear_presence()
                
                # отправляем результат обратно в java
                self.send_event_to_java("discord_presence_cleared", {
                    "success": success
                })
                
            elif event_name == "discord_shutdown":
                # выключение discord rpc
                manager = get_discord_manager()
                manager.shutdown()
                
                # отправляем результат обратно в java
                self.send_event_to_java("discord_shutdown", {
                    "success": True
                })
                
            else:
                print(f"неизвестное событие: {event_name}")
                
        except Exception as e:
            print(f"ошибка обработки события {event_name}: {e}")
    
    def start(self) -> None:
        """запуск моста"""
        if not self.running:
            self.running = True
            
            # запускаем поток обработки событий
            event_thread = threading.Thread(target=self.process_events, daemon=True)
            event_thread.start()
            
            print("python мост запущен")
    
    def stop(self) -> None:
        """остановка моста"""
        self.running = False
        print("python мост остановлен")

# глобальный экземпляр моста
_bridge = None

def get_bridge() -> PythonBridge:
    """получение глобального экземпляра моста"""
    global _bridge
    if _bridge is None:
        _bridge = PythonBridge()
        _bridge.start()
    return _bridge

# функции для вызова из java
def java_receive_event(event_name: str, json_data: str) -> str:
    """получение события из java"""
    bridge = get_bridge()
    return bridge.receive_event_from_java(event_name, json_data)

def java_send_event(event_name: str, json_data: str) -> str:
    """отправка события в java (для тестов)"""
    try:
        data = json.loads(json_data)
        bridge = get_bridge()
        bridge.send_event_to_java(event_name, data)
        return json.dumps({"success": True})
    except Exception as e:
        return json.dumps({"success": False, "error": str(e)})

def initialize_bridge() -> str:
    """инициализация моста"""
    bridge = get_bridge()
    return json.dumps({"success": True, "status": "initialized"})

if __name__ == "__main__":
    # тестовый код
    print("тест python моста")
    
    bridge = get_bridge()
    
    # тестовое событие
    test_data = {
        "app_id": "1234567890123456789",
        "test": True
    }
    
    bridge.receive_event_from_java("test_event", json.dumps(test_data))
    
    # работаем немного
    import time
    time.sleep(1)
    
    bridge.stop()
