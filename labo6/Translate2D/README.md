# Práctica N° 06: Traslación 2D

Este repositorio contiene la implementación de la transformación geométrica de traslación en 2D utilizando **C++** y **OpenGL**.

## 1. Objetivo
* Comprender los fundamentos matemáticos de la transformación de traslación 2D.
* Expresar la traslación mediante coordenadas homogéneas y matrices 3x3.
* Implementar la traslación en el *pipeline programable* de OpenGL moderno
        usando _Vertex Shader_ y _Fragment Shader_ escritos en GLSL.
* Comparar el enfoque clásico (OpenGL fijo) con el enfoque moderno (shaders).

## 2. Traslación 2D

La \textbf{traslación} es una transformación rígida que desplaza cada punto del objeto
la misma distancia en la misma dirección, sin producir deformación.

Dado un punto $P_1 = (x_1,y_1)$, su imagen tras una traslación con vector
$\mathbf{t}=(t_x,t_y)$ es:


$$P_2 = P_1 + \mathbf{t} \quad\Longrightarrow\quad \begin{cases} x_2 = x_1 + t_x \\ y_2 = y_1 + t_y \end{cases}$$

1. **Configurar el proyecto:**
   ```bash
   cd Translate2D
   mkdir -p build && cd build
   cmake ..
   ```

2. **Compilar:**
   ```bash
   cmake --build .
   ```
 
3. **Ejecutar:**
   ```bash
   ./translate_app
   ```

Si estás ubicado en la raíz del workspace [laboGrafica](.), también puedes compilar así:

```bash
cmake -S Translate2D -B Translate2D/build
cmake --build Translate2D/build
./Translate2D/build/translate_app
```

Si por error ya entraste a [build](build) en la raíz del workspace, primero regresa un nivel:

```bash
cd ..
cmake -S Translate2D -B Translate2D/build
cmake --build Translate2D/build
./Translate2D/build/translate_app
```
