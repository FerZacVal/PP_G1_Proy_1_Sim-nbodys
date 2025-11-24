#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <vector>
#include <omp.h>
#include "NBodySim.h"

// Parámetros Globales
const int N_BODIES = 1000;
const int STEPS = 100;
const double DELTA_T = 1e-3;
const double G_CONST = 6.674e-11;

// Estructura para almacenar resultados
struct BenchmarkResult {
    int threads;
    double time_seconds;
    double speedup;
    double efficiency;
};

// Función para imprimir una línea separadora
void print_separator() {
    std::cout << "================================================================\n";
}

// Función para imprimir encabezado
void print_header() {
    print_separator();
    std::cout << "           N-BODY SIMULATION - BENCHMARK COMPLETO              \n";
    print_separator();
    std::cout << "Configuracion:\n";
    std::cout << "  - Numero de cuerpos (N): " << N_BODIES << "\n";
    std::cout << "  - Pasos de tiempo:       " << STEPS << "\n";
    std::cout << "  - Delta t:               " << DELTA_T << "\n";
    std::cout << "  - Constante G:           " << G_CONST << "\n";
    std::cout << "  - Threads disponibles:   " << omp_get_max_threads() << "\n";
    print_separator();
    std::cout << "\n";
}

// Función para imprimir tabla de resultados
void print_results_table(const std::vector<BenchmarkResult>& results) {
    std::cout << "\n";
    print_separator();
    std::cout << "                   TABLA DE RESULTADOS                         \n";
    print_separator();
    std::cout << std::fixed << std::setprecision(4);
    std::cout << std::setw(12) << "Threads"
              << std::setw(16) << "Tiempo (s)"
              << std::setw(16) << "Speedup"
              << std::setw(16) << "Eficiencia\n";
    std::cout << "----------------------------------------------------------------\n";

    for (const auto& result : results) {
        std::cout << std::setw(12) << result.threads
                  << std::setw(16) << result.time_seconds
                  << std::setw(16) << result.speedup << "x"
                  << std::setw(15) << (result.efficiency * 100) << "%\n";
    }

    print_separator();
}

// Función para guardar resultados en CSV
void save_results_csv(const std::vector<BenchmarkResult>& results) {
    std::ofstream file("performance_results.csv");

    if (!file.is_open()) {
        std::cerr << "Error: No se pudo crear performance_results.csv\n";
        return;
    }

    // Encabezado
    file << "Threads,Tiempo_Segundos,Speedup,Eficiencia_Porcentaje\n";

    // Datos
    file << std::fixed << std::setprecision(6);
    for (const auto& result : results) {
        file << result.threads << ","
             << result.time_seconds << ","
             << result.speedup << ","
             << (result.efficiency * 100) << "\n";
    }

    file.close();
    std::cout << "\n Resultados guardados en: performance_results.csv\n";
}

// Función para generar tabla en formato Markdown
void save_markdown_table(const std::vector<BenchmarkResult>& results) {
    std::ofstream file("tabla_resultados.md");

    if (!file.is_open()) {
        std::cerr << "Error: No se pudo crear tabla_resultados.md\n";
        return;
    }

    file << "## Tabla de Resultados - N-Body Simulation\n\n";
    file << "| Threads | Tiempo (s) | Speedup | Eficiencia |\n";
    file << "|---------|------------|---------|------------|\n";

    file << std::fixed << std::setprecision(4);
    for (const auto& result : results) {
        file << "| " << result.threads
             << " | " << result.time_seconds
             << " | " << result.speedup << "x"
             << " | " << (result.efficiency * 100) << "% |\n";
    }

    file << "\n**Configuracion:** N=" << N_BODIES
         << ", Steps=" << STEPS
         << ", Δt=" << DELTA_T << "\n";

    file.close();
    std::cout << " Tabla Markdown guardada en: tabla_resultados.md\n";
}

// Función para mostrar análisis de resultados
void print_analysis(const std::vector<BenchmarkResult>& results) {
    std::cout << "\n";
    print_separator();
    std::cout << "                      ANALISIS DE RESULTADOS                   \n";
    print_separator();

    // Encontrar mejor speedup
    double best_speedup = 0;
    int best_threads = 1;
    for (const auto& result : results) {
        if (result.speedup > best_speedup) {
            best_speedup = result.speedup;
            best_threads = result.threads;
        }
    }

    std::cout << "\n Mejor rendimiento:\n";
    std::cout << "   - Speedup maximo: " << std::fixed << std::setprecision(2)
              << best_speedup << "x\n";
    std::cout << "   - Logrado con: " << best_threads << " threads\n";

    // Calcular eficiencia promedio (excluyendo secuencial)
    double total_efficiency = 0;
    int count = 0;
    for (const auto& result : results) {
        if (result.threads > 1) {
            total_efficiency += result.efficiency;
            count++;
        }
    }

    if (count > 0) {
        double avg_efficiency = total_efficiency / count;
        std::cout << "\n Eficiencia promedio: "
                  << (avg_efficiency * 100) << "%\n";
    }

    // Escalabilidad
    std::cout << "\n Observaciones:\n";
    if (best_speedup >= best_threads * 0.8) {
        std::cout << "    Buena escalabilidad (>80% del ideal)\n";
    } else if (best_speedup >= best_threads * 0.6) {
        std::cout << "   ️  Escalabilidad moderada (60-80% del ideal)\n";
    } else {
        std::cout << "    Escalabilidad limitada (<60% del ideal)\n";
    }

    // Ley de Amdahl
    if (results.size() >= 2) {
        double last_efficiency = results.back().efficiency;
        if (last_efficiency < 0.7) {
            std::cout << "    La eficiencia disminuye con más threads\n";
            std::cout << "      (efecto de la Ley de Amdahl y overhead)\n";
        }
    }

    print_separator();
}

int main() {
    // Mostrar encabezado
    print_header();

    // Vector para almacenar resultados
    std::vector<BenchmarkResult> results;

    // ================================================================
    // PASO 1: SIMULACIÓN SECUENCIAL (BASELINE)
    // ================================================================
    std::cout << " PASO 1: Ejecutando version SECUENCIAL (baseline)...\n";

    NBodySim sim_seq(N_BODIES, DELTA_T, G_CONST);
    sim_seq.initialize_bodies();

    auto start_seq = std::chrono::high_resolution_clock::now();
    sim_seq.update_sequential(STEPS);
    auto end_seq = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration_seq = end_seq - start_seq;
    double time_sequential = duration_seq.count();

    std::cout << "    Tiempo secuencial: " << time_sequential << " segundos\n";
    sim_seq.save_final_state("final_state_sequential.csv");
    std::cout << "    Estado guardado en: final_state_sequential.csv\n\n";

    // Agregar resultado secuencial
    results.push_back({1, time_sequential, 1.0, 1.0});

    // ================================================================
    // PASO 2: SIMULACIONES PARALELAS
    // ================================================================
    std::cout << " PASO 2: Ejecutando versiones PARALELAS...\n\n";

    // Array de números de threads a probar
    std::vector<int> thread_counts = {2, 4, 8};

    int max_threads = omp_get_max_threads();

    for (int num_threads : thread_counts) {
        // Verificar si el número de threads es válido
        if (num_threads > max_threads) {
            std::cout << "️  Saltando " << num_threads
                      << " threads (excede maximo: " << max_threads << ")\n\n";
            continue;
        }

        std::cout << "   Probando con " << num_threads << " threads...\n";

        // Configurar OpenMP
        omp_set_num_threads(num_threads);

        // Crear nueva simulación
        NBodySim sim_par(N_BODIES, DELTA_T, G_CONST);
        sim_par.initialize_bodies();

        // Medir tiempo
        auto start_par = std::chrono::high_resolution_clock::now();
        sim_par.update_parallel_omp(STEPS);
        auto end_par = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> duration_par = end_par - start_par;
        double time_parallel = duration_par.count();

        // Calcular métricas
        double speedup = time_sequential / time_parallel;
        double efficiency = speedup / num_threads;

        std::cout << "      Tiempo:     " << time_parallel << " segundos\n";
        std::cout << "      Speedup:    " << std::fixed << std::setprecision(2)
                  << speedup << "x\n";
        std::cout << "      Eficiencia: " << (efficiency * 100) << "%\n\n";

        // Guardar estado (solo para el último)
        if (num_threads == thread_counts.back() || num_threads == max_threads) {
            sim_par.save_final_state("final_state_parallel.csv");
            std::cout << "       Estado paralelo guardado en: final_state_parallel.csv\n\n";
        }

        // Agregar resultado
        results.push_back({num_threads, time_parallel, speedup, efficiency});
    }

    // ================================================================
    // PASO 3: MOSTRAR RESULTADOS
    // ================================================================
    print_results_table(results);

    // ================================================================
    // PASO 4: ANÁLISIS
    // ================================================================
    print_analysis(results);

    // ================================================================
    // PASO 5: GUARDAR RESULTADOS
    // ================================================================
    save_results_csv(results);
    save_markdown_table(results);

    std::cout << "\n";
    print_separator();
    std::cout << "                  BENCHMARK COMPLETADO                         \n";
    print_separator();
    std::cout << "\n";
    std::cout << " Archivos generados:\n";
    std::cout << "   - final_state_sequential.csv\n";
    std::cout << "   - final_state_parallel.csv\n";
    std::cout << "   - performance_results.csv\n";
    std::cout << "   - tabla_resultados.md\n";
    std::cout << "\n";

    return 0;
}
