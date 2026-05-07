#include <gtkmm.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "../include/discord_rpc_real.h"
#include "../include/config_manager.h"

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
    DiscordRPCReal discord;
    ConfigManager config;
    bool connected;
    bool connecting;
    
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
    
    // сигнал успешного подключения
    sigc::signal<void()> signal_connected_;
};

ConnectWindow::ConnectWindow() 
    : Gtk::Window(), 
      discord(),
      config(),
      connected(false), 
      connecting(false) {
    
    // настройка окна
    set_title("Storm Discord RPC - Подключение");
    set_default_size(600, 450);
    set_resizable(true);
    set_margin(30);
    
    // загружаем Client ID из конфигурации
    std::string saved_client_id = config.get_client_id();
    if (!saved_client_id.empty()) {
        client_id_entry.set_text(saved_client_id);
    }
    
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
    client_id_entry.set_margin_bottom(20);
    client_id_entry.set_size_request(400, -1);
    
    // кнопки
    auto button_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 15);
    button_box->set_halign(Gtk::Align::CENTER);
    
    connect_button.set_label("Подключиться");
    connect_button.set_sensitive(false);
    connect_button.add_css_class("suggested-action");
    connect_button.set_size_request(180, 45);
    
    check_button.set_label("Проверить Discord");
    check_button.add_css_class("pill");
    check_button.set_size_request(180, 45);
    
    button_box->append(connect_button);
    button_box->append(check_button);
    
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
    main_box.append(*button_box);
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
        status_label.set_text("Неверный формат Client ID");
        status_label.remove_css_class("success");
        status_label.add_css_class("error");
        return;
    }
    
    connecting = true;
    connect_button.set_sensitive(false);
    client_id_entry.set_sensitive(false);
    check_button.set_sensitive(false);
    
    status_label.set_text("Подключение к Discord...");
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
        Glib::signal_idle().connect_once([this, success, client_id]() {
            connecting = false;
            progress_bar.set_visible(false);
            spinner.stop();
            spinner.set_visible(false);
            
            if (success) {
                connected = true;
                status_label.set_text("Успешное подключение! Перехожу к настройкам...");
                status_label.remove_css_class("error");
                status_label.remove_css_class("accent");
                status_label.add_css_class("success");
                
                // сохраняем Client ID в конфигурацию
                config.set_client_id(client_id);
                config.save_config();
                
                // отправляем сигнал об успешном подключении
                signal_connected_.emit();
                
                // закрываем окно через секунду
                Glib::signal_timeout().connect_once([this]() {
                    hide();
                }, 1000);
            } else {
                connected = false;
                std::string error = discord.getLastError();
                status_label.set_text("Ошибка: " + error);
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

class MainWindow : public Gtk::Window {
private:
    // виджеты
    Gtk::Box main_box;
    Gtk::Notebook notebook;
    Gtk::Box status_box;
    Gtk::Box settings_box;
    Gtk::Box presets_box;
    Gtk::Box about_box;
    
    // виджеты для заголовка и статуса
    Gtk::HeaderBar header_bar;
    Gtk::Label client_id_label;
    Gtk::Grid status_grid;
    Gtk::Label info_label;
    
    // вкладка статуса
    Gtk::Label status_title;
    Gtk::Box status_fields_box;
    Gtk::Box state_box;
    Gtk::Label state_label;
    Gtk::Entry state_entry;
    Gtk::Box details_box;
    Gtk::Label details_label;
    Gtk::Entry details_entry;
    Gtk::Box images_box;
    Gtk::Label images_title;
    Gtk::Box large_image_box;
    Gtk::Label large_image_label;
    Gtk::Entry large_image_entry;
    Gtk::Label large_text_label;
    Gtk::Entry large_text_entry;
    Gtk::Box small_image_box;
    Gtk::Label small_image_label;
    Gtk::Entry small_image_entry;
    Gtk::Label small_text_label;
    Gtk::Entry small_text_entry;
    Gtk::Box status_buttons_box;
    Gtk::Button update_button;
    Gtk::Button clear_button;
    Gtk::Button clear_status_button;
    Gtk::Label status_label;
    
    // вкладка настроек
    Gtk::Label settings_title;
    Gtk::Box settings_content_box;
    Gtk::Button disconnect_button;
    Gtk::Button reconnect_button;
    Gtk::Label settings_status_label;
    
    // вкладка пресетов
    Gtk::Label presets_title;
    Gtk::Box presets_content_box;
    Gtk::Box preset_list_box;
    Gtk::Button add_preset_button;
    Gtk::Button save_preset_button;
    Gtk::Entry preset_name_entry;
    Gtk::ComboBoxText preset_combo;
    
    // вкладка о программе
    Gtk::Label about_title;
    Gtk::Label about_desc;
    Gtk::Label about_version;
    Gtk::Label about_author;
    
    DiscordRPCReal discord;
    ConfigManager config;
    std::string client_id;
    
    // обработчики
    void on_update_clicked();
    void on_clear_clicked();
    void on_clear_status_clicked();
    void on_disconnect_clicked();
    void on_reconnect_clicked();
    void on_add_preset_clicked();
    void on_save_preset_clicked();
    void on_preset_changed();
    bool update_status();
    void load_presets();
    void save_last_status();
    bool on_close_window();
    
public:
    MainWindow(const std::string& cid);
    virtual ~MainWindow();
    
    // конструктор по умолчанию для make_managed
    MainWindow() = delete;
};

MainWindow::MainWindow(const std::string& cid) 
    : main_box(Gtk::Orientation::VERTICAL),
      status_box(Gtk::Orientation::VERTICAL, 10),
      settings_box(Gtk::Orientation::VERTICAL, 10),
      presets_box(Gtk::Orientation::VERTICAL, 10),
      about_box(Gtk::Orientation::VERTICAL, 10),
      discord(),
      config(),
      client_id(cid) {
    
    // настройка окна
    set_title("Storm Discord RPC");
    set_default_size(800, 700);
    set_resizable(true);
    
    // заголовок
    header_bar.set_title_widget(*Gtk::make_managed<Gtk::Label>("Storm Discord RPC"));
    set_titlebar(header_bar);
    
    // метка Client ID
    client_id_label.set_text("Client ID: " + client_id);
    client_id_label.add_css_class("caption");
    client_id_label.set_margin_bottom(10);
    
    // поля статуса
    status_grid.set_row_spacing(15);
    status_grid.set_column_spacing(15);
    status_grid.set_margin_start(30);
    status_grid.set_margin_end(30);
    status_grid.set_margin_top(20);
    status_grid.set_margin_bottom(20);
    
    status_grid.attach(*Gtk::make_managed<Gtk::Label>("Состояние:"), 0, 0);
    state_entry.set_placeholder_text("например: Играю в игру");
    state_entry.set_size_request(300, -1);
    status_grid.attach(state_entry, 1, 0);
    
    status_grid.attach(*Gtk::make_managed<Gtk::Label>("Детали:"), 0, 1);
    details_entry.set_placeholder_text("например: Уровень 5");
    details_entry.set_size_request(300, -1);
    status_grid.attach(details_entry, 1, 1);
    
    status_grid.attach(*Gtk::make_managed<Gtk::Label>("Большое изображение:"), 0, 2);
    large_image_entry.set_placeholder_text("ключ изображения");
    large_image_entry.set_size_request(300, -1);
    status_grid.attach(large_image_entry, 1, 2);
    
    status_grid.attach(*Gtk::make_managed<Gtk::Label>("Текст большого изображения:"), 0, 3);
    large_text_entry.set_placeholder_text("описание изображения");
    large_text_entry.set_size_request(300, -1);
    status_grid.attach(large_text_entry, 1, 3);
    
    status_grid.attach(*Gtk::make_managed<Gtk::Label>("Маленькое изображение:"), 0, 4);
    small_image_entry.set_placeholder_text("ключ изображения");
    small_image_entry.set_size_request(300, -1);
    status_grid.attach(small_image_entry, 1, 4);
    
    status_grid.attach(*Gtk::make_managed<Gtk::Label>("Текст маленького изображения:"), 0, 5);
    small_text_entry.set_placeholder_text("описание изображения");
    small_text_entry.set_size_request(300, -1);
    status_grid.attach(small_text_entry, 1, 5);
    
    // загружаем последний статус из конфигурации
    std::string last_state, last_details, last_large_image, last_large_text, last_small_image, last_small_text;
    config.get_last_status(last_state, last_details, last_large_image, last_large_text, last_small_image, last_small_text);
    
    if (!last_state.empty()) state_entry.set_text(last_state);
    if (!last_details.empty()) details_entry.set_text(last_details);
    if (!last_large_image.empty()) large_image_entry.set_text(last_large_image);
    if (!last_large_text.empty()) large_text_entry.set_text(last_large_text);
    if (!last_small_image.empty()) small_image_entry.set_text(last_small_image);
    if (!last_small_text.empty()) small_text_entry.set_text(last_small_text);
    
    // кнопки
    auto button_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 15);
    button_box->set_margin_top(30);
    button_box->set_margin_bottom(20);
    button_box->set_halign(Gtk::Align::CENTER);
    
    update_button.set_label("Обновить статус");
    update_button.add_css_class("suggested-action");
    update_button.set_size_request(150, 40);
    
    clear_button.set_label("Очистить поля");
    clear_button.set_size_request(150, 40);
    
    clear_status_button.set_label("Очистить статус Discord");
    clear_status_button.add_css_class("destructive-action");
    clear_status_button.set_size_request(180, 40);
    
    button_box->append(update_button);
    button_box->append(clear_button);
    button_box->append(clear_status_button);
    
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
                      "Статус будет отображаться в вашем профиле Discord.\n\n"
                      "Поле ввода Client ID");
    info_text->set_wrap(true);
    info_text->set_margin_top(10);
    
    auto info_button_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 15);
    info_button_box->set_margin_top(30);
    info_button_box->set_halign(Gtk::Align::CENTER);
    
    disconnect_button.set_label("Отключиться");
    disconnect_button.add_css_class("destructive-action");
    disconnect_button.set_size_request(150, 40);
    
    reconnect_button.set_label("Переподключиться");
    reconnect_button.add_css_class("suggested-action");
    reconnect_button.set_size_request(150, 40);
    
    info_button_box->append(disconnect_button);
    info_button_box->append(reconnect_button);
    
    settings_box.append(info_label);
    settings_box.append(*info_text);
    settings_box.append(*info_button_box);
    
    // вкладки
    notebook.append_page(status_box, "Статус");
    notebook.append_page(settings_box, "Настройки");
    
    // сборка главного окна
    main_box.append(notebook);
    set_child(main_box);
    
    // обработчики
    update_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_update_clicked));
    clear_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_clear_clicked));
    clear_status_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_clear_status_clicked));
    disconnect_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_disconnect_clicked));
    reconnect_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_reconnect_clicked));
    
    // обработчик закрытия окна
    signal_close_request().connect(sigc::mem_fun(*this, &MainWindow::on_close_window), false);
    
    // инициализация Discord в отдельном потоке чтобы не блокировать интерфейс
    std::thread([this]() {
        if (discord.initialize(client_id)) {
            Glib::signal_idle().connect_once([this]() {
                status_label.set_text("Подключено к Discord");
                status_label.remove_css_class("error");
                status_label.add_css_class("success");
            });
        } else {
            Glib::signal_idle().connect_once([this]() {
                status_label.set_text("Ошибка подключения к Discord");
                status_label.remove_css_class("success");
                status_label.add_css_class("error");
            });
        }
    }).detach();
    
    // таймер для обработки событий Discord (более частый)
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &MainWindow::update_status), 100);
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
    
    // сохраняем последний статус
    save_last_status();
    
    status_label.set_text("Статус обновлен");
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
    status_label.set_text("Отключено от Discord");
    status_label.remove_css_class("success");
    status_label.add_css_class("error");
    
    // закрываем главное окно и возвращаемся к начальному
    Glib::signal_timeout().connect_once([this]() {
        hide();
    }, 1000);
}

void MainWindow::on_reconnect_clicked() {
    if (discord.initialize(client_id)) {
        status_label.set_text("Переподключено к Discord");
        status_label.remove_css_class("error");
        status_label.add_css_class("success");
    } else {
        status_label.set_text("Ошибка переподключения");
        status_label.remove_css_class("success");
        status_label.add_css_class("error");
    }
}

bool MainWindow::update_status() {
    // неблокирующая обработка событий Discord
    try {
        discord.processEvents();
    } catch (...) {
        // игнорируем ошибки чтобы не блокировать интерфейс
    }
    return true;
}

void MainWindow::save_last_status() {
    std::string state = state_entry.get_text();
    std::string details = details_entry.get_text();
    std::string large_image = large_image_entry.get_text();
    std::string large_text = large_text_entry.get_text();
    std::string small_image = small_image_entry.get_text();
    std::string small_text = small_text_entry.get_text();
    
    config.set_last_status(state, details, large_image, large_text, small_image, small_text);
    config.save_config();
}

void MainWindow::load_presets() {
    // загружаем пресеты в комбобокс
    preset_combo.remove_all();
    preset_combo.append(""); // пустой элемент
    
    auto presets = config.get_presets();
    for (const auto& preset : presets) {
        preset_combo.append(preset.name);
    }
}

void MainWindow::on_add_preset_clicked() {
    std::string name = preset_name_entry.get_text();
    if (name.empty()) {
        status_label.set_text("Введите название пресета");
        status_label.remove_css_class("success");
        status_label.add_css_class("error");
        return;
    }
    
    StatusPreset preset;
    preset.name = name;
    preset.state = state_entry.get_text();
    preset.details = details_entry.get_text();
    preset.large_image = large_image_entry.get_text();
    preset.large_text = large_text_entry.get_text();
    preset.small_image = small_image_entry.get_text();
    preset.small_text = small_text_entry.get_text();
    
    config.add_preset(preset);
    config.save_config();
    
    load_presets();
    preset_name_entry.set_text("");
    
    status_label.set_text("Пресет добавлен");
    status_label.remove_css_class("error");
    status_label.add_css_class("success");
}

void MainWindow::on_save_preset_clicked() {
    on_add_preset_clicked();
}

void MainWindow::on_preset_changed() {
    std::string selected = preset_combo.get_active_text();
    if (selected.empty()) return;
    
    StatusPreset preset = config.get_preset(selected);
    if (!preset.name.empty()) {
        state_entry.set_text(preset.state);
        details_entry.set_text(preset.details);
        large_image_entry.set_text(preset.large_image);
        large_text_entry.set_text(preset.large_text);
        small_image_entry.set_text(preset.small_image);
        small_text_entry.set_text(preset.small_text);
        
        status_label.set_text("Загружен пресет: " + preset.name);
        status_label.remove_css_class("error");
        status_label.add_css_class("success");
    }
}

void MainWindow::on_clear_status_clicked() {
    discord.clear_status();
    
    status_label.set_text("Статус Discord очищен");
    status_label.remove_css_class("error");
    status_label.add_css_class("success");
}

bool MainWindow::on_close_window() {
    // очищаем статус Discord перед закрытием
    discord.clear_status();
    discord.disconnect();
    
    // закрываем окно
    hide();
    return false; // позволяем продолжить закрытие
}

// класс приложения для управления окнами
class DiscordApp : public Gtk::Application {
private:
    ConnectWindow* connect_window = nullptr;
    MainWindow* main_window = nullptr;
    
protected:
    DiscordApp() : Gtk::Application("org.storm.discordrpc") {}

public:
    static Glib::RefPtr<DiscordApp> create() {
        return Glib::RefPtr<DiscordApp>(new DiscordApp());
    }

    // главный метод запуска
    void on_activate() override {
        // создаем окно подключения
        connect_window = Gtk::make_managed<ConnectWindow>();
        
        // подключаем сигнал успешного подключения
        connect_window->signal_connected_.connect(sigc::mem_fun(*this, &DiscordApp::on_connected));
        
        // обработчик закрытия окна подключения
        connect_window->signal_hide().connect([this]() {
            if (connect_window && !connect_window->is_connected()) {
                // пользователь закрыл окно без подключения
                quit();
            }
        });
        
        add_window(*connect_window);
        connect_window->show();
    }

private:
    // обработчик успешного подключения
    void on_connected() {
        if (!connect_window) return;
        
        std::string client_id = connect_window->get_client_id();
        
        // удаляем окно подключения
        remove_window(*connect_window);
        connect_window = nullptr;
        
        // создаем главное окно
        main_window = Gtk::make_managed<MainWindow>(client_id);
        
        // обработчик закрытия главного окна
        main_window->signal_hide().connect([this]() {
            // возвращаемся к начальному окну
            auto new_connect_window = Gtk::make_managed<ConnectWindow>();
            new_connect_window->signal_connected_.connect(sigc::mem_fun(*this, &DiscordApp::on_connected));
            new_connect_window->signal_hide().connect([this, new_connect_window]() {
                if (!new_connect_window->is_connected()) {
                    quit();
                }
            });
            
            add_window(*new_connect_window);
            new_connect_window->show();
        });
        
        add_window(*main_window);
        main_window->show();
    }
};

int main(int argc, char* argv[]) {
    auto app = DiscordApp::create();
    return app->run(argc, argv);
}
