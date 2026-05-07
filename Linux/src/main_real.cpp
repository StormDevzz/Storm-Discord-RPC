#include <gtkmm.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "discord_rpc_real.h"

// класс для начального окна подключения
class ConnectWindow : public Gtk::Window {
private:
    // виджеты
    Gtk::Box main_box;
    Gtk::Label title_label;
    Gtk::Label desc_label;
    Gtk::Entry client_id_entry;
    Gtk::Button connect_button;
    Gtk::Button check_button;
    Gtk::Label status_label;
    Gtk::ProgressBar progress_bar;
    Gtk::Spinner spinner;
    
    // состояние
    bool connecting;
    bool connected;
    
    // Discord RPC
    DiscordRPCReal discord;
    
    // обработчики
    void on_connect_clicked();
    void on_check_clicked();
    void on_client_id_changed();
    bool validate_client_id(const std::string& id);
    bool update_ui();
    
public:
    ConnectWindow();
    virtual ~ConnectWindow();
    
    // проверка подключения
    bool is_connected() const { return connected; }
    std::string get_client_id() const;
};

ConnectWindow::ConnectWindow() 
    : main_box(Gtk::Orientation::VERTICAL, 15),
      connecting(false),
      connected(false) {
    
    // настройка окна
    set_title("Storm Discord RPC - Подключение");
    set_default_size(450, 350);
    set_resizable(false);
    set_margin(20);
    
    // заголовок
    title_label.set_text("Storm Discord RPC");
    title_label.add_css_class("title-1");
    title_label.set_margin_bottom(10);
    
    // описание
    desc_label.set_text("Введите ваш Discord Application ID для подключения к Rich Presence");
    desc_label.set_wrap(true);
    desc_label.set_margin_bottom(20);
    desc_label.add_css_class("body");
    
    // поле ввода Client ID
    client_id_entry.set_placeholder_text("Например: 1234567890123456789");
    client_id_entry.set_margin_bottom(10);
    
    // кнопки
    connect_button.set_label("Подключиться");
    connect_button.set_sensitive(false);
    connect_button.add_css_class("suggested-action");
    
    check_button.set_label("Проверить Discord");
    check_button.add_css_class("pill");
    
    // статус
    status_label.set_text("Введите Client ID для начала");
    status_label.add_css_class("caption");
    status_label.set_margin_top(10);
    
    // прогресс
    progress_bar.set_visible(false);
    progress_bar.set_margin_top(10);
    
    spinner.set_visible(false);
    spinner.set_margin_top(10);
    
    // упаковка
    main_box.append(title_label);
    main_box.append(desc_label);
    main_box.append(client_id_entry);
    main_box.append(connect_button);
    main_box.append(check_button);
    main_box.append(status_label);
    main_box.append(progress_bar);
    main_box.append(spinner);
    
    set_child(main_box);
    
    // обработчики событий
    connect_button.signal_clicked().connect(sigc::mem_fun(*this, &ConnectWindow::on_connect_clicked));
    check_button.signal_clicked().connect(sigc::mem_fun(*this, &ConnectWindow::on_check_clicked));
    client_id_entry.signal_changed().connect(sigc::mem_fun(*this, &ConnectWindow::on_client_id_changed));
    
    // таймер для обновления UI
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &ConnectWindow::update_ui), 100);
}

ConnectWindow::~ConnectWindow() {
    discord.disconnect();
}

void ConnectWindow::on_connect_clicked() {
    std::string client_id = client_id_entry.get_text();
    
    if (!validate_client_id(client_id)) {
        status_label.set_text("❌ Неверный формат Client ID");
        status_label.remove_css_class("success");
        status_label.add_css_class("error");
        return;
    }
    
    connecting = true;
    connect_button.set_sensitive(false);
    client_id_entry.set_sensitive(false);
    check_button.set_sensitive(false);
    
    status_label.set_text("🔄 Подключение к Discord...");
    status_label.remove_css_class("error");
    status_label.add_css_class("accent");
    
    progress_bar.set_visible(true);
    progress_bar.set_pulse_step(0.1);
    spinner.set_visible(true);
    spinner.start();
    
    // подключение в отдельном потоке
    std::thread([this, client_id]() {
        bool success = discord.initialize(client_id);
        
        // обновление UI в главном потоке
        Glib::signal_idle().connect_once([this, success]() {
            connecting = false;
            progress_bar.set_visible(false);
            spinner.stop();
            spinner.set_visible(false);
            
            if (success) {
                connected = true;
                status_label.set_text("✅ Успешное подключение к Discord!");
                status_label.remove_css_class("error");
                status_label.remove_css_class("accent");
                status_label.add_css_class("success");
                
                // закрываем окно через секунду
                Glib::signal_timeout().connect_once([this]() {
                    hide(); // скрываем вместо close()
                }, 1000);
            } else {
                connected = false;
                std::string error = discord.getLastError();
                status_label.set_text("❌ " + error);
                status_label.remove_css_class("success");
                status_label.remove_css_class("accent");
                status_label.add_css_class("error");
                
                connect_button.set_sensitive(true);
                client_id_entry.set_sensitive(true);
                check_button.set_sensitive(true);
            }
        });
    }).detach();
}

void ConnectWindow::on_check_clicked() {
    if (DiscordRPCReal::isDiscordAvailable()) {
        status_label.set_text("✅ Discord запущен и доступен");
        status_label.remove_css_class("error");
        status_label.add_css_class("success");
    } else {
        status_label.set_text("❌ Discord не найден. Запустите Discord!");
        status_label.remove_css_class("success");
        status_label.add_css_class("error");
    }
}

void ConnectWindow::on_client_id_changed() {
    std::string client_id = client_id_entry.get_text();
    
    if (client_id.empty()) {
        connect_button.set_sensitive(false);
        status_label.set_text("Введите Client ID для начала");
        status_label.remove_css_class("error");
        status_label.remove_css_class("success");
    } else if (validate_client_id(client_id)) {
        connect_button.set_sensitive(true);
        status_label.set_text("✅ Client ID выглядит корректно");
        status_label.remove_css_class("error");
        status_label.add_css_class("success");
    } else {
        connect_button.set_sensitive(false);
        status_label.set_text("❌ Client ID должен содержать 17-19 цифр");
        status_label.remove_css_class("success");
        status_label.add_css_class("error");
    }
}

bool ConnectWindow::validate_client_id(const std::string& id) {
    std::string error;
    return ClientIdValidator::fullValidate(id, error);
}

bool ConnectWindow::update_ui() {
    if (connecting) {
        progress_bar.pulse();
    } else if (connected) {
        discord.processEvents();
    }
    return true;
}

std::string ConnectWindow::get_client_id() const {
    return client_id_entry.get_text();
}

// главное окно управления статусом
class MainWindow : public Gtk::Window {
private:
    // виджеты
    Gtk::Box main_box;
    Gtk::HeaderBar header_bar;
    Gtk::Notebook notebook;
    
    // вкладка статуса
    Gtk::Box status_box;
    Gtk::Grid status_grid;
    Gtk::Label client_id_label;
    Gtk::Entry state_entry;
    Gtk::Entry details_entry;
    Gtk::Entry large_image_entry;
    Gtk::Entry large_text_entry;
    Gtk::Entry small_image_entry;
    Gtk::Entry small_text_entry;
    Gtk::Button update_button;
    Gtk::Button clear_button;
    Gtk::Label status_label;
    
    // вкладка информации
    Gtk::Box info_box;
    Gtk::Label info_label;
    Gtk::Button disconnect_button;
    Gtk::Button reconnect_button;
    
    // Discord RPC
    DiscordRPCReal discord;
    std::string client_id;
    
    // обработчики
    void on_update_clicked();
    void on_clear_clicked();
    void on_disconnect_clicked();
    void on_reconnect_clicked();
    bool update_status();
    
public:
    MainWindow(const std::string& cid);
    virtual ~MainWindow();
    
    // конструктор по умолчанию для make_managed
    MainWindow() = delete;
};

MainWindow::MainWindow(const std::string& cid) 
    : main_box(Gtk::Orientation::VERTICAL),
      status_box(Gtk::Orientation::VERTICAL, 10),
      info_box(Gtk::Orientation::VERTICAL, 10),
      client_id(cid) {
    
    // настройка окна
    set_title("Storm Discord RPC");
    set_default_size(500, 600);
    
    // заголовок
    header_bar.set_title_widget(*Gtk::make_managed<Gtk::Label>("Storm Discord RPC"));
    set_titlebar(header_bar);
    
    // метка Client ID
    client_id_label.set_text("Client ID: " + client_id);
    client_id_label.add_css_class("caption");
    client_id_label.set_margin_bottom(10);
    
    // поля статуса
    status_grid.set_row_spacing(10);
    status_grid.set_column_spacing(10);
    status_grid.set_margin_start(20);
    status_grid.set_margin_end(20);
    
    status_grid.attach(*Gtk::make_managed<Gtk::Label>("Состояние:"), 0, 0);
    state_entry.set_placeholder_text("например: Играю в игру");
    status_grid.attach(state_entry, 1, 0);
    
    status_grid.attach(*Gtk::make_managed<Gtk::Label>("Детали:"), 0, 1);
    details_entry.set_placeholder_text("например: Уровень 5");
    status_grid.attach(details_entry, 1, 1);
    
    status_grid.attach(*Gtk::make_managed<Gtk::Label>("Большое изображение:"), 0, 2);
    large_image_entry.set_placeholder_text("ключ изображения");
    status_grid.attach(large_image_entry, 1, 2);
    
    status_grid.attach(*Gtk::make_managed<Gtk::Label>("Текст большого изображения:"), 0, 3);
    large_text_entry.set_placeholder_text("описание изображения");
    status_grid.attach(large_text_entry, 1, 3);
    
    status_grid.attach(*Gtk::make_managed<Gtk::Label>("Маленькое изображение:"), 0, 4);
    small_image_entry.set_placeholder_text("ключ изображения");
    status_grid.attach(small_image_entry, 1, 4);
    
    status_grid.attach(*Gtk::make_managed<Gtk::Label>("Текст маленького изображения:"), 0, 5);
    small_text_entry.set_placeholder_text("описание изображения");
    status_grid.attach(small_text_entry, 1, 5);
    
    // кнопки
    auto button_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
    button_box->set_margin_top(20);
    button_box->set_margin_bottom(10);
    
    update_button.set_label("Обновить статус");
    update_button.add_css_class("suggested-action");
    
    clear_button.set_label("Очистить поля");
    
    button_box->append(update_button);
    button_box->append(clear_button);
    
    // статус
    status_label.set_text("Готов к работе");
    status_label.add_css_class("caption");
    status_label.set_margin_bottom(10);
    
    // сборка вкладки статуса
    status_box.append(client_id_label);
    status_box.append(status_grid);
    status_box.append(*button_box);
    status_box.append(status_label);
    
    // вкладка информации
    info_label.set_text("Информация о подключении");
    info_label.add_css_class("heading");
    
    auto info_text = Gtk::make_managed<Gtk::Label>();
    info_text->set_text("Вы подключены к Discord Rich Presence с Client ID: " + client_id + "\n\n"
                      "Теперь вы можете настраивать свой статус Discord.\n"
                      "Статус будет отображаться в вашем профиле Discord.");
    info_text->set_wrap(true);
    info_text->set_margin_top(10);
    
    auto info_button_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
    info_button_box->set_margin_top(20);
    
    disconnect_button.set_label("Отключиться");
    disconnect_button.add_css_class("destructive-action");
    
    reconnect_button.set_label("Переподключиться");
    reconnect_button.add_css_class("suggested-action");
    
    info_button_box->append(disconnect_button);
    info_button_box->append(reconnect_button);
    
    info_box.append(info_label);
    info_box.append(*info_text);
    info_box.append(*info_button_box);
    
    // вкладки
    notebook.append_page(status_box, "Статус");
    notebook.append_page(info_box, "Информация");
    
    // сборка главного окна
    main_box.append(notebook);
    set_child(main_box);
    
    // обработчики
    update_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_update_clicked));
    clear_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_clear_clicked));
    disconnect_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_disconnect_clicked));
    reconnect_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_reconnect_clicked));
    
    // инициализация Discord
    discord.initialize(client_id);
    
    // таймер для обработки событий
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &MainWindow::update_status), 1000);
}

MainWindow::~MainWindow() {
    discord.disconnect();
}

void MainWindow::on_update_clicked() {
    std::string state = state_entry.get_text();
    std::string details = details_entry.get_text();
    std::string large_image = large_image_entry.get_text();
    std::string large_text = large_text_entry.get_text();
    std::string small_image = small_image_entry.get_text();
    std::string small_text = small_text_entry.get_text();
    
    discord.updatePresence(state, details, large_image, large_text, small_image, small_text);
    
    status_label.set_text("✅ Статус обновлен");
    status_label.remove_css_class("error");
    status_label.add_css_class("success");
}

void MainWindow::on_clear_clicked() {
    state_entry.set_text("");
    details_entry.set_text("");
    large_image_entry.set_text("");
    large_text_entry.set_text("");
    small_image_entry.set_text("");
    small_text_entry.set_text("");
    
    status_label.set_text("Поля очищены");
    status_label.remove_css_class("success");
    status_label.remove_css_class("error");
}

void MainWindow::on_disconnect_clicked() {
    discord.disconnect();
    status_label.set_text("❌ Отключено от Discord");
    status_label.remove_css_class("success");
    status_label.add_css_class("error");
}

void MainWindow::on_reconnect_clicked() {
    if (discord.initialize(client_id)) {
        status_label.set_text("✅ Переподключено к Discord");
        status_label.remove_css_class("error");
        status_label.add_css_class("success");
    } else {
        status_label.set_text("❌ Ошибка переподключения");
        status_label.remove_css_class("success");
        status_label.add_css_class("error");
    }
}

bool MainWindow::update_status() {
    discord.processEvents();
    return true;
}

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("org.storm.discordrpc");
    
    // создаем окно подключения
    ConnectWindow connect_window;
    
    // запускаем окно подключения
    int result = app->make_window_and_run<ConnectWindow>(argc, argv);
    
    // если пользователь закрыл окно подключения
    if (result != 0) {
        return 0;
    }
    
    // если подключение успешно, показываем главное окно
    if (connect_window.is_connected()) {
        MainWindow main_window(connect_window.get_client_id());
        return app->make_window_and_run<MainWindow>(argc, argv);
    }
    
    return 0;
}
