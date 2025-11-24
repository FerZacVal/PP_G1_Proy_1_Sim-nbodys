#include "NBodySim.h"
#include <iostream>
#include <fstream>
#include <random>
#include <omp.h>

// ==========================================
// CONSTRUCTOR
// ==========================================
NBodySim::NBodySim(int N, double dt, double G)
    : N_bodies(N), delta_t(dt), G_const(G) {
    bodies.reserve(N);
}

// ==========================================
// INICIALIZACIÓN REPRODUCIBLE
// ==========================================
void NBodySim::initialize_bodies(unsigned int seed) {

    std::mt19937 gen(seed);

    std::uniform_real_distribution<> dist_pos(-10.0, 10.0);
    std::uniform_real_distribution<> dist_vel(-0.1, 0.1);
    std::uniform_real_distribution<> dist_mass(1.0, 100.0);

    bodies.clear();
    bodies.reserve(N_bodies);

    for (int i = 0; i < N_bodies; ++i) {
        bodies.emplace_back(
            dist_mass(gen),
            Vector3(dist_pos(gen), dist_pos(gen), dist_pos(gen)),
            Vector3(dist_vel(gen), dist_vel(gen), dist_vel(gen))
        );
    }
}

// ==========================================
// FUERZAS SEC. (O(N²))
// ==========================================
void NBodySim::calculate_forces() {
    for (int i = 0; i < N_bodies; ++i)
        bodies[i].reset_force();

    for (int i = 0; i < N_bodies; ++i) {
        for (int j = i + 1; j < N_bodies; ++j) {

            Vector3 r = bodies[j].position - bodies[i].position;
            double r_sq = r.magnitude_sq();

            if (r_sq < 1e-9) continue;

            double dist = sqrt(r_sq);
            double F_mag = G_const * bodies[i].mass * bodies[j].mass / r_sq;
            Vector3 F = r * (F_mag / dist);

            bodies[i].force = bodies[i].force + F;
            bodies[j].force = bodies[j].force - F;
        }
    }
}

// ==========================================
// INTEGRACIÓN (EULER)
// ==========================================
void NBodySim::integrate() {
    for (int i = 0; i < N_bodies; ++i) {

        Body &b = bodies[i];

        Vector3 acc = b.force / b.mass;

        b.velocity = b.velocity + acc * delta_t;
        b.position = b.position + b.velocity * delta_t;
    }
}

// ==========================================
// VERSIÓN SECUENCIAL
// ==========================================
void NBodySim::update_sequential(int steps) {
    for (int t = 0; t < steps; t++) {
        calculate_forces();
        integrate();
    }
}

// ==========================================
// VERSIÓN PARALELA O(N²)
// ==========================================
void NBodySim::update_parallel_omp(int steps) {

    #pragma omp parallel
    {
        #pragma omp single
        std::cout << "[OMP] Hilos activos: "
                  << omp_get_num_threads() << "\n";
    }

    for (int t = 0; t < steps; ++t) {

        std::vector<Vector3> force_global(N_bodies, Vector3(0,0,0));

        #pragma omp parallel
        {
            std::vector<Vector3> local(N_bodies, Vector3(0,0,0));

            #pragma omp for schedule(dynamic)
            for (int i = 0; i < N_bodies; ++i) {
                for (int j = i + 1; j < N_bodies; ++j) {

                    Vector3 r = bodies[j].position - bodies[i].position;
                    double r_sq = r.magnitude_sq();
                    if (r_sq < 1e-9) continue;

                    double dist = sqrt(r_sq);
                    double F_mag = G_const * bodies[i].mass * bodies[j].mass / r_sq;
                    Vector3 F = r * (F_mag / dist);

                    local[i] = local[i] + F;
                    local[j] = local[j] - F;
                }
            }

            #pragma omp critical
            {
                for (int i = 0; i < N_bodies; ++i)
                    force_global[i] = force_global[i] + local[i];
            }
        }

        for (int i = 0; i < N_bodies; ++i)
            bodies[i].force = force_global[i];

        #pragma omp parallel for
        for (int i = 0; i < N_bodies; ++i) {
            Body &b = bodies[i];
            Vector3 acc = b.force / b.mass;
            b.velocity = b.velocity + acc * delta_t;
            b.position = b.position + b.velocity * delta_t;
        }
    }
}

// ==========================================
// EXPORTAR CSV
// ==========================================
void NBodySim::save_final_state(const std::string &filename) {
    std::ofstream ofs(filename);
    ofs << "id,mass,x,y,z,vx,vy,vz\n";

    for (int i = 0; i < N_bodies; ++i) {
        Body &b = bodies[i];

        ofs << i << "," << b.mass << ","
            << b.position.x << ","
            << b.position.y << ","
            << b.position.z << ","
            << b.velocity.x << ","
            << b.velocity.y << ","
            << b.velocity.z << "\n";
    }
}
