// ============================================================
//  Practica 09 - TRANSFORMACION 2D: SHEARING
//  UNSAAC - Computacion Grafica I
//
//  Tarea propuesta: letra E deformada con shx = 2 y shy = 3.
// ============================================================

#include "glcommon.hpp"

#include <iostream>

const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;
uniform mat4 uTransform;

void main()
{
    gl_Position = uTransform * vec4(aPos, 0.0, 1.0);
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;
uniform vec4 uColor;

void main()
{
    FragColor = uColor;
}
)glsl";

GLuint shaderProgram = 0;
GLuint vao = 0;
GLuint vbo = 0;
GLuint fillEbo = 0;
GLuint contourEbo = 0;

constexpr GLsizei fillIndexCount = 24;
constexpr GLsizei contourIndexCount = 12;

void createShearingMatrix(float* mat, float shx, float shy)
{
    createIdentityMatrix(mat);
    mat[4] = shx;
    mat[1] = shy;
}

void createScaleMatrix2D(float* mat, float sx, float sy)
{
    createIdentityMatrix(mat);
    mat[0] = sx;
    mat[5] = sy;
}

void setupGeometry()
{
    // Poligono ortogonal de la letra E, definido en sentido antihorario.
    const float vertices[] = {
        -0.45f, -0.50f, // 0
         0.40f, -0.50f, // 1
         0.40f, -0.30f, // 2
        -0.15f, -0.30f, // 3
        -0.15f, -0.10f, // 4
         0.25f, -0.10f, // 5
         0.25f,  0.10f, // 6
        -0.15f,  0.10f, // 7
        -0.15f,  0.30f, // 8
         0.40f,  0.30f, // 9
         0.40f,  0.50f, // 10
        -0.45f,  0.50f  // 11
    };

    // Triangulacion manual por franjas rectangulares: columna, barra inferior,
    // barra media y barra superior.
    const unsigned int fillIndices[] = {
        0, 1, 2,   0, 2, 3,
        0, 3, 8,   0, 8, 11,
        4, 5, 6,   4, 6, 7,
        8, 9, 10,  8, 10, 11
    };

    const unsigned int contourIndices[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &fillEbo);
    glGenBuffers(1, &contourEbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fillEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fillIndices), fillIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, contourEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(contourIndices), contourIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void drawLetterE(GLint transformLoc, GLint colorLoc)
{
    float shear[16];
    float viewportScale[16];
    float transform[16];

    createShearingMatrix(shear, 2.0f, 3.0f);
    createScaleMatrix2D(viewportScale, 0.18f, 0.18f);
    multiplyMatrices(viewportScale, shear, transform);

    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform);

    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fillEbo);
    glUniform4f(colorLoc, 0.12f, 0.35f, 0.82f, 1.0f);
    glDrawElements(GL_TRIANGLES, fillIndexCount, GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, contourEbo);
    glLineWidth(2.5f);
    glUniform4f(colorLoc, 0.02f, 0.04f, 0.08f, 1.0f);
    glDrawElements(GL_LINE_LOOP, contourIndexCount, GL_UNSIGNED_INT, 0);
}

int main()
{
    configureGlfwForX11();
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "P09 - Letra E con Shearing", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error al inicializar GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    shaderProgram = compileShaderProgram(vertexShaderSource, fragmentShaderSource);
    setupGeometry();

    const GLint transformLoc = glGetUniformLocation(shaderProgram, "uTransform");
    const GLint colorLoc = glGetUniformLocation(shaderProgram, "uColor");

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        glClearColor(0.96f, 0.97f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        drawLetterE(transformLoc, colorLoc);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &fillEbo);
    glDeleteBuffers(1, &contourEbo);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
