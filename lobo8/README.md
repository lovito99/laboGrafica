# Práctica 08 – Transformación 2D: Reflexión
**UNSAAC – Computación Gráfica I**

## Estructura del proyecto

```
lobo8/
├── CMakeLists.txt
├── P08.pdf
└── src/
    ├── reflexion.cpp          ← Ejemplo del PDF (Sección 3)
    ├── actividadPentagono.cpp ← Actividad 1
    └── actividadTriangulo.cpp ← Actividad 2
```

## Compilar (desde WSL)

```bash
cd ~/Documentos/GitHub/laboGrafica/lobo8
mkdir -p build && cd build
cmake .. -G "Unix Makefiles"
make
```

## Ejecutables

### `reflexion` — Ejemplo del PDF (Sección 3.2)

Implementa **exactamente** el código del PDF:

| Función (PDF)           | Descripción                                   |
|-------------------------|-----------------------------------------------|
| `createIdentityMatrix`  | Matriz identidad 4×4 column-major             |
| `createTranslationMatrix` | Matriz de traslación 2D homogénea           |
| `createScaleMatrix`     | Matriz de escalamiento 2D homogénea           |
| `multiplyMatrices`      | Multiplicación A×B de matrices 4×4            |
| `renderTransformations` | Dibuja las 3 transformaciones del PDF         |

**Lo que se muestra:**

| Color    | Transformación                              |
|----------|---------------------------------------------|
| Blanco   | Triángulo original                          |
| Amarillo | Escalado con punto fijo Sx=Sy=1.5 (§3.1)   |
| Verde    | Reflexión eje X — `ScaleMatrix(1, -1)` (§3.2) |
| Cián     | Reflexión eje Y — `ScaleMatrix(-1, 1)` (§3.3) |

```bash
./build/reflexion
```

---

### `actividadPentagono` — Actividad 1

> *"Escribir un programa que refleje un polígono de 5 lados con respecto al eje X."*

Aplica `createScaleMatrix(mat, 1.0f, -1.0f)` sobre un pentágono regular.

| Color  | Figura                         |
|--------|-------------------------------|
| Blanco | Pentágono original             |
| Verde  | Pentágono reflejado en eje X   |

**Controles:** `+`/`-` cambiar tamaño · `R` reiniciar · `ESC` salir

```bash
./build/actividadPentagono
```

---

### `actividadTriangulo` — Actividad 2

> *"Escribir un programa para realizar reflexión general en 2D de un triángulo."*

Reflexión respecto a una **línea arbitraria** que pasa por el origen con ángulo θ.

**Matriz de reflexión general:**
```
| cos(2θ)   sin(2θ)  0  0 |
| sin(2θ)  -cos(2θ)  0  0 |
|    0         0     1  0 |
|    0         0     0  1 |
```

| Color   | Figura                              |
|---------|-------------------------------------|
| Blanco  | Triángulo original                  |
| Magenta | Triángulo reflejado                 |
| Amarillo| Línea de reflexión (eje rotado)     |

**Controles:** `←`/`→` rotar eje · `R` reiniciar · `ESC` salir

```bash
./build/actividadTriangulo
```

## Base teórica (resumen del PDF)

| Reflexión   | Matriz 3×3 (homogénea)                             |
|-------------|-----------------------------------------------------|
| Eje X       | `diag(1, -1, 1)` equivale a `Scale(1, -1)`          |
| Eje Y       | `diag(-1, 1, 1)` equivale a `Scale(-1, 1)`          |
| Línea θ     | `createReflectionMatrix(mat, theta)`                |
