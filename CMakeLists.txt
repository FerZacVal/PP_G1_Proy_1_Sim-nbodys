cmake_minimum_required(VERSION 3.10)
project(NBodyProject)

set(CMAKE_CXX_STANDARD 17)

# Incluir headers
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

# Enlazar OpenMP correctamente (MinGW requiere ambos métodos)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found!")
    target_link_libraries(NBodyProject PUBLIC OpenMP::OpenMP_CXX)

    # Necesario para MinGW (muy importante)
    target_compile_options(NBodyProject PRIVATE -fopenmp)
    target_link_options(NBodyProject PRIVATE -fopenmp)
endif()
