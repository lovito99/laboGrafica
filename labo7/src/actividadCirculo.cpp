#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>

// -------------------------------------------------------
// Fuentes de los shaders (en crudo como cadenas)
// -------------------------------------------------------
const char* vertSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat3 uTransform;
void main() {
    vec3 p = uTransform * vec3(aPos, 1.0);
    gl_Position = vec4(p.xy, 0.0, 1.0);
}
)";

const char* fragSrc = R"(
#version 330 core
uniform vec4 uColor;
out vec4 fragColor;
void main() {
    fragColor = uColor;
}
)";

// -------------------------------------------------------
// Variables globales
// -------------------------------------------------------
GLuint shaderProg, vao, vbo;
int numVerticesCirculo = 0;

// Centro y radio del circulo original
const float CENTRO_X = 0.0f;
const float CENTRO_Y = 0.0f;
const float RADIO    = 0.3f;

// Funcion auxiliar para revisar errores de compilacion
void chequearErroresShader(GLuint shader, std::string tipo) {
    GLint success;
    GLchar infoLog[1024];
    if (tipo != "PROGRAMA") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR del tipo: " <<
                tipo << "\n" << infoLog << "\n";
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR del tipo: " <<
                tipo << "\n" << infoLog << "\n";
        }
    }
}

// Funcion auxiliar: compilar y enlazar shaders
GLuint compilarShaders(const char* vs, const char* fs)
{
    GLuint vid = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vid, 1, &vs, nullptr);
    glCompileShader(vid);
    chequearErroresShader(vid, "VERTEX");

    GLuint fid = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fid, 1, &fs, nullptr);
    glCompileShader(fid);
    chequearErroresShader(fid, "FRAGMENT");

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vid);
    glAttachShader(prog, fid);
    glLinkProgram(prog);
    chequearErroresShader(prog, "PROGRAMA");

    glDeleteShader(vid);
    glDeleteShader(fid);
    return prog;
}

// Construir matriz identidad 3x3 (column-major)
void matrizIdentidad(float M[9])
{
    M[0] = 1.0f; M[3] = 0.0f; M[6] = 0.0f;
    M[1] = 0.0f; M[4] = 1.0f; M[7] = 0.0f;
    M[2] = 0.0f; M[5] = 0.0f; M[8] = 1.0f;
}

// Construir matriz de escalamiento alrededor de un punto (cx, cy)
void matrizEscalamientoAlrededorDe(float sx, float sy, float cx, float
    cy, float M[9])
{
    // Matriz: T(cx,cy) * S(sx,sy) * T(-cx,-cy)
    // p' = S*(p - centro) + centro = S*p + (centro - S*centro)
    float tx = cx - (sx * cx);
    float ty = cy - (sy * cy);

    M[0] = sx;   M[3] = 0.0f; M[6] = tx;
    M[1] = 0.0f; M[4] = sy;   M[7] = ty;
    M[2] = 0.0f; M[5] = 0.0f; M[8] = 1.0f;
}

// -------------------------------------------------------
// Inicializacion de VAO/VBO con los vertices del circulo
// (GL_TRIANGLE_FAN: vertice central + N+1 vertices del perimetro)
// -------------------------------------------------------
void inicializarGeometria()
{
    const int segmentos = 64;
    std::vector<float> vertices;

    // Vertice central (para GL_TRIANGLE_FAN)
    vertices.push_back(CENTRO_X);
    vertices.push_back(CENTRO_Y);

    // Vertices del perimetro
    for (int i = 0; i <= segmentos; i++) {
        float angulo = 2.0f * M_PI * i / segmentos;
        vertices.push_back(CENTRO_X + RADIO * cos(angulo));
        vertices.push_back(CENTRO_Y + RADIO * sin(angulo));
    }

    numVerticesCirculo = (int)(vertices.size() / 2);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
        vertices.data(), GL_STATIC_DRAW);

    // Atributo 0: vec2 aPos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// -------------------------------------------------------
// Programa principal
// -------------------------------------------------------
int main()
{
    // 1. Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    // Configurar GLFW para usar OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Crear la ventana
    GLFWwindow* window = glfwCreateWindow(600, 600,
        "Actividad 1 - Escalamiento de circulo", NULL, NULL);
    if (!window) {
        std::cerr << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 3. Inicializar GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error al inicializar GLEW" << std::endl;
        return -1;
    }

    // Configuracion inicial de OpenGL
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f); // fondo gris oscuro
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 4. Compilar shaders y cargar geometria
    shaderProg = compilarShaders(vertSrc, fragSrc);
    inicializarGeometria();

    // Ubicaciones de los uniforms
    GLint locMat   = glGetUniformLocation(shaderProg, "uTransform");
    GLint locColor = glGetUniformLocation(shaderProg, "uColor");

    float M[9];

    std::cout << "=== Actividad 1: Escalar circulo x2 manteniendo centro fijo ===" << std::endl;
    std::cout << "Blanco: circulo original  (radio " << RADIO << ")" << std::endl;
    std::cout << "Verde:  circulo escalado x2 con centro fijo en ("
              << CENTRO_X << ", " << CENTRO_Y << ")" << std::endl;
    std::cout << "ESC: Salir" << std::endl;

    // 5. Bucle principal de renderizado (Render Loop)
    while (!glfwWindowShouldClose(window))
    {
        // Procesar entradas
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Limpiar el buffer de color
        glClear(GL_COLOR_BUFFER_BIT);

        // Activar el programa shader
        glUseProgram(shaderProg);

        glBindVertexArray(vao);

        // --- Dibujar circulo ORIGINAL en blanco ---
        matrizIdentidad(M);
        glUniformMatrix3fv(locMat, 1, GL_FALSE, M);
        glUniform4f(locColor, 1.0f, 1.0f, 1.0f, 0.6f); // blanco
        glDrawArrays(GL_TRIANGLE_FAN, 0, numVerticesCirculo);

        // --- Dibujar circulo ESCALADO x2 manteniendo fijo el centro ---
        matrizEscalamientoAlrededorDe(2.0f, 2.0f, CENTRO_X, CENTRO_Y, M);
        glUniformMatrix3fv(locMat, 1, GL_FALSE, M);
        glUniform4f(locColor, 0.0f, 1.0f, 0.0f, 0.5f); // verde
        glDrawArrays(GL_TRIANGLE_FAN, 0, numVerticesCirculo);

        glBindVertexArray(0);

        // Intercambiar buffers y procesar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 6. Limpieza de memoria al salir
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProg);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
