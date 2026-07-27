# Practica 12 - Curvas de Bezier 3D

Implementacion del codigo base y del ejercicio propuesto de una curva de Bezier
compuesta con continuidad C1, evaluada en el vertex shader.

## Archivos solicitados

```text
labo12/
├── curve.vert                     # Ejemplo 3.1 del PDF
├── curve.frag                     # Ejemplo 3.2 del PDF
├── mainCurves.cpp                 # Ejemplo base 3.3
├── actividadBezierCompuesta.cpp   # Actividades 1, 2 y 3
├── bezierComun.hpp                # Funciones OpenGL reutilizables
├── capturaCurvaBase.png
├── capturaBezierCompuesta.png
└── informe/
    ├── informe.tex
    └── informe.pdf
```

## Compilar

```bash
cd labo12
cmake -S . -B build
cmake --build build
```

## Ejecutar

```bash
./build/mainCurves
./build/actividadBezierCompuesta
```

En la actividad compuesta:

- `1`: seleccionar el extremo `A0`.
- `2`: seleccionar el extremo `B3`.
- Flechas: desplazar el nodo seleccionado en X/Y.
- `W` y `S`: desplazarlo en Z.
- `ESC`: cerrar.

Cada segmento recibe los mismos 241 valores de `t` entre 0 y 1. La GPU evalua
los polinomios de Bernstein. En cada cuadro se calculan `B0=A3` y
`B1=A3+(A3-A2)`, por lo que las derivadas en la union son iguales.

## Informe

```bash
cd informe
pdflatex informe.tex
pdflatex informe.tex
```
