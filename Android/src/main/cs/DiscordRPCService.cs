using System;
using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using AndroidX.Core.App;

// фоновый сервис для работы с discord rpc на c#
namespace StormDiscordRPC
{
    [Service]
    public class DiscordRPCService : Service
    {
        private const string CHANNEL_ID = "DiscordRPCChannel";
        private const int NOTIFICATION_ID = 1;
        
        private DiscordRPCManager discordRPCManager;
        private bool isRunning = false;

        public override void OnCreate()
        {
            base.OnCreate();
            CreateNotificationChannel();
            discordRPCManager = new DiscordRPCManager();
        }

        public override StartCommandResult OnStartCommand(Intent intent, StartCommandFlags flags, int startId)
        {
            if (intent != null && !isRunning)
            {
                // получаем данные из интента
                string status = intent.GetStringExtra("status");
                string details = intent.GetStringExtra("details");
                string state = intent.GetStringExtra("state");

                // запускаем сервис в foreground
                StartForeground(NOTIFICATION_ID, CreateNotification());
                
                // инициализируем discord rpc
                discordRPCManager.Initialize("1234567890123456789"); // твой discord app id
                discordRPCManager.UpdatePresence(status, details, state);
                isRunning = true;
            }
            
            return StartCommandResult.Sticky;
        }

        public override IBinder OnBind(Intent intent)
        {
            return null;
        }

        public override void OnDestroy()
        {
            base.OnDestroy();
            if (discordRPCManager != null && isRunning)
            {
                discordRPCManager.Shutdown();
                isRunning = false;
            }
        }

        // создаем канал уведомлений для android 8+
        private void CreateNotificationChannel()
        {
            if (Build.VERSION.SdkInt >= BuildVersionCodes.O)
            {
                NotificationChannel serviceChannel = new NotificationChannel(
                    CHANNEL_ID,
                    "Discord RPC Service",
                    NotificationImportance.Default
                );
                serviceChannel.Description = "управляет discord rich presence";
                
                NotificationManager manager = (NotificationManager)GetSystemService(NotificationService);
                manager.CreateNotificationChannel(serviceChannel);
            }
        }

        // создаем уведомление для foreground сервиса
        private Notification CreateNotification()
        {
            NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
                .SetContentTitle("Storm Discord RPC")
                .SetContentText("discord rich presence активен")
                .SetSmallIcon(Resource.Drawable.ic_notification)
                .SetOngoing(true)
                .SetColor(Color.ParseColor("#533483")); // фиолетовый цвет

            return builder.Build();
        }
    }
}
