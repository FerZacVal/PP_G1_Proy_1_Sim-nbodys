#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>

struct Vector3 {
    double x, y, z;

    // Constructor
    Vector3(double _x = 0.0, double _y = 0.0, double _z = 0.0) : x(_x), y(_y), z(_z) {}

    // Operación: Magnitud del vector (distancia r)
    double magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Operación: Magnitud al cuadrado (para evitar la raíz cuadrada en el cálculo de fuerza)
    double magnitude_sq() const {
        return x * x + y * y + z * z;
    }

    // Operador de resta (r_ij = x_j - x_i)
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    // Operador de suma
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // Operador de multiplicación por escalar (ej. a * dt)
    Vector3 operator*(double scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // Operador de división por escalar (ej. F / m)
    Vector3 operator/(double scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
};

#endif // VECTOR3_H