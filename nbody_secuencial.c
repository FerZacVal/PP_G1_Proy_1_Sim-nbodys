/*
 * nbody_secuencial.c
 * Simulación N-body secuencial
 *
 * Compilar:
 *   gcc -O2 nbody_secuencial.c -o nbody_secuencial -lm
 *
 * Ejecutar:
 *   ./nbody_secuencial [N] [steps] [dt]
 * Ejemplo:
 *   ./nbody_secuencial 5 500 0.001
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct {
    double x, y, z;
} Vector3;

typedef struct {
    double m;
    Vector3 r; // posicion
    Vector3 v; // velocidad
} Body;

// Funciones auxiliares
Vector3 vector_add(Vector3 a, Vector3 b) {
    Vector3 r = {a.x + b.x, a.y + b.y, a.z + b.z};
    return r;
}

Vector3 vector_sub(Vector3 a, Vector3 b) {
    Vector3 r = {a.x - b.x, a.y - b.y, a.z - b.z};
    return r;
}

Vector3 vector_scale(Vector3 a, double s) {
    Vector3 r = {a.x * s, a.y * s, a.z * s};
    return r;
}

int main(int argc, char** argv) {

    int N = 5;
    int steps = 500;
    double dt = 0.001;

    if (argc >= 2) N = atoi(argv[1]);
    if (argc >= 3) steps = atoi(argv[2]);
    if (argc >= 4) dt = atof(argv[3]);

    const double G = 6.67430e-11;
    const double softening = 1e-5;

    Body* bodies = (Body*) malloc(N * sizeof(Body));
    Vector3* forces = (Vector3*) malloc(N * sizeof(Vector3));

    if (bodies == NULL || forces == NULL) {
        printf("Error: no se pudo asignar memoria.\n");
        return 1;
    }

    // Inicializar primeros 5 cuerpos fijos
    if (N >= 1) { bodies[0].m = 1e10; bodies[0].r = (Vector3){-0.5,0.0,0.0}; bodies[0].v = (Vector3){0.0,0.1,0.0}; }
    if (N >= 2) { bodies[1].m = 1e10; bodies[1].r = (Vector3){ 0.5,0.0,0.0}; bodies[1].v = (Vector3){0.0,-0.1,0.0}; }
    if (N >= 3) { bodies[2].m = 5e9;  bodies[2].r = (Vector3){0.0,0.6,0.0};  bodies[2].v = (Vector3){-0.1,0.0,0.0}; }
    if (N >= 4) { bodies[3].m = 5e9;  bodies[3].r = (Vector3){0.0,-0.6,0.0}; bodies[3].v = (Vector3){0.1,0.0,0.0}; }
    if (N >= 5) { bodies[4].m = 1e9;  bodies[4].r = (Vector3){0.0,0.0,0.5};  bodies[4].v = (Vector3){0.0,0.0,-0.1}; }

    // Inicializar aleatoriamente si N > 5
    srand(12345);
    for (int i = 5; i < N; i++) {
        bodies[i].m = 1e9 + (rand() % 1000) * 1e6;
        bodies[i].r = (Vector3){
            ((double)rand() / RAND_MAX) * 2 - 1,
            ((double)rand() / RAND_MAX) * 2 - 1,
            ((double)rand() / RAND_MAX) * 2 - 1
        };
        bodies[i].v = (Vector3){0.0, 0.0, 0.0};
    }

    // Medir tiempo
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Bucle de simulación
    for (int step = 0; step < steps; step++) {

        // Reiniciar fuerzas
        for (int i = 0; i < N; i++) {
            forces[i] = (Vector3){0.0, 0.0, 0.0};
        }

        // Calcular fuerzas (O(N^2))
        for (int i = 0; i < N; i++) {
            for (int j = i+1; j < N; j++) {

                Vector3 dr = vector_sub(bodies[j].r, bodies[i].r);
                double r2 = dr.x*dr.x + dr.y*dr.y + dr.z*dr.z + softening*softening;
                double r = sqrt(r2);

                double Fmag = 0.0;
                if (r > 0.0)
                    Fmag = G * bodies[i].m * bodies[j].m / r2;

                Vector3 fij = {0.0, 0.0, 0.0};
                if (r > 0.0) {
                    double invr = 1.0 / r;
                    fij = vector_scale(dr, Fmag * invr);
                }

                forces[i] = vector_add(forces[i], fij);
                forces[j] = vector_sub(forces[j], fij);
            }
        }

        // Actualizar posiciones (Euler)
        for (int i = 0; i < N; i++) {
            Vector3 a = vector_scale(forces[i], 1.0 / bodies[i].m);

            bodies[i].v = vector_add(bodies[i].v, vector_scale(a, dt));
            bodies[i].r = vector_add(bodies[i].r, vector_scale(bodies[i].v, dt));
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;

    // Guardar el resultado
    FILE* fout = fopen("output.txt", "w");
    if (!fout) {
        printf("Error: no se pudo crear output.txt\n");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        fprintf(fout,
            "Body %d: m=%e  x=%f y=%f z=%f  vx=%f vy=%f vz=%f\n",
            i,
            bodies[i].m,
            bodies[i].r.x, bodies[i].r.y, bodies[i].r.z,
            bodies[i].v.x, bodies[i].v.y, bodies[i].v.z
        );
    }
    fclose(fout);

    printf("Simulacion N-body secuencial finalizada.\n");
    printf("N=%d, steps=%d, dt=%f\n", N, steps, dt);
    printf("Tiempo de ejecucion: %.6f s\n", elapsed);
    printf("Salida escrita en output.txt\n");

    free(bodies);
    free(forces);

    return 0;
}
