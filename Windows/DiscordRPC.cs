using System;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace StormDiscordRPC
{
    // класс для работы с Discord Rich Presence на C#
    public class DiscordRPC
    {
        private bool connected = false;
        private string clientId = "";
        private CancellationTokenSource cancellationTokenSource;
        
        // импорт функций из C++ библиотеки (Windows)
        [DllImport("discord_rpc.dll", CharSet = CharSet.Unicode)]
        private static extern bool discord_initialize(string client_id);
        
        [DllImport("discord_rpc.dll", CharSet = CharSet.Unicode)]
        private static extern void discord_update_presence(
            string state, string details,
            string large_image_key, string large_image_text,
            string small_image_key, string small_image_text);
        
        [DllImport("discord_rpc.dll")]
        private static extern void discord_shutdown();
        
        [DllImport("discord_rpc.dll")]
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
                MessageBox.Show($"ошибка инициализации: {ex.Message}", "Ошибка", 
                              MessageBoxButtons.OK, MessageBoxIcon.Error);
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
                MessageBox.Show("ошибка: не подключен к Discord", "Ошибка", 
                              MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            
            try
            {
                discord_update_presence(state, details, 
                                       largeImageKey, largeImageText,
                                       smallImageKey, smallImageText);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"ошибка обновления статуса: {ex.Message}", "Ошибка", 
                              MessageBoxButtons.OK, MessageBoxIcon.Error);
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
            }
            catch (Exception ex)
            {
                MessageBox.Show($"ошибка отключения: {ex.Message}", "Ошибка", 
                              MessageBoxButtons.OK, MessageBoxIcon.Error);
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
                catch (Exception)
                {
                    // игнорируем ошибки в цикле обработки событий
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
    
    // главная форма приложения
    public class MainForm : Form
    {
        private DiscordRPC discord;
        private TextBox clientIdTextBox;
        private Button connectButton;
        private Button disconnectButton;
        private GroupBox statusGroupBox;
        private TextBox stateTextBox;
        private TextBox detailsTextBox;
        private TextBox largeImageTextBox;
        private TextBox largeTextTextBox;
        private TextBox smallImageTextBox;
        private TextBox smallTextTextBox;
        private Button updateButton;
        private Label statusLabel;
        
        public MainForm()
        {
            discord = new DiscordRPC();
            InitializeComponent();
        }
        
        private void InitializeComponent()
        {
            // настройка формы
            this.Text = "Storm Discord RPC";
            this.Size = new System.Drawing.Size(450, 550);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.FormBorderStyle = FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            
            // Client ID
            var clientIdLabel = new Label { Text = "Client ID:", Left = 20, Top = 20, Width = 100 };
            clientIdTextBox = new TextBox { Left = 20, Top = 45, Width = 380 };
            
            // кнопки подключения
            connectButton = new Button { Text = "Подключиться", Left = 20, Top = 80, Width = 110 };
            disconnectButton = new Button { Text = "Отключиться", Left = 140, Top = 80, Width = 110 };
            disconnectButton.Enabled = false;
            
            // группа настроек статуса
            statusGroupBox = new GroupBox { Text = "Настройки статуса", Left = 20, Top = 120, Width = 380, Height = 280 };
            
            // поля для статуса
            var stateLabel = new Label { Text = "Состояние:", Left = 10, Top = 25, Width = 100 };
            stateTextBox = new TextBox { Left = 10, Top = 50, Width = 350 };
            stateTextBox.PlaceholderText = "например: Играю в игру";
            
            var detailsLabel = new Label { Text = "Детали:", Left = 10, Top = 80, Width = 100 };
            detailsTextBox = new TextBox { Left = 10, Top = 105, Width = 350 };
            detailsTextBox.PlaceholderText = "например: Уровень 5";
            
            var largeImageLabel = new Label { Text = "Большое изображение:", Left = 10, Top = 135, Width = 120 };
            largeImageTextBox = new TextBox { Left = 10, Top = 160, Width = 350 };
            largeImageTextBox.PlaceholderText = "ключ изображения";
            
            var largeTextLabel = new Label { Text = "Текст большого изображения:", Left = 10, Top = 190, Width = 160 };
            largeTextTextBox = new TextBox { Left = 10, Top = 215, Width = 350 };
            largeTextTextBox.PlaceholderText = "описание изображения";
            
            var smallImageLabel = new Label { Text = "Маленькое изображение:", Left = 10, Top = 245, Width = 140 };
            smallImageTextBox = new TextBox { Left = 10, Top = 270, Width = 350 };
            smallImageTextBox.PlaceholderText = "ключ изображения";
            
            var smallTextLabel = new Label { Text = "Текст маленького изображения:", Left = 10, Top = 305, Width = 180 };
            smallTextTextBox = new TextBox { Left = 10, Top = 330, Width = 350 };
            smallTextTextBox.PlaceholderText = "описание изображения";
            
            // добавление полей в группу
            statusGroupBox.Controls.AddRange(new Control[] {
                stateLabel, stateTextBox, detailsLabel, detailsTextBox,
                largeImageLabel, largeImageTextBox, largeTextLabel, largeTextTextBox,
                smallImageLabel, smallImageTextBox, smallTextLabel, smallTextTextBox
            });
            
            // кнопка обновления статуса
            updateButton = new Button { Text = "Обновить статус", Left = 20, Top = 420, Width = 380 };
            updateButton.Enabled = false;
            
            // метка статуса
            statusLabel = new Label { Text = "Статус: не подключен", Left = 20, Top = 460, Width = 380 };
            
            // добавление всех элементов на форму
            this.Controls.AddRange(new Control[] {
                clientIdLabel, clientIdTextBox, connectButton, disconnectButton,
                statusGroupBox, updateButton, statusLabel
            });
            
            // обработчики событий
            connectButton.Click += OnConnectClicked;
            disconnectButton.Click += OnDisconnectClicked;
            updateButton.Click += OnUpdateClicked;
            this.FormClosing += OnFormClosing;
        }
        
        private void OnConnectClicked(object sender, EventArgs e)
        {
            string clientId = clientIdTextBox.Text.Trim();
            
            if (string.IsNullOrEmpty(clientId))
            {
                MessageBox.Show("Ошибка: введите Client ID", "Ошибка", 
                              MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            
            if (discord.Initialize(clientId))
            {
                connectButton.Enabled = false;
                disconnectButton.Enabled = true;
                updateButton.Enabled = true;
                clientIdTextBox.Enabled = false;
                statusLabel.Text = "Статус: подключено";
            }
        }
        
        private void OnDisconnectClicked(object sender, EventArgs e)
        {
            discord.Disconnect();
            connectButton.Enabled = true;
            disconnectButton.Enabled = false;
            updateButton.Enabled = false;
            clientIdTextBox.Enabled = true;
            statusLabel.Text = "Статус: не подключен";
        }
        
        private void OnUpdateClicked(object sender, EventArgs e)
        {
            discord.UpdatePresence(
                stateTextBox.Text,
                detailsTextBox.Text,
                largeImageTextBox.Text,
                largeTextTextBox.Text,
                smallImageTextBox.Text,
                smallTextTextBox.Text
            );
        }
        
        private void OnFormClosing(object sender, FormClosingEventArgs e)
        {
            discord.Dispose();
        }
    }
    
    // точка входа
    static class Program
    {
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
        }
    }
}
