cmake_minimum_required(VERSION 3.10)
project(Labo12CurvasBezier3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(mainCurves mainCurves.cpp)
add_executable(actividadBezierCompuesta actividadBezierCompuesta.cpp)

target_link_libraries(mainCurves PRIVATE OpenGL::GL GLEW::GLEW glfw)
target_link_libraries(actividadBezierCompuesta PRIVATE OpenGL::GL GLEW::GLEW glfw)
