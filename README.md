# Proyecto Grupal 1: Simulación de Cuerpos N-Body Paralela

## Equipo de Desarrollo

* Zacarias Valdez Fernando Andre (21200183) fernando.zacarias@unmsm.edu.pe
* Carrasco Curi Richard Alberto  (22200010) richard.carrasco@unmsm.edu.pe
* Quispe Huari Erick             (13200175) erick.quispe@unmsm.edu.pe
* Coronel Ramirez Patrick Miguel (18200143) patrick.coronel@unmsm.edu.pe

**Universidad:** Universidad Nacional Mayor de San Marcos

**Curso:** Programación Paralela

**Fecha:** 17 Noviembre 2025

---

## 1. Resumen (Abstract)

El problema de N-Body consiste en simular la evolución temporal de un sistema de &N& cuerpos bajo la influencia de la fuerza gravitacional mutua, regida por la Ley de Gravitación Universal de Newton. Este problema tiene aplicaciones en astrofísica, dinámica molecular y simulaciones de fluidos. El principal desafio computacional radica en el cálculo de las feurzas de interacción entre todos los pares de cuerpos, lo cual tiene una complejidad de &O(N^2)& por cada paso de tiempo.

En este proyecto, implementamos una simulación N-Body utilizado el Método de Euler para la integración numérica de las ecuaciones de movimiento. La primera fase del proyecto se enfocó en desarrollar e implemtnar una **versión secuencial completa y funcional** del alogritmo en C/C++. Esta versión sirve como referencia para validar la correción de futuras implementaciones paralelas.

Los objetivos de esta primera entrega son:
1. Implementar el algoritmo secuencial completo
2. Validar la correción física de la simulación
3. Medir el tiempo de ejecución como baseline para comparaciones futuras
4. Establecer la infraestructura del proyecto y documentación

En las siguientes fases del proyecto se implementará la paralelización utilizando **OpenMP** para lograr un speedup significativo, aprovechando la naturaleza inherentemente paralela del cálculo de fuerzas entre cuerpos independientes.

**Palabras clave:** N-Body, simulación gravitacional, OpenMP, programación paralela, HPC

## 2. Introducción

### 2.1 Contexto e Importancia

El problema de N-cuerpos es fundamental en la física computacional. Consiste en simular la evolución dinámica de un sistema de $N$ cuerpos que interactúan entre sí. Aunque su aplicación más conocida es la astrofísica (simulación de la formación de galaxias, cúmulos estelares y sistemas solares), el mismo principio se aplica a otros dominios (1).

En la **dinámica molecular**, las simulaciones N-Body modelan la interacción de átomos y moléculas para estudiar el plegamiento de proteínas, el diseño de fármacos o la ciencia de materiales (2). En la física de plasmas, se usa para modelar partículas cargadas. La capacidad de simular estos sistemas es crucial para la investigación científica.

### 2.2 Dificultad Computacional

La principal barrera computacional de la simulación N-Body, según se describe en el documento del proyecto (`proyecto01.pdf`), radica en su costo algorítmico. El método implementado, conocido como "suma directa" (*direct-sum*), calcula la fuerza neta sobre cada cuerpo sumando las fuerzas individuales ejercidas por los $N-1$ cuerpos restantes.

Este cálculo debe repetirse para cada uno de los $N$ cuerpos del sistema. La complejidad total de esta fase es de orden $O(N^2)$ por cada paso de tiempo (3). Un costo $O(N^2)$ significa: duplicar el número de cuerpos ($N$) multiplica el tiempo de cómputo aproximadamente por cuatro. Para simulaciones científicas con millones de cuerpos, este costo cuadrático vuelve la versión secuencial computacionalmente prohibitiva.

### 2.3 Objetivo de Paralelización

Dada esta complejidad, el método de suma directa es un candidato ideal para la paralelización. El cálculo de la fuerza para el cuerpo *i* es independiente del cálculo para el cuerpo *j*. El objetivo de este proyecto es explotar esta independencia utilizando OpenMP para paralelizar el bucle de $O(N^2)$.

Existen algoritmos de aproximación, como **Barnes-Hut**, los cuales reducen la complejidad a $O(N \log N)$ agrupando cuerpos distantes (4). Sin embargo, estos métodos introducen estructuras de datos complejas (árboles) las cuales son más difíciles de implementar y balancear en paralelo. El método de suma directa, aunque costoso, es aritméticamente denso y su estructura de bucles anidados es perfecta para el paralelismo de datos ofrecido por OpenMP.

## Referencias

1.  Dassault Systèmes. (s.f.). *Simulaciones de dinámica molecular*. BIOVIA. Recuperado de: https://www.3ds.com/es/products/biovia/discovery-studio/simulations
2.  IRB Barcelona. (2019). *La simulación por dinámica molecular ofrece una perspectiva de "interacción singular" a la dinámica de proteínas y la unión a fármacos*. Recuperado de: https://www.irbbarcelona.org/es/news/la-simulacion-por-dinamica-molecular-ofrece-una-perspectiva-de-interaccion-singular-a-la
3.  Wikipedia. (s.f.). *Problema de los n cuerpos*. Recuperado de: https://es.wikipedia.org/wiki/Problema_de_los_n_cuerpos
4.  Wikipedia. (s.f.). *Barnes–Hut simulation*. Recuperado de: https://en.wikipedia.org/wiki/Barnes%E2%80%93Hut_simulation
