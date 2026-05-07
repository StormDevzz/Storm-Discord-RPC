#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <map>
#include <vector>

struct StatusPreset {
    std::string name;
    std::string state;
    std::string details;
    std::string large_image;
    std::string large_text;
    std::string small_image;
    std::string small_text;
};

class ConfigManager {
private:
    std::string config_file_path;
    std::map<std::string, std::string> settings;
    std::vector<StatusPreset> presets;
    
    std::string get_config_dir();
    std::string escape_json_string(const std::string& str);
    std::string unescape_json_string(const std::string& str);
    
public:
    ConfigManager();
    ~ConfigManager();
    
    // загрузка и сохранение конфигурации
    bool load_config();
    bool save_config();
    
    // работа с настройками
    void set_setting(const std::string& key, const std::string& value);
    std::string get_setting(const std::string& key, const std::string& default_value = "");
    
    // удобные методы для основных настроек
    void set_client_id(const std::string& client_id);
    std::string get_client_id();
    
    void set_last_status(const std::string& state, const std::string& details,
                        const std::string& large_image, const std::string& large_text,
                        const std::string& small_image, const std::string& small_text);
    void get_last_status(std::string& state, std::string& details,
                        std::string& large_image, std::string& large_text,
                        std::string& small_image, std::string& small_text);
    
    // работа с пресетами
    void add_preset(const StatusPreset& preset);
    void remove_preset(const std::string& name);
    std::vector<StatusPreset> get_presets();
    StatusPreset get_preset(const std::string& name);
    
    // настройки таймера
    void set_auto_disconnect_time(int minutes);
    int get_auto_disconnect_time();
    bool is_auto_disconnect_enabled();
    
    // прочие настройки
    void set_window_geometry(int x, int y, int width, int height);
    void get_window_geometry(int& x, int& y, int& width, int& height);
    
    void set_theme(const std::string& theme);
    std::string get_theme();
};

#endif
