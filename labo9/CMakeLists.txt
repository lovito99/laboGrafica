cmake_minimum_required(VERSION 3.10)
project(Labo9Shearing2D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# Ejemplo base del PDF: deformacion 2D aplicada a un rectangulo.
add_executable(shearing shearing.cpp)

# Tarea propuesta: letra E deformada con shx = 2 y shy = 3.
add_executable(actividadLetraE actividadLetraE.cpp)

target_link_libraries(shearing PRIVATE OpenGL::GL GLEW::GLEW glfw)
target_link_libraries(actividadLetraE PRIVATE OpenGL::GL GLEW::GLEW glfw)
