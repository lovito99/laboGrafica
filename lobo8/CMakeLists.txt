cmake_minimum_required(VERSION 3.10)
project(Labo8Reflexion2D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# Ejemplo del PDF (Seccion 3): Escalamiento con punto fijo + Reflexion X + Reflexion Y
add_executable(reflexion src/reflexion.cpp)

# Actividad 1: Poligono de 5 lados reflejado en eje X
add_executable(actividadPentagono src/actividadPentagono.cpp)

# Actividad 2: Reflexion general 2D de un triangulo
add_executable(actividadTriangulo src/actividadTriangulo.cpp)

target_link_libraries(reflexion          PRIVATE OpenGL::GL GLEW::GLEW glfw)
target_link_libraries(actividadPentagono PRIVATE OpenGL::GL GLEW::GLEW glfw)
target_link_libraries(actividadTriangulo PRIVATE OpenGL::GL GLEW::GLEW glfw)
