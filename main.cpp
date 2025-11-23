#include <iostream>
#include <chrono>
#include "NBodySim.h"
#include <omp.h>

// Parámetros Globales (Hardcodeados para la Semana 1) [cite: 50]
const int N_BODIES = 1000;      // N: Número de cuerpos
const int STEPS = 100;          // Steps: Número de pasos de tiempo
const double DELTA_T = 1e-3;    // Δt: Tamaño del paso de tiempo
const double G_CONST = 6.674e-11; // Constante de Gravitación (G)

int main() {
    std::cout << "[N-Body Simulation: N=" << N_BODIES << ", Steps=" << STEPS << "]\n";

    // 1. Inicializar la simulación
    NBodySim sim(N_BODIES, DELTA_T, G_CONST);
    sim.initialize_bodies(); // Generar condiciones iniciales [cite: 50]

    // 2. Medir el tiempo de ejecución secuencial [cite: 17]
    auto start = std::chrono::high_resolution_clock::now();

    sim.update_sequential(STEPS); // Correr la simulación secuencial

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // 3. Reportar el tiempo de ejecución (Ejemplo de salida) [cite: 55]
    std::cout << "Tiempo Secuencial (1 thread): " << duration.count() << " segundos.\n";

    // 4. Guardar el estado final (Requisito de la Semana 1) [cite: 65, 54]
    sim.save_final_state("final_state_sequential.csv");
    std::cout << "Estado final guardado en: final_state_sequential.csv\n";


    // Prueba paralela
    omp_set_num_threads(4);

    std::cout << "\nEjecutando en paralelo con "
          << omp_get_max_threads() << " threads...\n";

    NBodySim sim2(N_BODIES, DELTA_T, G_CONST);
    sim2.initialize_bodies();

    auto start2 = std::chrono::high_resolution_clock::now();
    sim2.update_parallel_omp(STEPS);
    auto end2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration2 = end2 - start2;

    std::cout << "Tiempo Paralelo: " << duration2.count() << " segundos.\n";

    sim2.save_final_state("final_state_parallel.csv");
    std::cout << "Estado final paralelo guardado en: final_state_parallel.csv\n";

    //Estoy usando VSCode: g++ main.cpp src/NBodySim.cpp src/Body.cpp src/Vector3.cpp -I include -o simnbody.exe -std=c++17 -fopenmp
    // ./simnbody.exe

    return 0;
}