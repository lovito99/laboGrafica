cmake_minimum_required(VERSION 3.10)
project(Labo7Transformaciones2D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# Listado 1 del PDF: Escalamiento 2D
add_executable(escalamiento src/escalamiento.cpp)

# Listado 2 del PDF: Rotacion 2D
add_executable(rotacion src/rotacion.cpp)

# Actividad 1: Escalar un circulo x2 con centro fijo
add_executable(actividadCirculo src/actividadCirculo.cpp)

# Actividad 2: Trasladar y luego rotar un rectangulo
add_executable(actividadRectangulo src/actividadRectangulo.cpp)

target_link_libraries(escalamiento       PRIVATE OpenGL::GL GLEW::GLEW glfw)
target_link_libraries(rotacion           PRIVATE OpenGL::GL GLEW::GLEW glfw)
target_link_libraries(actividadCirculo   PRIVATE OpenGL::GL GLEW::GLEW glfw)
target_link_libraries(actividadRectangulo PRIVATE OpenGL::GL GLEW::GLEW glfw)
