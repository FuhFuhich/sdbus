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
        {
            std::lock_guard<std::mutex> lock(phrase_mutex);
            std::cout << phrase << "\n";
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
    }
}

int main()
{
    auto connection = sdbus::createSessionBusConnection();
    std::string objectPath = "/com/system/configurationManager/Application/confManagerApplication1";
    std::string interface = "com.system.configurationManager.Application.Configuration";
    auto proxy = sdbus::createProxy(*connection, "com.system.configurationManager", objectPath);
    
    std::map<std::string, sdbus::Variant> config;
    proxy->callMethod("GetConfiguration").storeResultsTo(config);
    
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
        .call([](const std::map<std::string, sdbus::Variant>& newConfig) {
        if (newConfig.count("Timeout")) 
        {
            timeout_ms = newConfig.at("Timeout").get<int32_t>();
        }
        if (newConfig.count("TimeoutPhrase")) 
        {
            std::lock_guard<std::mutex> lock(phrase_mutex);
            phrase = newConfig.at("TimeoutPhrase").get<std::string>();
        }
            });
    proxy->finishRegistration();
    
    std::thread printer(print_loop);
    
    connection->enterEventLoop();
    
    printer.join();
}
