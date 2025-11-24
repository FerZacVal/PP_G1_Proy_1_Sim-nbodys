#ifndef BODY_H
#define BODY_H

#include "Vector3.h"

struct Body {
    double mass;
    Vector3 position;   // x
    Vector3 velocity;   // v
    Vector3 force;      // F

    // Constructor (ejemplo de inicialización)
    Body(double m = 1.0, Vector3 pos = Vector3(), Vector3 vel = Vector3())
        : mass(m), position(pos), velocity(vel), force(Vector3()) {}

    // Reiniciar la fuerza neta para el nuevo cálculo en el siguiente paso de tiempo
    void reset_force() {
        force = Vector3();
    }
};

#endif // BODY_H