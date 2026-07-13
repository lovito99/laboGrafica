# Practica 10 - Transformaciones en el Espacio 3D
**UNSAAC - Computacion Grafica I**

## Estructura

```text
labo10/
├── CMakeLists.txt
├── P10.pdf
├── README.md
├── p10base.hpp
├── actCubo.cpp
└── actTetra.cpp
```

## Compilar

```bash
cd ~/Documentos/GitHub/laboGrafica/labo10
mkdir -p build && cd build
cmake .. -G "Unix Makefiles"
make
```

## Ejecutar

```bash
./build/actCubo
./build/actTetra
```

Presione `ESC` para cerrar cada ventana.

## Capturas

### Cubo rotado

![Cubo rotado](<CUBO ROTADO.png>)

### Tetraedro rotado

![Tetraedro rotado](<TETRAEDO-ROROTADO.png>)

## Librerias usadas

Se sigue la plantilla moderna del PDF:

```cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
```

## Rotacion alrededor de una linea arbitraria

Para una linea definida por dos puntos `A` y `B`, el vector director es:

```text
u = normalize(B - A)
```

La rotacion respecto al eje que pasa por el origen se calcula con Rodrigues:

```text
R = cI + (1 - c)uuT + s[u]x
```

donde:

```text
c = cos(theta)
s = sin(theta)
```

Si la linea no pasa por el origen, la matriz final es:

```text
M = T(A) * R * T(-A)
```

## Actividad 1

Cubo rotado `30 grados` alrededor de:

```text
A = [0, 0, 0]
B = [7, 7, 7]
```

Como la recta pasa por el origen:

```text
M = R
u = normalize(7, 7, 7) = (1/sqrt(3), 1/sqrt(3), 1/sqrt(3))
```

El programa muestra el cubo original en modo alambre y el cubo rotado con caras coloreadas.

## Actividad 2

Tetraedro rotado `45 grados` alrededor de la recta que pasa por:

```text
P = [2, 1, 0]
Q = [6, 5, 0]
```

El vector director es:

```text
Q - P = [4, 4, 0]
u = normalize(4, 4, 0) = (1/sqrt(2), 1/sqrt(2), 0)
```

La transformacion requerida es:

```text
M = T(2, 1, 0) * R(u, 45 grados) * T(-2, -1, 0)
```

El programa muestra el tetraedro original en modo alambre y el tetraedro rotado con caras coloreadas.
