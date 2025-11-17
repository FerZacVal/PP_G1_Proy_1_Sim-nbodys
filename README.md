# Simulación de Cuerpos N-Body Paralela

## Equipo de Desarrollo

|       Nombre       | Rol | Github |

| ================== | ====== | ====== |

| Erick | Implementación Secuencial | 

| Fernando Zacarias | Medición de Rendimiento | 

| Richard Carrasco | Documentación | 

**Universidad:** Universidad Nacional Mayor de San Marcos
**Curso:** Programación Paralela
**Fecha:** 17 Noviembre 2025

---

## 1. Resumen (Abstract)

El problema de N-Body consiste en simular la evolución temporal de un sistema de &N& cuerpos bajo la influencia de la fuerza gravitacional mutua, regida por la Ley de Gravitación Universal de Newton. Este problema tiene aplicaciones en astrofísica, dinámica molecular y simulaciones de fluidos. El principal desafio computacional radica en el cálculo de las feurzas de interacción entre todos los pares de cuerpos, lo cual tiene una complejidad de &O(N²)& por cada paso de tiempo.

En este proyecto, implementamos una simulación N-Body utilizado el Método de Euler para la integración numérica de las ecuaciones de movimiento. La primera fase del proyecto se enfocó en desarrollar e implemtnar una **versión secuencial completa y funcional** del alogritmo en C/C++. Esta versión sirve como referencia para validar la correción de futuras implementaciones paralelas.

Los objetivos de esta primera entrega son:
1. Implementar el algoritmo secuencial completo
2. Validar la correción física de la simulación
3. Medir el tiempo de ejecución como baseline para comparaciones futuras
4. Establecer la infraestructura del proyecto y documentación

En las siguientes fases del proyecto se implementará la paralelización utilizando **OpenMP** para lograr un speedup significativo, aprovechando la naturaleza inherentemente paralela del cálculo de fuerzas entre cuerpos independientes.

**Palabras clave:** N-Body, simulación gravitacional, OpenMP, programación paralela, HPC

## 2. Introducción

### 2.1. Contexto del Problema

El problema de N-Body es un problema clásico en física computacional que consisten en predecir el movimiento de un conjunto de objetos celestes que interactúan gravitacionalmente. Este problema fue planteado originalmente por Isaac Newton en el siglo XVII para describir el movimiento de los planetas del sistema solar.

Aunque para dos cuerpos existe una solución analítica (el problema de Kepler), para tres o más cuerpos no existe una solución general en forma cerrada. Por lo tanto, es necesario recurrir a métodos numéricos para aproximar la evolución temporal del sistema.

