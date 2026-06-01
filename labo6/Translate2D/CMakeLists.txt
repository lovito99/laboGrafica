cmake_minimum_required(VERSION 3.10)
project(TranslateOpenGL LANGUAGES CXX)

# Usar el estándar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar paquetes necesarios (OpenGL, GLEW, GLFW)
set(OpenGL_GL_PREFERENCE GLVND) 

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# Crear el ejecutable apuntando al archivo fuente
add_executable(translate_app src/main.cpp)

# Si decides cargar los shaders desde archivo, esto permite a main.cpp
# encontrar la carpeta "shaders/" fácilmente pasándole la ruta absoluta
target_compile_definitions(translate_app PRIVATE 
    SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders/"
)

# Vincular las librerías al ejecutable
target_link_libraries(translate_app PRIVATE
    OpenGL::GL
    GLEW::GLEW
    glfw
)
