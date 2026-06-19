// ============================================================
//  Practica 09 - TRANSFORMACION 2D: SHEARING
//  UNSAAC - Computacion Grafica I
//
//  Ejemplo base del PDF: deformacion 2D de un rectangulo.
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
GLuint ebo = 0;

void createShearingMatrix(float* mat, float shx, float shy)
{
    createIdentityMatrix(mat);
    mat[4] = shx;
    mat[1] = shy;
}

void setupGeometry()
{
    const float vertices[] = {
        -0.25f, -0.25f,
         0.25f, -0.25f,
         0.25f,  0.25f,
        -0.25f,  0.25f
    };

    const unsigned int indices[] = {0, 1, 2, 3};

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
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

    GLFWwindow* window = glfwCreateWindow(700, 700, "P09 - Shearing 2D", nullptr, nullptr);
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

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float transform[16];
        createShearingMatrix(transform, 1.5f, 0.0f);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform);
        glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0f);

        glBindVertexArray(vao);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
