#include <sdbus-c++/sdbus-c++.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <variant>

class AppConfig {
private:
    std::string app_name_;
    std::map<std::string, sdbus::Variant> config_;
    std::string config_path_;
    std::unique_ptr<sdbus::IObject> object_;

public:
    AppConfig(sdbus::IConnection& connection, const std::string& app_name, std::map<std::string, sdbus::Variant> config, const std::string& config_path)
        : app_name_(app_name), config_(std::move(config)), config_path_(config_path)
    {
        std::string object_path = "/com/system/configurationManager/Application/" + app_name_;
        object_ = sdbus::createObject(connection, object_path);

        object_->registerMethod("ChangeConfiguration")
            .onInterface("com.system.configurationManager.Application.Configuration")
            .implementedAs([this](const std::string& key, const sdbus::Variant& value) {
            this->changeConfiguration(key, value);
                });
        
        object_->registerMethod("GetConfiguration")
            .onInterface("com.system.configurationManager.Application.Configuration")
            .implementedAs([this]() {
            return this->getConfiguration();
                });
        
        object_->registerSignal("configurationChanged")
            .onInterface("com.system.configurationManager.Application.Configuration")
            .withParameters<std::map<std::string, sdbus::Variant>>();
        
        object_->finishRegistration();
    }
    
    void changeConfiguration(const std::string& key, const sdbus::Variant& value)
    {
        if (config_.count(key) == 0)
        {
            throw sdbus::Error("com.system.configurationManager.Error", "Unknown configuration key");
        }
        config_[key] = value;
        saveConfigToFile();
        emitConfigurationChanged();
    }
    
    std::map<std::string, sdbus::Variant> getConfiguration()
    {
        return config_;
    }
    
    void emitConfigurationChanged()
    {
        object_->emitSignal("configurationChanged")
            .onInterface("com.system.configurationManager.Application.Configuration")
            .withArguments(config_);
    }

private:
    void saveConfigToFile()
    {
        nlohmann::json j;
        for (const auto& [key, val] : config_) 
        {
            try
            {
                j[key] = val.get<int32_t>();
            }
            catch (const sdbus::Error&)
            {
                j[key] = val.get<std::string>();
            }
        }
        std::ofstream f(config_path_);
        f << j.dump(4);
    }
};

std::map<std::string, sdbus::Variant> loadConfig(const std::string& filename)
{
    std::ifstream f(filename);
    
    if (!f) throw std::runtime_error("Config file not found: " + filename);
    
    nlohmann::json j;
    f >> j;
    
    std::map<std::string, sdbus::Variant> config;
    
    for (auto& [key, val] : j.items())
    {
        if (val.is_number_integer())
        {
            config[key] = sdbus::Variant(val.get<int32_t>());
        }
        else if (val.is_string())
        {
            config[key] = sdbus::Variant(val.get<std::string>());
        }
    }
    
    return config;
}

int main()
{
    auto connection = sdbus::createSessionBusConnection("com.system.configurationManager");

    std::string config_dir = std::string(getenv("HOME")) + "/com.system.configurationManager/";
    std::vector<std::unique_ptr<AppConfig>> app_objects;

    for (const auto& entry : std::filesystem::directory_iterator(config_dir))
    {
        if (entry.path().extension() == ".json")
        {
            std::string app_name = entry.path().stem().string();

            std::map<std::string, sdbus::Variant> config = loadConfig(entry.path().string());
            app_objects.emplace_back(std::make_unique<AppConfig>(*connection, app_name, config, entry.path().string()));
            std::cout << "Loaded config for: " << app_name << "\n";
        }
    }

    connection->enterEventLoop();
}
