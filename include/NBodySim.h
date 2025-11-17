#ifndef NBODYSIM_H
#define NBODYSIM_H

#include <vector>
#include <chrono>
#include "Body.h"
#include <string>
class NBodySim {
public:
    NBodySim(int N, double dt, double G = 6.674e-11);

    // Métodos para la simulación
    void update_sequential(int steps); // El foco de la Semana 1

    // Inicialización de cuerpos (Hardcodeado o aleatorio, según el requisito de Input)
    void initialize_bodies();

    // Guardar el estado final para verificación [cite: 54]
    void save_final_state(const std::string& filename);

private:
    std::vector<Body> bodies;
    const int N_bodies;
    const double delta_t; // Paso de tiempo (Δt) [cite: 50]
    const double G_const; // Constante de Gravitación [cite: 22]

    // Función que calcula la fuerza neta O(N^2) sobre todos los cuerpos
    void calculate_forces();

    // Función que actualiza posición y velocidad (Integración de Euler)
    void integrate();
};

#endif // NBODYSIM_H