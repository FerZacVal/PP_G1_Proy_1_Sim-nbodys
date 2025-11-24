cmake_minimum_required(VERSION 3.10)
project(NBodyProject)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# Flags globales de compilación y enlace
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")

add_executable(NBodyProject
        main.cpp
        src/Vector3.cpp
        src/Body.cpp
        src/NBodySim.cpp
)

message(STATUS "========================================")
message(STATUS "Compilando con: ${CMAKE_CXX_FLAGS}")
message(STATUS "Enlazando con: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "========================================")
