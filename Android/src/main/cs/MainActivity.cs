using System;
using Android.App;
using Android.OS;
using Android.Widget;
using Android.Content;
using Android.Graphics;
using Android.Views.Animations;

// главный актив приложения на c#
namespace StormDiscordRPC
{
    [Activity(Label = "StormDiscordRPC", MainLauncher = true, Theme = "@style/AppTheme")]
    public class MainActivity : Activity
    {
        // элементы интерфейса
        private EditText statusEditText;
        private EditText detailsEditText;
        private EditText stateEditText;
        private Button startButton;
        private Button stopButton;
        private Switch autoStartSwitch;
        private View backgroundView;

        protected override void OnCreate(Bundle savedInstanceState)
        {
            base.OnCreate(savedInstanceState);
            SetContentView(Resource.Layout.activity_main);

            // инициализация элементов
            InitializeViews();
            SetupClickListeners();
            SetupAnimations();
        }

        // инициализация всех view элементов
        private void InitializeViews()
        {
            statusEditText = FindViewById<EditText>(Resource.Id.statusEditText);
            detailsEditText = FindViewById<EditText>(Resource.Id.detailsEditText);
            stateEditText = FindViewById<EditText>(Resource.Id.stateEditText);
            startButton = FindViewById<Button>(Resource.Id.startButton);
            stopButton = FindViewById<Button>(Resource.Id.stopButton);
            autoStartSwitch = FindViewById<Switch>(Resource.Id.autoStartSwitch);
            backgroundView = FindViewById<View>(Resource.Id.backgroundView);
        }

        // настройка кликов на кнопки
        private void SetupClickListeners()
        {
            startButton.Click += StartDiscordRPC;
            stopButton.Click += StopDiscordRPC;
            
            // анимация при фокусе на полях ввода
            statusEditText.FocusChange += (sender, e) =>
            {
                if (e.HasFocus)
                {
                    AnimateBackground(Color.ParseColor("#1A1A2E"));
                }
            };
        }

        // запуск discord rpc
        private void StartDiscordRPC(object sender, EventArgs e)
        {
            string status = statusEditText.Text.Trim();
            string details = detailsEditText.Text.Trim();
            string state = stateEditText.Text.Trim();

            // проверка на пустой статус
            if (string.IsNullOrEmpty(status))
            {
                Toast.MakeText(this, "введи статус!", ToastLength.Short).Show();
                return;
            }

            // запуск сервиса
            Intent serviceIntent = new Intent(this, typeof(DiscordRPCService));
            serviceIntent.PutExtra("status", status);
            serviceIntent.PutExtra("details", details);
            serviceIntent.PutExtra("state", state);
            
            StartService(serviceIntent);
            
            Toast.MakeText(this, "discord rpc запущен!", ToastLength.Short).Show();
            UpdateUI(true);
            AnimateBackground(Color.ParseColor("#0F3460")); // синий фон при работе
        }

        // остановка discord rpc
        private void StopDiscordRPC(object sender, EventArgs e)
        {
            Intent serviceIntent = new Intent(this, typeof(DiscordRPCService));
            StopService(serviceIntent);
            
            Toast.MakeText(this, "discord rpc остановлен", ToastLength.Short).Show();
            UpdateUI(false);
            AnimateBackground(Color.ParseColor("#16213E")); // обычный фон
        }

        // обновление интерфейса
        private void UpdateUI(bool isRunning)
        {
            statusEditText.Enabled = !isRunning;
            detailsEditText.Enabled = !isRunning;
            stateEditText.Enabled = !isRunning;
            startButton.Enabled = !isRunning;
            stopButton.Enabled = isRunning;
            autoStartSwitch.Enabled = !isRunning;
            
            // анимация кнопок
            if (isRunning)
            {
                startButton.Animate().Alpha(0.5f).SetDuration(300);
                stopButton.Animate().Alpha(1.0f).SetDuration(300);
            }
            else
            {
                startButton.Animate().Alpha(1.0f).SetDuration(300);
                stopButton.Animate().Alpha(0.5f).SetDuration(300);
            }
        }

        // анимация фона
        private void AnimateBackground(Color color)
        {
            backgroundView.SetBackgroundColor(color);
        }

        // настройка начальных анимаций
        private void SetupAnimations()
        {
            // плавное появление элементов
            statusEditText.Alpha = 0f;
            detailsEditText.Alpha = 0f;
            stateEditText.Alpha = 0f;
            startButton.Alpha = 0f;
            stopButton.Alpha = 0f;

            statusEditText.Animate().Alpha(1f).SetDuration(800).SetStartDelay(100);
            detailsEditText.Animate().Alpha(1f).SetDuration(800).SetStartDelay(200);
            stateEditText.Animate().Alpha(1f).SetDuration(800).SetStartDelay(300);
            startButton.Animate().Alpha(1f).SetDuration(800).SetStartDelay(400);
            stopButton.Animate().Alpha(1f).SetDuration(800).SetStartDelay(500);
        }
    }
}
