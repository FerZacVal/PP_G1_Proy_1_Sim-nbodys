#include "NBodySim.h"
#include <iostream>
#include <fstream>
#include <random>
#include <omp.h>


NBodySim::NBodySim(int N, double dt, double G)
    : N_bodies(N), delta_t(dt), G_const(G) {
    bodies.reserve(N);
}

// Lógica O(N^2) para calcular la fuerza neta en cada cuerpo
void NBodySim::calculate_forces() {
    // 1. Resetear todas las fuerzas
    for (int i = 0; i < N_bodies; ++i) {
        bodies[i].reset_force();
    }

    // 2. Calcular las interacciones (N^2)
    for (int i = 0; i < N_bodies; ++i) {
        for (int j = i + 1; j < N_bodies; ++j) {
            // Vector de distancia (r_ij)
            Vector3 r_vec = bodies[j].position - bodies[i].position;
            double r_sq = r_vec.magnitude_sq();

            // Usamos un epsilon pequeño para evitar la división por cero si r=0
            const double EPSILON = 1e-9;
            if (r_sq < EPSILON) continue;

            double r = std::sqrt(r_sq);

            // Calcular la magnitud de la fuerza (F = G * m1 * m2 / r^2) [cite: 22]
            double F_mag = G_const * bodies[i].mass * bodies[j].mass / r_sq;

            // Vector de fuerza F_ij (es la magnitud * el vector unitario)
            Vector3 F_vec = r_vec * (F_mag / r); // (F_mag / r) es el factor de r_vec

            // Aplicar fuerzas:
            // F_i += F_vec  (fuerza que j ejerce sobre i)
            bodies[i].force = bodies[i].force + F_vec;
            // F_j -= F_vec  (fuerza que i ejerce sobre j, 3ra ley de Newton)
            bodies[j].force = bodies[j].force - F_vec;
        }
    }
}

// Integración de Euler (actualización de posición y velocidad)
void NBodySim::integrate() {
    for (int i = 0; i < N_bodies; ++i) {
        Body& b = bodies[i];

        // Aceleración: a_t = F_neta / m [cite: 27]
        Vector3 acceleration = b.force / b.mass;

        // Nueva Velocidad: v_t+1 = v_t + a_t * Δt [cite: 29]
        b.velocity = b.velocity + acceleration * delta_t;

        // Nueva Posición: x_t+1 = x_t + v_t+1 * Δt (usamos v_t+1 para una integración más estable, aunque el PDF sugiere v_t [cite: 30])
        b.position = b.position + b.velocity * delta_t;
    }
}

// Función principal de la simulación secuencial
void NBodySim::update_sequential(int steps) {
    for (int t = 0; t < steps; ++t) {
        calculate_forces();
        integrate();
    }
}

// Inicialización de cuerpos (Ejemplo de generación aleatoria)
void NBodySim::initialize_bodies() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist_pos(-10.0, 10.0);
    std::uniform_real_distribution<> dist_vel(-0.1, 0.1);
    std::uniform_real_distribution<> dist_mass(1.0, 100.0);

    for (int i = 0; i < N_bodies; ++i) {
        bodies.emplace_back(
            dist_mass(gen),
            Vector3(dist_pos(gen), dist_pos(gen), dist_pos(gen)),
            Vector3(dist_vel(gen), dist_vel(gen), dist_vel(gen))
        );
    }
}

// Guardar el estado final [cite: 54]
void NBodySim::save_final_state(const std::string& filename) {
    std::ofstream ofs(filename);
    if (!ofs) {
        std::cerr << "Error al abrir el archivo de salida: " << filename << std::endl;
        return;
    }
    ofs << "id,mass,x,y,z,vx,vy,vz\n";
    for (int i = 0; i < N_bodies; ++i) {
        const Body& b = bodies[i];
        ofs << i << "," << b.mass << ","
            << b.position.x << "," << b.position.y << "," << b.position.z << ","
            << b.velocity.x << "," << b.velocity.y << "," << b.velocity.z << "\n";
    }
    ofs.close();
}

// Función principal de la simulación paralela con OpenMP
void NBodySim::update_parallel_omp(int steps) {

    #pragma omp parallel
    {
        #pragma omp single
        std::cout << "[OMP] Hilos activos: " << omp_get_num_threads() << "\n";
    }

    for (int t = 0; t < steps; ++t) {

        // 1. Buffer local por hilo para acumulación de fuerzas
        std::vector<Vector3> force_buffer(N_bodies);

        #pragma omp parallel
        {
            // Cada hilo tiene su propio buffer privado
            std::vector<Vector3> local_force(N_bodies, Vector3(0,0,0));

            #pragma omp for schedule(dynamic)
            for (int i = 0; i < N_bodies; ++i) {
                for (int j = i + 1; j < N_bodies; ++j) {

                    Vector3 r_vec = bodies[j].position - bodies[i].position;
                    double r_sq = r_vec.magnitude_sq();
                    const double EPSILON = 1e-9;
                    if (r_sq < EPSILON) continue;

                    double r = std::sqrt(r_sq);
                    double F_mag = G_const * bodies[i].mass * bodies[j].mass / r_sq;
                    Vector3 F_vec = r_vec * (F_mag / r);

                    // Cada hilo acumula sus fuerzas en su propio buffer
                    local_force[i] = local_force[i] + F_vec;
                    local_force[j] = local_force[j] - F_vec;
                }
            }

            // Reducción paralela: combinar fuerzas locales hacia el buffer global
            #pragma omp critical
            {
                for (int i = 0; i < N_bodies; ++i) {
                    force_buffer[i] = force_buffer[i] + local_force[i];
                }
            }
        }

        // Copiar las fuerzas acumuladas al arreglo principal
        for (int i = 0; i < N_bodies; ++i) {
            bodies[i].force = force_buffer[i];
        }

        // 3. Integración paralela
        #pragma omp parallel for
        for (int i = 0; i < N_bodies; ++i) {

            Body& b = bodies[i];
            Vector3 acceleration = b.force / b.mass;

            b.velocity = b.velocity + acceleration * delta_t;
            b.position = b.position + b.velocity * delta_t;
        }
    }
}
