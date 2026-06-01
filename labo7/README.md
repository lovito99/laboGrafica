# Práctica 07 — Transformación 2D: Escala & Rotación

**Computación Gráfica I**  
Universidad Nacional de San Antonio Abad del Cusco  
Escuela Profesional: Ing. Informática y de Sistemas

> Repositorio: <https://github.com/lovito99/laboGrafica/tree/main/labo7>

---

## Descripción

Implementación de las transformaciones 2D de **escalamiento** y **rotación**
mediante matrices 3×3 en coordenadas homogéneas (column-major, OpenGL 3.3 Core Profile).

Se desarrollaron 4 programas:

| Ejecutable | Archivo | Descripción |
|---|---|---|
| `escalamiento` | `src/escalamiento.cpp` | Escalamiento 2D con factores Sx, Sy e uniforme (Listado 1 del PDF) |
| `rotacion` | `src/rotacion.cpp` | Rotación 2D alrededor del origen y de un punto arbitrario (Listado 2 del PDF) |
| `actividadCirculo` | `src/actividadCirculo.cpp` | **Actividad 1**: Escalar un círculo ×2 manteniendo fijo su centro |
| `actividadRectangulo` | `src/actividadRectangulo.cpp` | **Actividad 2**: Traslación y luego rotación de un rectángulo |

---

## Requisitos

- CMake ≥ 3.10
- GCC / G++ con C++17
- OpenGL 3.3
- GLEW
- GLFW3
- GLM

En Ubuntu/Debian:
```bash
sudo apt install cmake libglew-dev libglfw3-dev libglm-dev
```

---

## Compilación

```bash
cd labo7
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

---

## Ejecución

Desde `labo7/build/`:

```bash
# Listado 1 del PDF — Escalamiento 2D
./escalamiento

# Listado 2 del PDF — Rotación 2D
./rotacion

# Actividad 1 — Escalar círculo ×2 con centro fijo
./actividadCirculo

# Actividad 2 — Traslación + Rotación de rectángulo
./actividadRectangulo
```

---

## Controles

### `escalamiento`
| Tecla | Acción |
|---|---|
| `Q` / `A` | Aumentar / Disminuir escala en X |
| `W` / `S` | Aumentar / Disminuir escala en Y |
| `E` / `D` | Aumentar / Disminuir escala uniforme |
| `R` | Reiniciar escalas a 1.0 |
| `ESC` | Salir |

### `rotacion` y `actividadRectangulo`
| Tecla | Acción |
|---|---|
| `R` | Aumentar ángulo de rotación |
| `T` | Disminuir ángulo de rotación |
| `ESC` | Salir |

### `actividadCirculo`
Solo visualización estática. Cerrar con `ESC` o el botón ✕ de la ventana.

---

## Fundamento matemático

### Escalamiento
$$S(s_x, s_y) = \begin{pmatrix} s_x & 0 & 0 \\ 0 & s_y & 0 \\ 0 & 0 & 1 \end{pmatrix}$$

Escalamiento alrededor de un punto $(c_x, c_y)$:
$$S_c = T(c_x,c_y) \cdot S(s_x,s_y) \cdot T(-c_x,-c_y)$$

### Rotación
$$R(\theta) = \begin{pmatrix} \cos\theta & -\sin\theta & 0 \\ \sin\theta & \cos\theta & 0 \\ 0 & 0 & 1 \end{pmatrix}$$

### Composición (Actividad 2)
$$M = R(\theta) \cdot T(t_x, t_y)$$
Aplica la traslación primero, luego la rotación.

---

## Estructura del proyecto

```
labo7/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── escalamiento.cpp
│   ├── rotacion.cpp
│   ├── actividadCirculo.cpp
│   └── actividadRectangulo.cpp
├── documentacion/
│   ├── infomelobo7.tex
│   └── infomelobo7.pdf
└── build/
    ├── escalamiento
    ├── rotacion
    ├── actividadCirculo
    └── actividadRectangulo
```

---

**Estudiante:** Efrain Vitorino Marin — Código: 160337  
**Docente:** Dr. Hans Harley Ccacyahuillca Bejar  
**Fecha:** 1 de junio de 2026
