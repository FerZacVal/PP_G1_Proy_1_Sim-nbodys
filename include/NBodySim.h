#ifndef NBODYSIM_H
#define NBODYSIM_H

#include <vector>
#include <string>

#include "Body.h"
#include "Vector3.h"

class NBodySim {
public:
    int N_bodies;
    double delta_t;
    double G_const;
    std::vector<Body> bodies;

    NBodySim(int N, double dt, double G);

    void initialize_bodies(unsigned int seed);
    void calculate_forces();
    void integrate();
    void update_sequential(int steps);
    void update_parallel_omp(int steps);
    void save_final_state(const std::string& filename);
};

#endif
