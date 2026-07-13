#pragma once

#include "gl3d.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr int anchoVent = 800;
constexpr int altoVent = 700;

constexpr GLsizei cantAristasPir = 12;

const char* const shaderVert = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 transformMatrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * transformMatrix * vec4(aPos, 1.0);
}
)glsl";

const char* const shaderFrag = R"glsl(
#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;

void main()
{
    FragColor = vec4(objectColor, 1.0);
}
)glsl";

struct BuffEscena {
    GLuint axesVao = 0;
    GLuint axesVbo = 0;
    GLuint pyramidVao = 0;
    GLuint pyramidVbo = 0;
    GLuint pyramidEbo = 0;
};

inline GLFWwindow* createWindow(const char* title)
{
    configureGlfwForX11();
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(anchoVent, altoVent, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error al inicializar GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    glEnable(GL_DEPTH_TEST);
    return window;
}

inline void setupSceneGeometry(BuffEscena* buffers)
{
    const float axesVertices[] = {
        0.0f, 0.0f, 0.0f, 120.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 120.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 120.0f
    };

    const float pyramidVertices[] = {
        40.0f, 20.0f, 40.0f,
        60.0f, 30.0f, 20.0f,
        40.0f, 50.0f, 30.0f,
        20.0f, 30.0f, 20.0f
    };

    const unsigned int pyramidEdges[] = {
        0, 1, 1, 2, 2, 0,
        0, 3, 1, 3, 2, 3
    };

    glGenVertexArrays(1, &buffers->axesVao);
    glGenBuffers(1, &buffers->axesVbo);
    glBindVertexArray(buffers->axesVao);
    glBindBuffer(GL_ARRAY_BUFFER, buffers->axesVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axesVertices), axesVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &buffers->pyramidVao);
    glGenBuffers(1, &buffers->pyramidVbo);
    glGenBuffers(1, &buffers->pyramidEbo);
    glBindVertexArray(buffers->pyramidVao);
    glBindBuffer(GL_ARRAY_BUFFER, buffers->pyramidVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->pyramidEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidEdges), pyramidEdges, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

inline void drawAxes(const BuffEscena& buffers, GLint transformLoc, GLint colorLoc)
{
    float identity[16];
    createIdentityMatrix(identity);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, identity);

    glBindVertexArray(buffers.axesVao);
    glLineWidth(2.0f);
    glUniform3f(colorLoc, 0.85f, 0.10f, 0.10f);
    glDrawArrays(GL_LINES, 0, 2);
    glUniform3f(colorLoc, 0.10f, 0.60f, 0.18f);
    glDrawArrays(GL_LINES, 2, 2);
    glUniform3f(colorLoc, 0.10f, 0.22f, 0.85f);
    glDrawArrays(GL_LINES, 4, 2);
}

inline void drawPyramid(const BuffEscena& buffers, GLint transformLoc, GLint colorLoc, const float* transform, float r, float g, float b)
{
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform);
    glUniform3f(colorLoc, r, g, b);
    glBindVertexArray(buffers.pyramidVao);
    glLineWidth(3.0f);
    glDrawElements(GL_LINES, cantAristasPir, GL_UNSIGNED_INT, 0);
}

inline void sendCamera(GLuint program)
{
    const glm::mat4 projection = glm::ortho(-120.0f, 120.0f, -100.0f, 130.0f, -250.0f, 250.0f);
    const glm::mat4 view = glm::lookAt(
        glm::vec3(95.0f, 80.0f, 145.0f),
        glm::vec3(25.0f, 25.0f, 25.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
}

inline void destroyScene(GLFWwindow* window, GLuint program, BuffEscena* buffers)
{
    glDeleteVertexArrays(1, &buffers->axesVao);
    glDeleteBuffers(1, &buffers->axesVbo);
    glDeleteVertexArrays(1, &buffers->pyramidVao);
    glDeleteBuffers(1, &buffers->pyramidVbo);
    glDeleteBuffers(1, &buffers->pyramidEbo);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
}
