# Práctica 09 - Transformación 2D: Shearing
**UNSAAC - Computación Gráfica I**

## Estructura del proyecto

```text
labo9/
├── CMakeLists.txt
├── P09.pdf
├── README.md
├── glcommon.hpp
├── shearing.cpp          <- Ejemplo base del PDF
├── actividadLetraE.cpp   <- Tarea propuesta: letra E
├── infome/
│   ├── informe.tex
│   └── informe.pdf
```

## Compilar

```bash
cd ~/Documentos/GitHub/laboGrafica/labo9
mkdir -p build && cd build
cmake .. -G "Unix Makefiles"
make
```

## Ejecutables

### `shearing` - Ejemplo base del PDF

Renderiza un rectángulo y aplica una deformación horizontal mediante una matriz uniforme enviada al vertex shader.

```bash
./build/shearing
```

### `actividadLetraE` - Tarea propuesta

Implementa la figura geométrica de la letra `E` con 12 puntos y la deforma aplicando:

```text
shx = 2
shy = 3
```

La letra se renderiza con triangulación manual usando `GL_TRIANGLES`; además, se dibuja su contorno con `GL_LINE_LOOP`.

```bash
./build/actividadLetraE
```

## Base teórica

La deformación 2D combina las ecuaciones:

```text
x' = x + shx * y
y' = shy * x + y
```

En forma matricial:

```text
| x' |   |  1   shx | | x |
| y' | = | shy   1  | | y |
```

En OpenGL, la matriz se envía en formato column-major:

```cpp
mat[4] = shx;
mat[1] = shy;
```

## Informe

El informe se encuentra en:

```text
infome/informe.tex
infome/informe.pdf
```
