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

### 4.5. Condiciones Iniciales

El programa inicializa los primeros 5 cuerpos con valores fijos para reproducibilidad, y el resto (si N > 5) con valores aleatorios pero con semilla fija (`srand(12345)`).

### 4.6. Entrada y Salida

**Entrada (parámetros de línea de comandos):**
```bash
./nbody_secuencial [N] [steps] [dt]
```
- `N`: Número de cuerpos (default: 5)
- `steps`: Número de pasos de tiempo (default: 500)
- `dt`: Tamaño del paso de tiempo (default: 0.001)

**Salida:**
- Archivo `output.txt`: Posiciones y velocidades finales de todos los cuerpos
- Consola: Tiempo de ejecución y parámetros de simulación

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

```
output.txt - Estado final del sistema (posiciones y velocidades)
```

Ejemplo de salida:
```
Body 0: m=1.000000e+10  x=-0.450123 y=0.049876 z=0.000000  vx=0.000234 vy=0.099845 vz=0.000000
Body 1: m=1.000000e+10  x=0.550234 y=-0.050123 z=0.000000  vx=-0.000123 vy=-0.100234 vz=0.000000
...
```

---

## 6. Conclusiones y Trabajo Futuro

### 6.1. Logros de la Entrega 1

En esta primera fase del proyecto se logró:

1. **Implementación completa y funcional** del algoritmo N-Body secuencial
2. **Validación de la corrección física** de la simulación
3. **Infraestructura del proyecto** establecida (GitHub, estructura de código)
4. **Documentación completa** del fundamento teórico y diseño
5. **Código limpio y bien comentado** siguiendo buenas prácticas

La versión secuencial sirve como:
- **Baseline** para medir el speedup de la versión paralela
- **Referencia de corrección** para validar la implementación paralela
- **Base de código** para la paralelización en las siguientes semanas

### 6.2. Trabajo Futuro (Semanas 2 y 3)

**Semana 2: Paralelización con OpenMP**
- Implementar `update_parallel_omp()` con `#pragma omp parallel for`
- Identificar variables compartidas y privadas
- Manejar la reducción de fuerzas si es necesario
- Validar que los resultados sean idénticos a la versión secuencial

**Semana 3: Análisis de Rendimiento y Optimización**
- Medir tiempos con 1, 2, 4, 8, 16 threads
- Calcular Speedup y Eficiencia
- Experimentar con cláusulas de schedule (static, dynamic, guided)
- Analizar resultados en el contexto de la Ley de Amdahl
- Optimizar acceso a memoria (false sharing, cache locality)

---

## 7. Referencias

1. 

---

## Estructura del Repositorio

```
NBody-OpenMP/
├── README.md                    (Este archivo)
├── src/
│   ├── nbody_secuencial.c      (Implementación secuencial - Entrega 1)
│   ├── nbody_parallel.c        (Implementación paralela - Semana 2)
│   └── Makefile
├── data/
│   ├── output.txt              (Salida de la simulación)
│   └── performance_data.csv    (Datos de rendimiento - Semana 3)
├── docs/
│   └── images/                 (Gráficos de speedup - Semana 3)
└── .gitignore
```

---

## Instrucciones de Uso

### Compilación
```bash
gcc -O2 nbody_secuencial.c -o nbody_secuencial -lm
```

### Ejecución
```bash
# Caso pequeño (5 cuerpos, 500 pasos)
./nbody_secuencial 5 500 0.001

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
- [ ] **Semana 2:** Paralelización con OpenMP
- [ ] **Semana 3:** Análisis de Rendimiento y Entrega Final

---

**Última actualización:** [17/11/2025]  
**Contacto:** [erick.quispe@unmsm.edu.pe]
              [patrick.coronel@unmsm.edu.pe]
              [richard.carrasco@unmsm.edu.pe]
              [richard.carrasco@unmsm.edu.pe]

