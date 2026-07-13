# Practica 11 - Transformaciones Avanzadas en 3D
**UNSAAC - Computacion Grafica I**

## Estructura

```text
labo11/
├── CMakeLists.txt
├── CG1-P11.pdf
├── README.md
├── gl3d.hpp
├── comun3d.hpp
├── actARefPlano.cpp
├── actBRefZ5.cpp
└── actCShearDyn.cpp
```

## Compilar

```bash
cd ~/Documentos/GitHub/laboGrafica/labo11
mkdir -p build && cd build
cmake .. -G "Unix Makefiles"
make
```

## Ejecutar

```bash
./build/actARefPlano
./build/actBRefZ5
./build/actCShearDyn
```

Presione `ESC` para cerrar cada ventana.

## A) Reflexion sobre 3x + 5y + 6z = 0

El plano pasa por el origen y tiene normal:

```text
n = (3, 5, 6)
```

La matriz de reflexion de un punto `p` respecto a un plano con normal `n` es:

```text
p' = p - 2 * n * (n . p) / (n . n)
R = I - 2 * n * n^T / (n . n)
```

Como:

```text
n . n = 3^2 + 5^2 + 6^2 = 70
```

se implementa:

```text
R =
|  1 - 18/70   -30/70       -36/70       0 |
|  -30/70       1 - 50/70   -60/70       0 |
|  -36/70      -60/70        1 - 72/70   0 |
|   0            0            0           1 |
```

El codigo lo calcula en `createReflectionPlaneThroughOrigin(...)`.

La parte de camara y proyeccion usa GLM, igual que la plantilla del PDF:

```cpp
glm::ortho(...)
glm::lookAt(...)
glm::value_ptr(...)
```

## B) Reflexion estricta sobre Z = 5

Para reflejar respecto al plano infinito `Z = 5`, se traslada el plano al origen, se refleja en `Z = 0` y se retorna:

```text
M = T(0, 0, 5) * Rxy * T(0, 0, -5)
```

Resultado:

```text
x' = x
y' = y
z' = 10 - z
```

Matriz homogenea:

```text
| 1  0   0   0  |
| 0  1   0   0  |
| 0  0  -1   10 |
| 0  0   0   1  |
```

El termino de traslacion acoplado es `2 * 5 = 10`.

## C) Distorsion dinamica cruzada

Sobre la piramide reflejada en `Z = 5`, se aplica una matriz de cizallamiento 3D:

```text
Sh =
| 1     shxy  0     0 |
| 0     1     shyz  0 |
| 0     0     1     0 |
| 0     0     0     1 |
```

con:

```text
shxy = sin(tiempo)
shyz = cos(tiempo)
```

Por tanto:

```text
x' = x + sin(tiempo) * y
y' = y + cos(tiempo) * z
z' = z
```

La transformacion final del ejecutable C es:

```text
Mfinal = Sh(tiempo) * ReflexionZ5
```
