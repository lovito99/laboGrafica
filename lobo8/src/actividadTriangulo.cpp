// ============================================================
//  Practica 08 - ACTIVIDAD 2
//  UNSAAC - Computacion Grafica I
//
//  "Escribir un programa para realizar reflexion general
//   en 2D de un triangulo."
//
//  La reflexion general 2D refleja el triangulo respecto
//  a una linea arbitraria que pasa por el origen.
//
//  Metodo (usando las funciones del PDF):
//    Reflexion respecto a la linea Y = X  (theta = 45 grados):
//      1. Rotar -45 grados  (alinear la linea con eje X)
//      2. Reflexion en eje X: createScaleMatrix(1, -1)
//      3. Rotar +45 grados  (restaurar)
//
//    La matriz resultante es:
//      | 0  1  0  0 |
//      | 1  0  0  0 |
//      | 0  0  1  0 |
//      | 0  0  0  1 |
// ============================================================

#include "gl_common.hpp"
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

void reflexionGeneral(float* result, float theta)
{
    float Rpos[16], RefX[16], Rneg[16], tmp[16];

    createRotationMatrix(Rpos,  theta);   // R(+theta)
    createScaleMatrix(RefX, 1.0f, -1.0f); // Reflexion eje X
    createRotationMatrix(Rneg, -theta);   // R(-theta)

    // P' = R(+theta) * RefX * R(-theta) * P
    multiplyMatrices(RefX,  Rneg, tmp);    // RefX * R(-theta)
    multiplyMatrices(Rpos,  tmp,  result); // R(+theta) * ...
}

// -------------------------------------------------------
//  VAO / VBO – triangulo
// -------------------------------------------------------
GLuint shaderProgram, vao, vbo;

void inicializarGeometria()
{
    float vertices[] = {
         0.05f,  0.50f,  // v0 - superior
         0.55f, -0.30f,  // v1 - inferior derecho
        -0.45f, -0.30f   // v2 - inferior izquierdo
    };

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
        "P08 - Actividad 2: Reflexion General 2D", NULL, NULL);
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

    // Angulo de la linea de reflexion: 45 grados (linea Y = X)
    float theta = (float)M_PI / 4.0f;

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        // --- Triangulo ORIGINAL ---
        float matIdentidad[16];
        createIdentityMatrix(matIdentidad);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, matIdentidad);
        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // blanco
        glDrawArrays(GL_LINE_LOOP, 0, 3);

        // --- Reflexion GENERAL respecto a la linea Y = X (theta = 45 grados) ---
        float matReflexion[16];
        reflexionGeneral(matReflexion, theta);

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, matReflexion);
        glUniform3f(colorLoc, 1.0f, 0.2f, 0.8f); // magenta
        glDrawArrays(GL_LINE_LOOP, 0, 3);

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
