// ============================================================
//  Practica 08 - ACTIVIDAD 1
//  UNSAAC - Computacion Grafica I
//
//  "Escribir un programa que refleje un poligono de 5 lados
//   con respecto al eje X."
//
//  Aplicacion directa del PDF:
//    createScaleMatrix(mat, 1.0f, -1.0f)  →  Reflexion eje X
// ============================================================

#include "gl_common.hpp"
#include <iostream>

// -------------------------------------------------------
//  Shaders (identicos al PDF)
// -------------------------------------------------------
const char* vertSrc = R"(
#version 330 core
layout ( location = 0) in vec2 aPos ;
uniform mat4 transform ;

void main () {
    gl_Position = transform * vec4 (aPos , 0.0 , 1.0) ;
}
)";

const char* fragSrc = R"(
#version 330 core
out vec4 FragColor ;
uniform vec3 objectColor ;

void main () {
    FragColor = vec4 ( objectColor , 1.0) ;
}
)";

// -------------------------------------------------------
//  VAO / VBO  – pentagono regular de 5 lados
// -------------------------------------------------------
GLuint shaderProgram, vao, vbo;
static const int N = 5; // numero de lados

void inicializarGeometria()
{
    // Calcular los 5 vertices del pentagono centrado en el origen
    // El primer vertice apunta hacia arriba (angulo 90 grados = PI/2)
    float vertices[N * 2];
    for (int i = 0; i < N; ++i) {
        float angulo = (float)M_PI / 2.0f + (2.0f * (float)M_PI * i) / N;
        vertices[i * 2]     = 0.5f * cosf(angulo);  // x
        vertices[i * 2 + 1] = 0.5f * sinf(angulo);  // y
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

// -------------------------------------------------------
//  main
// -------------------------------------------------------
int main()
{
    configureGlfwForX11();
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(700, 700,
        "P08 - Actividad 1: Pentagono - Reflexion eje X", NULL, NULL);
    if (!window) {
        std::cerr << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error al inicializar GLEW" << std::endl;
        return -1;
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    shaderProgram = compileShaderProgram(vertSrc, fragSrc);
    inicializarGeometria();

    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    unsigned int colorLoc     = glGetUniformLocation(shaderProgram, "objectColor");

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        // --- Pentagono ORIGINAL ---
        float matIdentidad[16];
        createIdentityMatrix(matIdentidad);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, matIdentidad);
        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // blanco
        glDrawArrays(GL_LINE_LOOP, 0, N);

        // --- Reflexion con respecto al eje X (Sy = -1) ---
        // Aplicacion directa del PDF seccion 3.2
        float finalReflectXMatrix[16];
        createScaleMatrix(finalReflectXMatrix, 1.0f, -1.0f);

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, finalReflectXMatrix);
        glUniform3f(colorLoc, 0.2f, 1.0f, 0.4f); // verde
        glDrawArrays(GL_LINE_LOOP, 0, N);

        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
