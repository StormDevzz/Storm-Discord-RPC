#include <gtkmm.h>
#include <iostream>
#include <string>
#include "discord_rpc.h"

class DiscordWindow : public Gtk::Window {
private:
    // виджеты
    Gtk::Box main_box;
    Gtk::Box input_box;
    Gtk::Label title_label;
    Gtk::Entry client_id_entry;
    Gtk::Button connect_button;
    Gtk::Button disconnect_button;
    Gtk::Separator separator;
    Gtk::Box status_box;
    Gtk::Label status_label;
    Gtk::Entry state_entry;
    Gtk::Entry details_entry;
    Gtk::Entry large_image_entry;
    Gtk::Entry large_text_entry;
    Gtk::Entry small_image_entry;
    Gtk::Entry small_text_entry;
    Gtk::Button update_button;
    Gtk::Label connection_status;
    
    // Discord RPC
    DiscordRPC discord;
    bool connected;
    
    // обработчики событий
    void on_connect_clicked();
    void on_disconnect_clicked();
    void on_update_clicked();
    bool update_connection_status();
    
public:
    DiscordWindow();
    virtual ~DiscordWindow();
};

DiscordWindow::DiscordWindow() 
    : main_box(Gtk::Orientation::VERTICAL, 10),
      input_box(Gtk::Orientation::VERTICAL, 5),
      status_box(Gtk::Orientation::VERTICAL, 5),
      connected(false) {
    
    // настройка окна
    set_title("Storm Discord RPC");
    set_default_size(400, 500);
    set_margin(10);
    
    // добавляем виджеты
    title_label.set_text("Discord Rich Presence");
    title_label.set_margin_bottom(10);
    
    // поля для подключения
    client_id_entry.set_placeholder_text("Введите Client ID");
    connect_button.set_label("Подключиться");
    disconnect_button.set_label("Отключиться");
    disconnect_button.set_sensitive(false);
    
    // поля для статуса
    status_label.set_text("Настройки статуса:");
    state_entry.set_placeholder_text("Состояние (например: Играю в игру)");
    details_entry.set_placeholder_text("Детали (например: Уровень 5)");
    large_image_entry.set_placeholder_text("Ключ большого изображения");
    large_text_entry.set_placeholder_text("Текст большого изображения");
    small_image_entry.set_placeholder_text("Ключ маленького изображения");
    small_text_entry.set_placeholder_text("Текст маленького изображения");
    update_button.set_label("Обновить статус");
    update_button.set_sensitive(false);
    
    connection_status.set_text("Статус: не подключен");
    connection_status.set_margin_top(10);
    
    // упаковываем виджеты
    main_box.append(title_label);
    main_box.append(input_box);
    input_box.append(client_id_entry);
    input_box.append(connect_button);
    input_box.append(disconnect_button);
    main_box.append(separator);
    main_box.append(status_box);
    status_box.append(status_label);
    status_box.append(state_entry);
    status_box.append(details_entry);
    status_box.append(large_image_entry);
    status_box.append(large_text_entry);
    status_box.append(small_image_entry);
    status_box.append(small_text_entry);
    status_box.append(update_button);
    main_box.append(connection_status);
    
    // подключаем обработчики
    connect_button.signal_clicked().connect(sigc::mem_fun(*this, &DiscordWindow::on_connect_clicked));
    disconnect_button.signal_clicked().connect(sigc::mem_fun(*this, &DiscordWindow::on_disconnect_clicked));
    update_button.signal_clicked().connect(sigc::mem_fun(*this, &DiscordWindow::on_update_clicked));
    
    set_child(main_box);
    
    // таймер для обработки событий Discord
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &DiscordWindow::update_connection_status), 1000, Glib::PRIORITY_DEFAULT);
}

DiscordWindow::~DiscordWindow() {
    discord.disconnect();
}

void DiscordWindow::on_connect_clicked() {
    std::string client_id = client_id_entry.get_text();
    
    if (client_id.empty()) {
        Gtk::MessageDialog dialog(*this, "Ошибка: введите Client ID", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK);
        dialog.show();
        dialog.hide();
        return;
    }
    
    if (discord.initialize(client_id)) {
        connected = true;
        connect_button.set_sensitive(false);
        disconnect_button.set_sensitive(true);
        update_button.set_sensitive(true);
        client_id_entry.set_sensitive(false);
        connection_status.set_text("Статус: подключено");
    }
}

void DiscordWindow::on_disconnect_clicked() {
    discord.disconnect();
    connected = false;
    connect_button.set_sensitive(true);
    disconnect_button.set_sensitive(false);
    update_button.set_sensitive(false);
    client_id_entry.set_sensitive(true);
    connection_status.set_text("Статус: не подключен");
}

void DiscordWindow::on_update_clicked() {
    std::string state = state_entry.get_text();
    std::string details = details_entry.get_text();
    std::string large_image = large_image_entry.get_text();
    std::string large_text = large_text_entry.get_text();
    std::string small_image = small_image_entry.get_text();
    std::string small_text = small_text_entry.get_text();
    
    discord.updatePresence(state, details, large_image, large_text, small_image, small_text);
}

bool DiscordWindow::update_connection_status() {
    if (connected) {
        discord.processEvents();
    }
    return true; // продолжать таймер
}

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("org.storm.discordrpc");
    
    DiscordWindow window;
    
    return app->make_window_and_run<DiscordWindow>(argc, argv);
}
