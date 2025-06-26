sudo apt install libsdbus-c++-dev nlohmann-json3-dev dbus
git clone https://github.com/FuhFuhich/sdbus.git
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
mkdir -p ~/com.system.configurationManager
echo '{"Timeout":2000,"TimeoutPhrase":"Initial phrase"}' > ~/com.system.configurationManager/confManagerApplication1.json
# Запуск сервиса
# Запуск клиента во втором окне
# В окне с сервисом вписать команду <gdbus send -e -d com.system.configurationManager -o com.system.configurationManager.Application.confManagerApplication1 -m com.system.configurationManager.Application.ChangeConfiguration "TimeoutPhrase" "s Please stop me"> (пока что не работает)
