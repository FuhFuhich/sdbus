## Установка и создание файлов
``` console
sudo apt install libsdbus-c++-dev nlohmann-json3-dev dbus
```
## Клонирование репозитория
``` console
git clone https://github.com/FuhFuhich/sdbus.git
```
## Переход в файл
``` console
cd sdbus
```
## Создание папки билда
``` console
mkdir build
```
## Переход в билд
``` console
cd build
```
## Генерация проекта с помощью CMake
``` console
cmake -DCMAKE_BUILD_TYPE=Release ..
```
## Сборка проекта
``` console
make
```
## Создание директории для конфигов
``` console
mkdir -p ~/com.system.configurationManager
```
## Создание тестового конфигурационного файла
```console
echo '{"Timeout":2000,"TimeoutPhrase":"Initial phrase"}' > ~/com.system.configurationManager/confManagerApplication1.json
```

## Запустите сервер в первом окне
``` console
cd service
./server
```
## Запустите клиент во втором окне
```console
cd ..
cd applications
cd confManagerApplication1
./client
```
## После откройте 3 окно и введите одну из команд. Первая команда на изменение фразы:
``` console
dbus-send --session   --dest=com.system.configurationManager   --type=method_call   --print-reply   /com/system/configurationManager/Application/confManagerApplication1   com.system.configurationManager.Application.Configuration.ChangeConfiguration   string:"TimeoutPhrase" variant:string:"(nya)"
```
## Вторая команда на изменение времени (в миллисекундах):
```console
dbus-send --session   --dest=com.system.configurationManager   --type=method_call   --print-reply   /com/system/configurationManager/Application/confManagerApplication1   com.system.configurationManager.Application.Configuration.ChangeConfiguration   string:"Timeout" variant:int32:1000
```
