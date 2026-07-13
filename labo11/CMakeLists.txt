cmake_minimum_required(VERSION 3.10)
project(Labo11Transformaciones3D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(actARefPlano actARefPlano.cpp)
add_executable(actBRefZ5 actBRefZ5.cpp)
add_executable(actCShearDyn actCShearDyn.cpp)

target_link_libraries(actARefPlano PRIVATE OpenGL::GL GLEW::GLEW glfw)
target_link_libraries(actBRefZ5 PRIVATE OpenGL::GL GLEW::GLEW glfw)
target_link_libraries(actCShearDyn PRIVATE OpenGL::GL GLEW::GLEW glfw)
