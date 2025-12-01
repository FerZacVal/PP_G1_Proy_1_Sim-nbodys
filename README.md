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

### 2.2. Importancia Física y Aplicaciones

La simulación N-Body tiene aplicaciones en múltiples áreas de la ciencia:

- **Astrofísica:** Simulación de galaxias, cúmulos estelares, formación de estructuras a gran escala en el universo
- **Dinámica del Sistema Solar:** Predicción de órbitas planetarias, asteroides y cometas
- **Dinámica Molecular:** Simulación de interacciones entre partículas en sistemas químicos y biológicos
- **Cosmología:** Estudio de la evolución del universo y la distribución de materia oscura

### 2.3. Desafío Computacional

El cálculo de la fuerza gravitacional entre todos los pares de cuerpos requiere evaluar N(N-1)/2 interacciones por cada paso de tiempo. Para un sistema con N = 10,000 cuerpos simulado durante 1,000 pasos de tiempo, esto resulta en aproximadamente:

**Operaciones totales ≈ (10,000 × 9,999) / 2 × 1,000 ≈ 5 × 10¹⁰ operaciones**

Esta complejidad cuadrática O(N²) hace que el tiempo de cómputo crezca rápidamente con el número de cuerpos, limitando el tamaño de las simulaciones que pueden realizarse en tiempo razonable con algoritmos secuenciales.

### 2.4. Motivación para Paralelización

El cálculo de la fuerza neta sobre cada cuerpo es **inherentemente paralelizable** porque:

1. La fuerza sobre el cuerpo $i$ es independiente de la fuerza sobre el cuerpo $j$ (para $i \neq j$)
2. No existen dependencias de datos entre los cálculos de diferentes cuerpos en el mismo paso de tiempo
3. Todos los cuerpos acceden a los mismos datos de entrada (posiciones y masas) sin modificarlos durante el cálculo

Esta estructura hace que el problema sea ideal para **paralelización de datos** usando el modelo de memoria compartida de OpenMP, donde múltiples threads pueden calcular simultáneamente las fuerzas sobre diferentes subconjuntos de cuerpos.

### 2.5. Objetivo del Proyecto

El objetivo principal de este proyecto es implementar una simulación N-Body eficiente utilizando paralelización con OpenMP, y analizar el rendimiento obtenido mediante las métricas de:

- **Speedup:** S = T_secuencial / T_paralelo
- **Eficiencia:** E = S / N_threads

**Objetivos específicos de la Entrega 1 (Semana 1):**
1. Implementar la versión secuencial completa del algoritmo N-Body
2. Validar la corrección de la física simulada
3. Establecer la infraestructura del proyecto (estructura de archivos, GitHub)
4. Documentar el fundamento teórico y diseño del sistema
5. Generar salidas en formato de texto para verificación

En las entregas subsiguientes se implementará la paralelización con OpenMP y se realizará el análisis de rendimiento comparativo.

---

## 3. Fundamento Teórico

### 3.1. Ley de Gravitación Universal de Newton

El núcleo de la simulación N-Body se basa en la **Ley de Gravitación Universal** formulada por Isaac Newton en 1687. Esta ley establece que dos cuerpos con masas m₁ y m₂ separados por una distancia r ejercen una fuerza gravitacional atractiva mutua cuya magnitud está dada por:

```
F = G × (m₁ × m₂) / r²
```

Donde:
- **F** es la magnitud de la fuerza gravitacional (en Newtons)
- **G** es la constante de gravitación universal (G = 6.674 × 10⁻¹¹ N·m²/kg²)
- **m₁, m₂** son las masas de los dos cuerpos (en kilogramos)
- **r** es la distancia entre los centros de masa de los cuerpos (en metros)

En forma vectorial, la fuerza que el cuerpo j ejerce sobre el cuerpo i se expresa como:

```
F_ij = G × (m_i × m_j) / r_ij² × r̂_ij
```

Donde r̂_ij es el vector unitario que apunta del cuerpo i al cuerpo j, calculado como:

```
r̂_ij = (r_j - r_i) / |r_j - r_i|
```

La **fuerza neta** sobre un cuerpo i en un sistema de N cuerpos es la suma vectorial de todas las fuerzas ejercidas por los demás cuerpos:

```
F_i^net = Σ F_ij    (para j = 1 hasta N, j ≠ i)
```

Esta suma sobre todos los pares de cuerpos resulta en una complejidad computacional de **O(N²)** por cada paso de tiempo, lo cual representa el principal cuello de botella de la simulación.

### 3.2. Representación Vectorial

Para modelar la simulación, cada cuerpo se representa mediante los siguientes atributos vectoriales en un espacio tridimensional:

#### **Posición (r)**
Vector que indica la ubicación del cuerpo en el espacio:
```
r = (x, y, z)
```

#### **Velocidad (v)**
Vector que representa la tasa de cambio de la posición respecto al tiempo:
```
v = dr/dt = (vₓ, vᵧ, vᵤ)
```

#### **Fuerza (F)**
Vector que representa la fuerza neta que actúa sobre el cuerpo:
```
F = (Fₓ, Fᵧ, Fᵤ)
```

#### **Aceleración (a)**
De acuerdo con la segunda ley de Newton, la aceleración se calcula como:
```
a = F^net / m
```

Estas cantidades vectoriales requieren operaciones básicas como suma, resta, producto escalar y normalización, las cuales se implementan mediante funciones auxiliares en el código.

### 3.3. Integración Numérica

La evolución temporal del sistema se simula mediante **integración numérica** de las ecuaciones de movimiento. Dado que las ecuaciones diferenciales del sistema N-Body no tienen solución analítica en general, se utilizan métodos numéricos para aproximar la trayectoria de cada cuerpo.

#### **3.3.1. Método de Euler (Forward Euler)**

El método más simple de integración es el **Método de Euler**, que actualiza la posición y velocidad de cada cuerpo usando las siguientes fórmulas:

```
v(t + Δt) = v(t) + a(t) × Δt

r(t + Δt) = r(t) + v(t) × Δt
```

Donde:
- **a(t) = F^net(t) / m** es la aceleración en el tiempo t
- **Δt** es el paso de tiempo (time step), típicamente un valor pequeño como 10⁻³ segundos

**Ventajas:**
- Simple de implementar
- Computacionalmente económico

**Desventajas:**
- Error de truncamiento de primer orden: O(Δt)
- Inestable para pasos de tiempo grandes
- No conserva la energía del sistema

### 3.4. Parámetro de Suavizado (Softening Parameter)

En la práctica, cuando dos cuerpos se acercan demasiado, la fuerza gravitacional tiende a infinito (r → 0), lo cual causa inestabilidades numéricas. Para evitar esto, se introduce un **parámetro de suavizado** ε (epsilon) que modifica la ley de gravitación:

```
F = G × (m₁ × m₂) / (r² + ε²)
```

Este término adicional previene que la fuerza crezca sin límite cuando r → 0, estabilizando la simulación sin afectar significativamente el comportamiento a distancias normales. En nuestra implementación usamos ε = 10⁻⁵.

### 3.5. Complejidad Computacional

El cálculo de la fuerza neta sobre cada cuerpo requiere evaluar la interacción con los N-1 cuerpos restantes. Por lo tanto, para N cuerpos, la complejidad por paso de tiempo es:

**Complejidad = O(N²)**

Para una simulación con N = 10,000 cuerpos y S = 1,000 pasos de tiempo, esto resulta en aproximadamente:

**10,000 × 10,000 × 1,000 = 10¹¹ operaciones**

Esta complejidad cuadrática hace que el cálculo de fuerzas sea el **cuello de botella** principal de la simulación, justificando la necesidad de paralelización mediante OpenMP en las siguientes etapas del proyecto.

---

## 4. Diseño e Implementación

### 4.1. Estructura de Datos

La implementación utiliza dos estructuras principales:

#### **Vector3**
```c
typedef struct {
    double x, y, z;
} Vector3;
```
Representa un vector tridimensional usado para posiciones, velocidades y fuerzas.

#### **Body**
```c
typedef struct {
    double m;      // masa
    Vector3 r;     // posición
    Vector3 v;     // velocidad
} Body;
```
Representa un cuerpo con masa, posición y velocidad.

### 4.2. Funciones Auxiliares

Se implementaron funciones para operaciones vectoriales:

- `vector_add(a, b)`: Suma de vectores
- `vector_sub(a, b)`: Resta de vectores
- `vector_scale(a, s)`: Multiplicación por escalar

### 4.3. Algoritmo Secuencial

El algoritmo principal sigue estos pasos:

```
Para cada paso de tiempo:
    1. Reiniciar todas las fuerzas a cero
    2. Para cada par de cuerpos (i, j):
        - Calcular vector distancia: dr = r_j - r_i
        - Calcular magnitud de fuerza: F = G * m_i * m_j / (r² + ε²)
        - Actualizar fuerzas: F_i += F, F_j -= F (3ra ley de Newton)
    3. Para cada cuerpo:
        - Calcular aceleración: a = F / m
        - Actualizar velocidad: v += a * dt
        - Actualizar posición: r += v * dt
```

### 4.4. Optimización: Bucle j = i+1

Para evitar calcular dos veces la fuerza entre cada par de cuerpos, el bucle interno comienza en `j = i+1` y aplicamos simultáneamente la tercera ley de Newton:

```c
for (int i = 0; i < N; i++) {
    for (int j = i+1; j < N; j++) {
        // Calcular F_ij
        forces[i] = vector_add(forces[i], fij);
        forces[j] = vector_sub(forces[j], fij);  // -F_ij por 3ra ley
    }
}
```

Esto reduce el número de cálculos de N² a N(N-1)/2 (aproximadamente la mitad).

### 4.5. Estrategia de Paralelización con OpenMP

La función update_parallel_omp paraleliza el bucle de cálculo de fuerzas, que es el cuello de botella $O(N^2)$, utilizando la directiva principal:

```c
#pragma omp parallel for private(...) shared(...)
for (int i = 0; i < N; i++) {
    // Cálculo de la fuerza neta sobre el cuerpo i
}
``` 
El driver de benchmark (main.cpp) utiliza omp_set_num_threads() y std::chrono para medir el tiempo y calcular las métricas para $P=1, 2, 4, 8$.

### 4.6. Entrada y Salida

El código actual incluye un ejecutable de benchmark (compilado a partir de `main.cpp`) que ejecuta una versión secuencial seguida de varias ejecuciones paralelas. Por ahora los parámetros de simulación están definidos en `main.cpp` como constantes por defecto:

- `N_BODIES` = 1000
- `STEPS` = 100
- `DELTA_T` = 1e-3

**Salida:**
- Archivos CSV con estados finales y métricas (ver sección "Archivos generados").
- Impresiones en consola con tiempos, speedup y eficiencia por cada configuración de hilos.

---

## 5. Resultados y Análisis

### 5.1. Validación de Corrección

La implementación secuencial fue validada con casos de prueba pequeños (N = 5) verificando que:
- Las trayectorias sean físicamente razonables
- No haya valores NaN o Inf en las salidas
- El programa sea determinista (misma entrada → misma salida)

### 5.2. Tiempos de Ejecución (Baseline Secuencial)

Tabla de tiempos de ejecución con diferentes tamaños de problema:

| N (cuerpos) | Steps | Δt | Tiempo (s) | Observaciones |
|-------------|-------|-----|------------|---------------|
| 100 | 100 | 0.001 | s| Caso pequeño |
| 500 | 100 | 0.001 | s | Caso mediano |
| 1000 | 100 | 0.001 | s | Caso grande |
| 5000 | 100 | 0.001 | s | Caso muy grande |
| 10000 | 100 | 0.001 | s | Caso máximo |

**Análisis:**
- Se espera que el tiempo crezca cuadráticamente con N (comportamiento $O(N²)$)
- Estos tiempos servirán como baseline para medir el speedup en la versión paralela

### 5.3. Archivos Generados

Al ejecutar el benchmark desde `main.cpp` se generan los siguientes archivos (útiles para análisis y reportes):

- `final_state_sequential.csv`  : estado final (posiciones y velocidades) de la ejecución secuencial
- `final_state_parallel.csv`    : estado final de la ejecución paralela (guardado para la última configuración paralela)
- `performance_results.csv`     : CSV con columnas `Threads,Tiempo_Segundos,Speedup,Eficiencia_Porcentaje`
- `tabla_resultados.md`         : tabla en Markdown con los resultados (lista formateada para documentación)

En el repositorio también hay archivos de ejemplo ya generados para referencia (`final_state_parallel2.csv`, `final_state_sequential2.csv`).

### 5.4. Discusión de la Ley de Amdahl

La **Ley de Amdahl** es un principio fundamental en computación paralela que establece un límite teórico superior en el speedup alcanzable al paralelizar un programa. La ley se expresa matemáticamente como:

$$S(P) = \frac{1}{(1 - f) + \frac{f}{P}}$$

Donde:
- $S(P)$ es el speedup con $P$ procesadores
- $f$ es la fracción del código que **no puede ser paralelizada** (secuencial)
- $P$ es el número de procesadores/threads

**Implicaciones teóricas:**

- **Caso ideal** ($f = 0$): $S(P) = P$ (speedup lineal, 100% de eficiencia)
- **Caso real** ($f > 0$): El speedup se ve limitado por la fracción secuencial
- **Limite asintótico**: $\lim_{P \to \infty} S(P) = \frac{1}{f}$ (independiente de P)

Por ejemplo, si $f = 0.05$ (5% secuencial), el máximo speedup posible es $1/0.05 = 20$x, independientemente de cuántos threads se usen.

**Comportamiento observado en nuestros experimentos:**

Con los datos generados (N=1000, STEPS=100):

| Threads | Speedup | Eficiencia | Desviación del ideal |
|---------|---------|-----------|----------------------|
| 1 | 1.00x | 100.0% | 0% |
| 2 | 1.97x | 98.5% | +1.5% (overhead mínimo) |
| 4 | 3.47x | 86.8% | -13.2% (contención en memoria) |
| 8 | 4.01x | 50.1% | -49.9% (saturación) |

**Análisis detallado:**

1. **Excelente escalabilidad a P=2:** La eficiencia del 98.5% indica que la fracción secuencial es muy pequeña. El overhead de OpenMP (creación de threads, barreras, sincronización) es casi despreciable.

2. **Degradación notable a P=4 y P=8:** La eficiencia cae a 86.8% y 50.1% respectivamente. Esto sugiere que conforme aumenta el número de threads, factores no ideales comienzan a dominar:
   - **Contención en memoria compartida:** El acceso a datos compartidos (vector de cuerpos, fuerzas) genera conflictos entre threads.
   - **False sharing:** Threads escriben en líneas de caché adyacentes, invalidando la caché entre procesadores.
   - **Overhead de sincronización:** La acumulación de fuerzas requiere mecanismos de reducción (critical sections o atomics) que serializan parcialmente el código.

**Estimación de la fracción secuencial:**

Usando la fórmula de Amdahl con los datos de P=8:

$$4.01 = \frac{1}{(1 - f) + \frac{f}{8}}$$

Despejando $f$:

$$4.01 [(1 - f) + \frac{f}{8}] = 1$$
$$4.01 - 4.01f + \frac{4.01f}{8} = 1$$
$$4.01 - 3.01 = 4.01f - \frac{4.01f}{8}$$
$$3.01 = f(4.01 - 0.50125) = 3.51f$$
$$f \approx 0.858 \text{ (85.8%)}$$

Este valor parece alto y sugiere que el modelo de Amdahl puro no captura completamente el comportamiento observado. Las razones principales son:

- **Overhead de paralelización:** Crear, sincronizar y destruir threads tiene un costo fijo que es especialmente visible en problemas con cálculos cortos por thread.
- **Contención y false sharing:** Aumenta más rápido que linealmente con P.
- **Saturación de ancho de banda:** El cálculo de fuerzas accede intensivamente a memoria compartida; con múltiples threads esto se convierte rápidamente en un cuello de botella.

**Conclusiones para futuras optimizaciones:**

1. **P=2 es óptimo** para este tamaño de problema (N=1000) en hardware típico: alta eficiencia, bajo overhead.
2. **Para P>4**, se recomienda:
   - Implementar buffering local de fuerzas por thread para reducir contención.
   - Usar alineamiento y padding para evitar false sharing.
   - Considerar algoritmos alternativos (Barnes-Hut) si es necesario escalar a N mucho mayor.
   - Analizar con herramientas de perfilado (`perf`, `VTune`) para medir exactamente dónde ocurren los cuellos de botella.

3. **Variación según N:** Para problemas más grandes (N >> 1000), el cálculo por thread es mayor, reduciendo el overhead relativo y permitiendo mejor escalabilidad.

---

## 6. Conclusiones y Trabajo Futuro

### 6.1. Logros de la Entrega Final (Semanas 1–3)

**Semana 1 — Implementación Secuencial:**

1. **Implementación completa y funcional** del algoritmo N-Body secuencial en C++
2. **Validación de corrección física** de la simulación
3. **Infraestructura del proyecto** establecida (GitHub, estructura de código, CMakeLists.txt)
4. **Documentación teórica completa** (Ley de Gravitación, método de Euler, parámetro de suavizado)
5. **Código limpio y bien comentado** con estructura modular (Body, Vector3, NBodySim)

**Semana 2 — Paralelización con OpenMP:**

6. **Implementación de `update_parallel_omp()`** con paralelización del cálculo de fuerzas
7. **Estrategia de reducción de fuerzas** usando buffers privados por thread + reducción crítica
8. **Identificación de variables compartidas/privadas** y aplicación de cláusulas OpenMP explícitas
9. **Uso de `#pragma omp parallel for` con `schedule(dynamic)`** para balanceo dinámico de carga
10. **Verificación funcional** de estados finales guardados en CSV para ambas versiones
11. **Benchmark driver en `main.cpp`** que ejecuta versiones secuencial y paralela con 1, 2, 4, 8 threads

**Semana 3 — Análisis de Rendimiento:**

12. **Medición de tiempos** con múltiples números de threads (1, 2, 4, 8)
13. **Cálculo de métricas**: Speedup y Eficiencia para cada configuración
14. **Análisis de la Ley de Amdahl** con estimación cuantitativa de fracción secuencial (~85.8% en P=8)
15. **Identificación de cuellos de botella**: contención en memoria, false sharing, overhead de sincronización
16. **Generación de reportes** en CSV (`performance_results.csv`) y Markdown (`tabla_resultados.md`)
17. **Análisis de escalabilidad** y conclusiones sobre configuración óptima (P=2 alcanza 98.5% de eficiencia)

**Archivos y Entregables:**

- `final_state_sequential.csv` y `final_state_parallel.csv`: estados finales de ambas versiones
- `performance_results.csv`: métricas cuantitativas (threads, tiempo, speedup, eficiencia)
- `tabla_resultados.md`: tabla formateada para reportes y documentación
- `README.md` completo: documentación teórica, diseño, resultados y análisis

### 6.2. Trabajo Completado vs. Trabajo Futuro

**Resumen de Implementación Realizada:**

El proyecto alcanzó un nivel de madurez significativo en las tres semanas. La version paralela está completamente funcional, con mediciones de rendimiento reales y análisis teórico basado en la Ley de Amdahl. El speedup máximo alcanzado fue **4.0x con 8 threads** (eficiencia 50.1%), confirmando que los cuellos de botella principales son contención de memoria y overhead de sincronización más que la fracción secuencial del código.

**Trabajo Futuro (mejoras y extensiones):**

El proyecto estableció una sólida base para futuras optimizaciones:

1. **Optimización de contención en memoria (Corto plazo):**
   - Implementar alineamiento de datos (padding) para evitar false sharing
   - Probar reducción con atomics en lugar de `#pragma omp critical`
   - Reorganizar datos a Structure-of-Arrays (SoA) para mejorar localidad de caché
   - Experimentar con diferentes `schedule` (static vs guided) y chunk sizes

2. **Escalabilidad a problemas más grandes (Corto plazo):**
   - Medir escalabilidad con N mucho mayor (10k, 100k cuerpos) donde el overhead relativo disminuye
   - Implementar pasos de tiempo adaptativos para mejorar eficiencia
   - Considerar algoritmos alternativos (Barnes-Hut, Fast Multipole) para O(N log N)

3. **Aceleración por GPU (Mediano plazo):**
   - Portar el núcleo de cálculo de fuerzas a CUDA o OpenCL para obtener speedups mayores
   - Manejar transferencias de datos GPU↔CPU eficientemente
   - Verificar precisión numérica en double precision

4. **Distribución en memoria (Mediano plazo):**
   - Paralelizar con MPI para escalar a clústeres
   - Combinar MPI + OpenMP (hybrid) para máxima escalabilidad
   - Implementar decomposición de dominio o decomposición de datos

5. **Integradores numéricos avanzados (Mediano plazo):**
   - Reemplazar Euler con Verlet/Leapfrog para mejor conservación de energía
   - Implementar RK de orden superior (RK4, RK8) para mayor precisión
   - Usar integradores symplecticos para sistemas Hamiltonianos

6. **Robustez y usabilidad (Corto plazo):**
   - Añadir parsing de argumentos para cambiar N, STEPS, DELTA_T sin recompilar
   - Soportar archivos de configuración (YAML/JSON)
   - Implementar pruebas unitarias e integración continua (CI)
   - Añadir checkpoints para reanudar simulaciones largas

7. **Visualización y análisis (Mediano plazo):**
   - Exportar trayectorias en formato VTK para visualización 3D
   - Generar animaciones de la evolución del sistema
   - Calcular magnitudes conservadas (energía, momento angular) para validación

---

## 7. Referencias

1. 

---

## Estructura del Repositorio

```
PP_G1_Proy_1_Sim-nbodys/
├── README.md
├── CMakeLists.txt
├── main.cpp
├── include/
│   ├── Body.h
│   ├── NBodySim.h
│   └── Vector3.h
├── src/
│   ├── Body.cpp
│   ├── NBodySim.cpp
│   └── Vector3.cpp
├── final_state_parallel.csv
├── final_state_parallel2.csv
├── final_state_sequential.csv
├── final_state_sequential2.csv
├── performance_results.csv
├── tabla_resultados.md
└── .gitignore
```

---

## Instrucciones de Uso

### Compilación

Ejemplo usando `g++` y OpenMP (comando que compila `main.cpp` y todos los archivos en `src/`):

```bash
g++ -O2 -std=c++11 main.cpp src/*.cpp -Iinclude -fopenmp -o simnbody.exe
```

También puede compilarse con `CMake` aprovechando el `CMakeLists.txt` incluido:

```bash
mkdir -p build && cd build
cmake ..
make -j
# El ejecutable resultante será algo como `simnbody.exe` según configuración de CMake
```

### Ejecución

Ejecutar el binario generado (no requiere argumentos por defecto):

```bash
./simnbody.exe
```

Comandos útiles para inspeccionar las salidas:

```bash
cat performance_results.csv
less tabla_resultados.md
head -n 10 final_state_sequential.csv
```

# Caso grande (1000 cuerpos, 100 pasos)
./nbody_secuencial 1000 100 0.001
```

### Verificación de Salida
```bash
cat output.txt
```

---

## Estado del Proyecto

- [x] **Semana 1:** Versión Secuencial 
- [x] **Semana 2:** Paralelización con OpenMP
- [x] **Semana 3:** Análisis de Rendimiento y Entrega Final

---

**Última actualización:** [1/12/2025]  
**Contacto:** [erick.quispe@unmsm.edu.pe],
              [patrick.coronel@unmsm.edu.pe],
              [fernando.zacarias@unmsm.edu.pe],
              [richard.carrasco@unmsm.edu.pe]

