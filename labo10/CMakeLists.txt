cmake_minimum_required(VERSION 3.10)
project(Labo10Transformaciones3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(actCubo actCubo.cpp)
add_executable(actTetra actTetra.cpp)

target_link_libraries(actCubo PRIVATE OpenGL::GL GLEW::GLEW glfw)
target_link_libraries(actTetra PRIVATE OpenGL::GL GLEW::GLEW glfw)
