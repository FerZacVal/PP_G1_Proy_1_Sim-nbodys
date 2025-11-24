#include <iostream>
#include <chrono>
#include <fstream>
#include "NBodySim.h"
#include <omp.h>

// ==========================================
// FUNCIÓN PARA VERIFICAR IGUALDAD DE CSVs
// ==========================================
bool compare_csv(const std::string& file1, const std::string& file2) {
    std::ifstream f1(file1), f2(file2);
    if (!f1 || !f2) {
        std::cerr << "Error: no se pudieron abrir los archivos.\n";
        return false;
    }

    std::string line1, line2;
    int line_num = 1;
    while (std::getline(f1, line1) && std::getline(f2, line2)) {
        if (line1 != line2) {
            std::cout << "Diferencia en linea " << line_num << "\n";
            std::cout << "Secuencial: " << line1 << "\n";
            std::cout << "Paralelo:   " << line2 << "\n";
            return false;
        }
        line_num++;
    }

    std::cout << "Los archivos CSV (SALIDA DE POSICIONES) son identicos.\n";
    return true;
}

// ==========================================
// PARÁMETROS DE LA SIMULACIÓN
// ==========================================
const int N_BODIES = 3000;
const int STEPS = 100;
const double DT = 1e-3;
const double G_CONST = 6.674e-11;

int main() {
    unsigned int seed = 12345;   // Permite reproducibilidad

    std::cout << "[N-Body Simulation: N=" << N_BODIES
              << ", Steps=" << STEPS << "]\n";

    // ========================
    // SIMULACIÓN SECUENCIAL
    // ========================
    NBodySim sim_seq(N_BODIES, DT, G_CONST);
    sim_seq.initialize_bodies(seed);

    auto start = std::chrono::high_resolution_clock::now();

    sim_seq.update_sequential(STEPS);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> seq_time = end - start;

    std::cout << "Tiempo Secuencial (1 thread): "
              << seq_time.count() << " segundos.\n";

    sim_seq.save_final_state("final_state_sequential.csv");
    std::cout << "Estado final guardado en: final_state_sequential.csv\n\n";


    // ========================
    // SIMULACIÓN PARALELA
    // ========================
    omp_set_num_threads(4);
    int threads = omp_get_max_threads();

    std::cout << "Ejecutando en paralelo con " << threads << " threads...\n";

    NBodySim sim_par(N_BODIES, DT, G_CONST);
    sim_par.initialize_bodies(seed);     // MISMO UNIVERSO

    auto start2 = std::chrono::high_resolution_clock::now();

    sim_par.update_parallel_omp(STEPS);

    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> par_time = end2 - start2;

    std::cout << "Tiempo Paralelo: "
              << par_time.count() << " segundos.\n";

    sim_par.save_final_state("final_state_parallel.csv");
    std::cout << "Estado final paralelo guardado en: final_state_parallel.csv\n";


    // ========================
    // PRUEBA DE IGUALDAD
    // ========================
    std::cout << "\n=== PRUEBA DE IGUALDAD ===\n";
    compare_csv("final_state_sequential.csv", "final_state_parallel.csv");

    return 0;
}
