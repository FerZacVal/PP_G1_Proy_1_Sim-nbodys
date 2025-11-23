cmake_minimum_required(VERSION 3.10)
project(NBodyProject)

set(CMAKE_CXX_STANDARD 17)

# Incluir carpeta de headers
include_directories(include)

# Buscar OpenMP
find_package(OpenMP REQUIRED)

# Archivos fuente
add_executable(NBodyProject
        main.cpp
        src/Vector3.cpp
        src/Body.cpp
        src/NBodySim.cpp
)

# Enlazar OpenMP
if(OpenMP_CXX_FOUND)
    target_link_libraries(NBodyProject PUBLIC OpenMP::OpenMP_CXX)
endif()
