#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <map>
#include <mutex>

std::atomic<int> timeout_ms{ 2000 };
std::string phrase = "Hello from nya1!";
std::mutex phrase_mutex;

void print_loop()
{
    while (true)
    {
        std::lock_guard<std::mutex> lock(phrase_mutex);
        std::cout << phrase << "\n";
        
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
    }
}

int main()
{
    auto connection = sdbus::createSessionBusConnection();
    std::string object_path = "/com/system/configurationManager/Application/confManagerApplication1";
    std::string interface = "com.system.configurationManager.Application.Configuration";
    auto proxy = sdbus::createProxy(*connection, "com.system.configurationManager", object_path);
    
    std::map<std::string, sdbus::Variant> config;
    proxy->callMethod("GetConfiguration")
	.onInterface(interface)
	.storeResultsTo(config);
    
    if (config.count("Timeout"))
    {
        timeout_ms = config["Timeout"].get<int32_t>();
    }
    if (config.count("TimeoutPhrase"))
    {
        phrase = config["TimeoutPhrase"].get<std::string>();
    }
    
    proxy->uponSignal("configurationChanged")
        .onInterface(interface)
        .call([](const std::map<std::string, sdbus::Variant>& new_config) {
        if (new_config.count("Timeout")) 
        {
            timeout_ms = new_config.at("Timeout").get<int32_t>();
        }
        if (new_config.count("TimeoutPhrase")) 
        {
            std::lock_guard<std::mutex> lock(phrase_mutex);
            phrase = new_config.at("TimeoutPhrase").get<std::string>();
        }
            });
    proxy->finishRegistration();
    
    std::thread printer(print_loop);
    
    connection->enterEventLoop();
    
    printer.join();
}
