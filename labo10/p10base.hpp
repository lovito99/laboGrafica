#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <iostream>

constexpr GLuint ancho = 900;
constexpr GLuint alto = 700;

const char* const shaderVert = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = aColor;
}
)glsl";

const char* const shaderFrag = R"glsl(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vertexColor, 1.0);
}
)glsl";

struct Malla {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei cant = 0;
};

inline void configGlfw()
{
#if defined(GLFW_PLATFORM) && defined(GLFW_PLATFORM_X11)
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif
}

inline glm::mat4 matTras(float tx, float ty, float tz)
{
    return glm::mat4(
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(tx, ty, tz, 1.0f));
}

inline glm::mat4 matPersp(float fovRad, float aspecto, float cerca, float lejos)
{
    const float f = 1.0f / std::tan(fovRad / 2.0f);
    return glm::mat4(
        glm::vec4(f / aspecto, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, (lejos + cerca) / (cerca - lejos), -1.0f),
        glm::vec4(0.0f, 0.0f, (2.0f * lejos * cerca) / (cerca - lejos), 0.0f));
}

inline glm::mat4 matRotEje(glm::vec3 eje, float angRad)
{
    eje = glm::normalize(eje);

    const float x = eje.x;
    const float y = eje.y;
    const float z = eje.z;
    const float c = std::cos(angRad);
    const float s = std::sin(angRad);
    const float omc = 1.0f - c;

    return glm::mat4(
        glm::vec4(c + x * x * omc, y * x * omc + z * s, z * x * omc - y * s, 0.0f),
        glm::vec4(x * y * omc - z * s, c + y * y * omc, z * y * omc + x * s, 0.0f),
        glm::vec4(x * z * omc + y * s, y * z * omc - x * s, c + z * z * omc, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

inline glm::mat4 matRotLinea(glm::vec3 a, glm::vec3 b, float angRad)
{
    const glm::vec3 eje = b - a;
    return matTras(a.x, a.y, a.z) * matRotEje(eje, angRad) * matTras(-a.x, -a.y, -a.z);
}

inline void verifShader(GLuint obj, const char* tipo)
{
    GLint ok = GL_FALSE;
    char log[1024];

    if (std::string(tipo) == "PROGRAMA") {
        glGetProgramiv(obj, GL_LINK_STATUS, &ok);
        if (!ok) {
            glGetProgramInfoLog(obj, 1024, nullptr, log);
            std::cerr << "Error al enlazar programa:\n" << log << std::endl;
        }
        return;
    }

    glGetShaderiv(obj, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(obj, 1024, nullptr, log);
        std::cerr << "Error al compilar " << tipo << ":\n" << log << std::endl;
    }
}

inline GLuint compilar()
{
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &shaderVert, nullptr);
    glCompileShader(vert);
    verifShader(vert, "VERTEX");

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &shaderFrag, nullptr);
    glCompileShader(frag);
    verifShader(frag, "FRAGMENT");

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    verifShader(prog, "PROGRAMA");

    glDeleteShader(vert);
    glDeleteShader(frag);
    return prog;
}

inline GLFWwindow* crearVent(const char* titulo)
{
    configGlfw();
    if (!glfwInit()) {
        std::cerr << "Error al iniciar GLFW" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* vent = glfwCreateWindow(ancho, alto, titulo, nullptr, nullptr);
    if (!vent) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(vent);
    glViewport(0, 0, ancho, alto);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(vent);
        glfwTerminate();
        return nullptr;
    }

    glEnable(GL_DEPTH_TEST);
    return vent;
}

inline Malla crearMalla(const float* verts, GLsizeiptr tamVerts, const unsigned int* inds, GLsizeiptr tamInds, GLsizei cantInds)
{
    Malla malla;
    malla.cant = cantInds;

    glGenVertexArrays(1, &malla.vao);
    glGenBuffers(1, &malla.vbo);
    glGenBuffers(1, &malla.ebo);

    glBindVertexArray(malla.vao);
    glBindBuffer(GL_ARRAY_BUFFER, malla.vbo);
    glBufferData(GL_ARRAY_BUFFER, tamVerts, verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, malla.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, tamInds, inds, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    return malla;
}

inline void enviarCam(GLuint prog)
{
    const glm::mat4 view = glm::lookAt(
        glm::vec3(10.0f, 9.0f, 18.0f),
        glm::vec3(1.5f, 1.5f, 1.5f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    const float fov = glm::radians(45.0f);
    const glm::mat4 projection = matPersp(fov, static_cast<float>(ancho) / static_cast<float>(alto), 0.1f, 100.0f);

    glUniformMatrix4fv(glGetUniformLocation(prog, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(prog, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

inline void dibujar(const Malla& malla, GLint modelLoc, const glm::mat4& model, GLenum modo)
{
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(malla.vao);
    glDrawElements(modo, malla.cant, GL_UNSIGNED_INT, 0);
}

inline void limpiar(GLFWwindow* vent, GLuint prog, Malla* malla)
{
    glDeleteVertexArrays(1, &malla->vao);
    glDeleteBuffers(1, &malla->vbo);
    glDeleteBuffers(1, &malla->ebo);
    glDeleteProgram(prog);
    glfwDestroyWindow(vent);
    glfwTerminate();
}
