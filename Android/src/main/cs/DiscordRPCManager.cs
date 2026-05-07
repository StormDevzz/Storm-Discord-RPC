using System;
using System.Runtime.InteropServices;
using Android.Log;

// менеджер для работы с discord rpc на c#
namespace StormDiscordRPC
{
    public class DiscordRPCManager
    {
        private bool initialized = false;
        private string applicationId;

        // импорт нативных функций discord rpc
        [DllImport("discord-rpc", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Discord_Initialize(string applicationId);

        [DllImport("discord-rpc", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Discord_UpdatePresence(string state, string details);

        [DllImport("discord-rpc", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Discord_Shutdown();

        [DllImport("discord-rpc", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Discord_RunCallbacks();

        public DiscordRPCManager()
        {
            // конструктор менеджера
            Log.Info("DiscordRPC", "создан discord rpc менеджер");
        }

        // инициализация discord rpc
        public void Initialize(string appId)
        {
            this.applicationId = appId;
            
            try
            {
                // инициализируем discord rpc
                Discord_Initialize(appId);
                initialized = true;
                
                Log.Info("DiscordRPC", $"discord rpc инициализирован с app id: {appId}");
            }
            catch (Exception ex)
            {
                Log.Error("DiscordRPC", $"ошибка инициализации discord rpc: {ex.Message}");
                initialized = false;
            }
        }

        // обновление статуса в discord
        public void UpdatePresence(string status, string details, string state)
        {
            if (!initialized)
            {
                Log.Error("DiscordRPC", "discord rpc не инициализирован!");
                return;
            }

            try
            {
                // обновляем presence в discord
                Discord_UpdatePresence(state ?? "", details ?? "");
                
                Log.Info("DiscordRPC", $"обновлен discord статус: {status}, детали: {details}, состояние: {state}");
            }
            catch (Exception ex)
            {
                Log.Error("DiscordRPC", $"ошибка обновления discord статуса: {ex.Message}");
            }
        }

        // обработка callback'ов (нужно вызывать регулярно)
        public void RunCallbacks()
        {
            if (initialized)
            {
                try
                {
                    Discord_RunCallbacks();
                }
                catch (Exception ex)
                {
                    Log.Error("DiscordRPC", $"ошибка обработки callback'ов: {ex.Message}");
                }
            }
        }

        // выключение discord rpc
        public void Shutdown()
        {
            if (initialized)
            {
                try
                {
                    Discord_Shutdown();
                    Log.Info("DiscordRPC", "discord rpc выключен");
                }
                catch (Exception ex)
                {
                    Log.Error("DiscordRPC", $"ошибка выключения discord rpc: {ex.Message}");
                }
                finally
                {
                    initialized = false;
                }
            }
        }

        // проверка инициализации
        public bool IsInitialized()
        {
            return initialized;
        }

        // получение application id
        public string GetApplicationId()
        {
            return applicationId;
        }
    }
}
