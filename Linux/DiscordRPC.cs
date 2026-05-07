using System;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;

namespace StormDiscordRPC
{
    // класс для работы с Discord Rich Presence на C#
    public class DiscordRPC
    {
        private bool connected = false;
        private string clientId = "";
        private CancellationTokenSource cancellationTokenSource;
        
        // импорт функций из C++ библиотеки
        [DllImport("libdiscord_rpc.so", CharSet = CharSet.Unicode)]
        private static extern bool discord_initialize(string client_id);
        
        [DllImport("libdiscord_rpc.so", CharSet = CharSet.Unicode)]
        private static extern void discord_update_presence(
            string state, string details,
            string large_image_key, string large_image_text,
            string small_image_key, string small_image_text);
        
        [DllImport("libdiscord_rpc.so")]
        private static extern void discord_shutdown();
        
        [DllImport("libdiscord_rpc.so")]
        private static extern void discord_run_callbacks();
        
        public DiscordRPC()
        {
            cancellationTokenSource = new CancellationTokenSource();
        }
        
        // инициализация Discord RPC
        public bool Initialize(string clientId)
        {
            this.clientId = clientId;
            
            try
            {
                bool result = discord_initialize(clientId);
                connected = result;
                
                if (connected)
                {
                    // запуск обработки событий в фоновом потоке
                    Task.Run(() => ProcessEventsLoop(cancellationTokenSource.Token));
                }
                
                return result;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"ошибка инициализации: {ex.Message}");
                return false;
            }
        }
        
        // обновление статуса
        public void UpdatePresence(string state = "", string details = "", 
                                 string largeImageKey = "", string largeImageText = "",
                                 string smallImageKey = "", string smallImageText = "")
        {
            if (!connected)
            {
                Console.WriteLine("ошибка: не подключен к Discord");
                return;
            }
            
            try
            {
                discord_update_presence(state, details, 
                                       largeImageKey, largeImageText,
                                       smallImageKey, smallImageText);
                Console.WriteLine("статус успешно обновлен");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"ошибка обновления статуса: {ex.Message}");
            }
        }
        
        // отключение
        public void Disconnect()
        {
            if (!connected)
                return;
            
            try
            {
                discord_shutdown();
                connected = false;
                cancellationTokenSource.Cancel();
                Console.WriteLine("отключение от Discord выполнено");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"ошибка отключения: {ex.Message}");
            }
        }
        
        // проверка подключения
        public bool IsConnected()
        {
            return connected;
        }
        
        // цикл обработки событий
        private async Task ProcessEventsLoop(CancellationToken cancellationToken)
        {
            while (!cancellationToken.IsCancellationRequested && connected)
            {
                try
                {
                    discord_run_callbacks();
                    await Task.Delay(1000, cancellationToken);
                }
                catch (TaskCanceledException)
                {
                    break;
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"ошибка обработки событий: {ex.Message}");
                }
            }
        }
        
        // освобождение ресурсов
        public void Dispose()
        {
            Disconnect();
            cancellationTokenSource?.Dispose();
        }
    }
    
    // пример консольного приложения
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Storm Discord RPC - C# версия");
            Console.WriteLine("================================");
            
            using (var discord = new DiscordRPC())
            {
                Console.Write("введите Client ID: ");
                string clientId = Console.ReadLine()?.Trim();
                
                if (string.IsNullOrEmpty(clientId))
                {
                    Console.WriteLine("ошибка: client id не может быть пустым");
                    return;
                }
                
                if (discord.Initialize(clientId))
                {
                    Console.WriteLine("успешное подключение к Discord!");
                    
                    // обновление статуса
                    discord.UpdatePresence(
                        state: "Тестирование C# модуля",
                        details: "Написание кода на C#",
                        largeImageKey: "logo",
                        largeImageText: "Storm Discord RPC",
                        smallImageKey: "csharp",
                        smallImageText: "C#"
                    );
                    
                    Console.WriteLine("статус обновлен. нажмите любую клавишу для выхода...");
                    Console.ReadKey();
                    
                    discord.Disconnect();
                }
                else
                {
                    Console.WriteLine("ошибка подключения к Discord");
                }
            }
            
            Console.WriteLine("программа завершена");
        }
    }
}
