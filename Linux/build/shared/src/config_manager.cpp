#include "config_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

ConfigManager::ConfigManager() {
    config_file_path = get_config_dir() + "/storm_discord_rpc.json";
    load_config();
}

ConfigManager::~ConfigManager() {
    save_config();
}

std::string ConfigManager::get_config_dir() {
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw ? pw->pw_dir : "/tmp";
    }
    
    std::string config_dir = std::string(home) + "/.config/storm-discord-rpc";
    
    // создаем директорию если не существует
    struct stat st = {0};
    if (stat(config_dir.c_str(), &st) == -1) {
        mkdir(config_dir.c_str(), 0755);
    }
    
    return config_dir;
}

bool ConfigManager::load_config() {
    std::ifstream file(config_file_path);
    if (!file.is_open()) {
        std::cout << "Config file not found, creating new one" << std::endl;
        return false;
    }
    
    try {
        std::string content((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
        file.close();
        
        // простой парсер JSON (без внешних библиотек)
        size_t pos = 0;
        
        // парсим основные настройки
        if (content.find("\"settings\":") != std::string::npos) {
            size_t settings_start = content.find("\"settings\":") + 11;
            size_t settings_end = content.find("}", settings_start);
            
            if (settings_end != std::string::npos) {
                std::string settings_str = content.substr(settings_start, settings_end - settings_start + 1);
                
                // извлекаем пары ключ-значение
                size_t key_start = 0;
                while ((key_start = settings_str.find("\"", key_start)) != std::string::npos) {
                    size_t key_end = settings_str.find("\"", key_start + 1);
                    if (key_end == std::string::npos) break;
                    
                    std::string key = settings_str.substr(key_start + 1, key_end - key_start - 1);
                    
                    size_t value_start = settings_str.find("\"", key_end + 1);
                    size_t value_end = settings_str.find("\"", value_start + 1);
                    
                    if (value_end != std::string::npos) {
                        std::string value = settings_str.substr(value_start + 1, value_end - value_start - 1);
                        settings[key] = unescape_json_string(value);
                        
                        key_start = value_end + 1;
                    } else {
                        break;
                    }
                }
            }
        }
        
        // парсим пресеты
        if (content.find("\"presets\":") != std::string::npos) {
            size_t presets_start = content.find("\"presets\":[") + 11;
            size_t presets_end = content.find("]", presets_start);
            
            if (presets_end != std::string::npos) {
                std::string presets_str = content.substr(presets_start, presets_end - presets_start);
                
                // парсим каждый пресет
                size_t preset_start = 0;
                while ((preset_start = presets_str.find("{", preset_start)) != std::string::npos) {
                    size_t preset_end = presets_str.find("}", preset_start);
                    if (preset_end == std::string::npos) break;
                    
                    std::string preset_str = presets_str.substr(preset_start, preset_end - preset_start + 1);
                    
                    StatusPreset preset;
                    
                    // извлекаем поля пресета
                    size_t pos = 0;
                    while ((pos = preset_str.find("\"", pos)) != std::string::npos) {
                        size_t field_end = preset_str.find("\"", pos + 1);
                        if (field_end == std::string::npos) break;
                        
                        std::string field = preset_str.substr(pos + 1, field_end - pos - 1);
                        
                        size_t value_start = preset_str.find("\"", field_end + 1);
                        size_t value_end = preset_str.find("\"", value_start + 1);
                        
                        if (value_end != std::string::npos) {
                            std::string value = preset_str.substr(value_start + 1, value_end - value_start - 1);
                            value = unescape_json_string(value);
                            
                            if (field == "name") preset.name = value;
                            else if (field == "state") preset.state = value;
                            else if (field == "details") preset.details = value;
                            else if (field == "large_image") preset.large_image = value;
                            else if (field == "large_text") preset.large_text = value;
                            else if (field == "small_image") preset.small_image = value;
                            else if (field == "small_text") preset.small_text = value;
                            
                            pos = value_end + 1;
                        } else {
                            break;
                        }
                    }
                    
                    if (!preset.name.empty()) {
                        presets.push_back(preset);
                    }
                    
                    preset_start = preset_end + 1;
                }
            }
        }
        
        std::cout << "Config loaded successfully" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Error loading config: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::save_config() {
    try {
        std::ostringstream json;
        json << "{\n";
        json << "  \"settings\": {\n";
        
        bool first = true;
        for (const auto& pair : settings) {
            if (!first) json << ",\n";
            json << "    \"" << pair.first << "\": \"" << escape_json_string(pair.second) << "\"";
            first = false;
        }
        
        json << "\n  },\n";
        json << "  \"presets\": [\n";
        
        for (size_t i = 0; i < presets.size(); ++i) {
            const auto& preset = presets[i];
            json << "    {\n";
            json << "      \"name\": \"" << escape_json_string(preset.name) << "\",\n";
            json << "      \"state\": \"" << escape_json_string(preset.state) << "\",\n";
            json << "      \"details\": \"" << escape_json_string(preset.details) << "\",\n";
            json << "      \"large_image\": \"" << escape_json_string(preset.large_image) << "\",\n";
            json << "      \"large_text\": \"" << escape_json_string(preset.large_text) << "\",\n";
            json << "      \"small_image\": \"" << escape_json_string(preset.small_image) << "\",\n";
            json << "      \"small_text\": \"" << escape_json_string(preset.small_text) << "\"\n";
            json << "    }";
            if (i < presets.size() - 1) json << ",";
            json << "\n";
        }
        
        json << "  ]\n";
        json << "}\n";
        
        std::ofstream file(config_file_path);
        if (!file.is_open()) {
            std::cout << "Cannot create config file" << std::endl;
            return false;
        }
        
        file << json.str();
        file.close();
        
        std::cout << "Config saved successfully" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Error saving config: " << e.what() << std::endl;
        return false;
    }
}

std::string ConfigManager::escape_json_string(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string ConfigManager::unescape_json_string(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case '"': result += '"'; ++i; break;
                case '\\': result += '\\'; ++i; break;
                case 'b': result += '\b'; ++i; break;
                case 'f': result += '\f'; ++i; break;
                case 'n': result += '\n'; ++i; break;
                case 'r': result += '\r'; ++i; break;
                case 't': result += '\t'; ++i; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

void ConfigManager::set_setting(const std::string& key, const std::string& value) {
    settings[key] = value;
}

std::string ConfigManager::get_setting(const std::string& key, const std::string& default_value) {
    auto it = settings.find(key);
    return it != settings.end() ? it->second : default_value;
}

void ConfigManager::set_client_id(const std::string& client_id) {
    set_setting("client_id", client_id);
}

std::string ConfigManager::get_client_id() {
    return get_setting("client_id", "");
}

void ConfigManager::set_last_status(const std::string& state, const std::string& details,
                                   const std::string& large_image, const std::string& large_text,
                                   const std::string& small_image, const std::string& small_text) {
    set_setting("last_state", state);
    set_setting("last_details", details);
    set_setting("last_large_image", large_image);
    set_setting("last_large_text", large_text);
    set_setting("last_small_image", small_image);
    set_setting("last_small_text", small_text);
}

void ConfigManager::get_last_status(std::string& state, std::string& details,
                                   std::string& large_image, std::string& large_text,
                                   std::string& small_image, std::string& small_text) {
    state = get_setting("last_state", "");
    details = get_setting("last_details", "");
    large_image = get_setting("last_large_image", "");
    large_text = get_setting("last_large_text", "");
    small_image = get_setting("last_small_image", "");
    small_text = get_setting("last_small_text", "");
}

void ConfigManager::add_preset(const StatusPreset& preset) {
    // проверяем нет ли уже пресета с таким именем
    for (auto& existing_preset : presets) {
        if (existing_preset.name == preset.name) {
            existing_preset = preset;
            return;
        }
    }
    presets.push_back(preset);
}

void ConfigManager::remove_preset(const std::string& name) {
    presets.erase(std::remove_if(presets.begin(), presets.end(),
        [&name](const StatusPreset& preset) {
            return preset.name == name;
        }), presets.end());
}

std::vector<StatusPreset> ConfigManager::get_presets() {
    return presets;
}

StatusPreset ConfigManager::get_preset(const std::string& name) {
    for (const auto& preset : presets) {
        if (preset.name == name) {
            return preset;
        }
    }
    return StatusPreset{};
}

void ConfigManager::set_auto_disconnect_time(int minutes) {
    set_setting("auto_disconnect_time", std::to_string(minutes));
}

int ConfigManager::get_auto_disconnect_time() {
    std::string value = get_setting("auto_disconnect_time", "0");
    return std::stoi(value);
}

bool ConfigManager::is_auto_disconnect_enabled() {
    return get_setting("auto_disconnect_enabled", "false") == "true";
}

void ConfigManager::set_window_geometry(int x, int y, int width, int height) {
    set_setting("window_x", std::to_string(x));
    set_setting("window_y", std::to_string(y));
    set_setting("window_width", std::to_string(width));
    set_setting("window_height", std::to_string(height));
}

void ConfigManager::get_window_geometry(int& x, int& y, int& width, int& height) {
    x = std::stoi(get_setting("window_x", "100"));
    y = std::stoi(get_setting("window_y", "100"));
    width = std::stoi(get_setting("window_width", "800"));
    height = std::stoi(get_setting("window_height", "600"));
}

void ConfigManager::set_theme(const std::string& theme) {
    set_setting("theme", theme);
}

std::string ConfigManager::get_theme() {
    return get_setting("theme", "default");
}
