#!/bin/bash

set -e

echo "Установка зависимостей..."
sudo apt install -y libsdbus-c++-dev nlohmann-json3-dev dbus

git clone https://github.com/FuhFuhich/sdbus.git

cd sdbus

mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release ..

make

mkdir -p ~/com.system.configurationManager

echo '{"Timeout":2000,"TimeoutPhrase":"Initial phrase"}' > ~/com.system.configurationManager/confManagerApplication1.json


