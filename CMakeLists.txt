cmake_minimum_required(VERSION 3.10)
project(NBodyProject)

# Usar C++17
set(CMAKE_CXX_STANDARD 17)

# Incluir carpeta de headers
include_directories(include)

# Archivos fuente
add_executable(NBodyProject
        main.cpp
        src/Vector3.cpp
        src/Body.cpp
        src/NBodySim.cpp
)
