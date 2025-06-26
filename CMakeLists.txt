cmake_minimum_required(VERSION 3.16)
project(configuration-manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(service)
add_subdirectory(applications/confManagerApplication1)
