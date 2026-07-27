#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

inline const char* curveVertexShader = R"glsl(
#version 330 core
layout (location = 0) in float aT;
uniform vec3 P0; uniform vec3 P1; uniform vec3 P2; uniform vec3 P3;
uniform mat4 model; uniform mat4 view; uniform mat4 projection;
out float tVector;
void main() {
    float t = aT, u = 1.0 - t;
    vec3 pos = (u*u*u)*P0 + (3.0*u*u*t)*P1
             + (3.0*u*t*t)*P2 + (t*t*t)*P3;
    gl_Position = projection * view * model * vec4(pos, 1.0);
    tVector = t;
}
)glsl";

inline const char* curveFragmentShader = R"glsl(
#version 330 core
in float tVector;
out vec4 FragColor;
uniform vec3 startColor;
uniform vec3 endColor;
void main() {
    FragColor = vec4(mix(startColor, endColor, tVector), 1.0);
}
)glsl";

inline GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Error de shader: " << log << '\n';
    }
    return shader;
}

inline GLuint createCurveProgram()
{
    GLuint vs = compileShader(GL_VERTEX_SHADER, curveVertexShader);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, curveFragmentShader);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Error de enlace: " << log << '\n';
    }
    return program;
}

inline GLFWwindow* createBezierWindow(const char* title)
{
    if (!glfwInit()) return nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(900, 700, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }
    glEnable(GL_DEPTH_TEST);
    glLineWidth(3.0f);
    return window;
}

inline GLuint createParameterBuffer(GLuint& vao, int segments)
{
    std::vector<float> values(segments + 1);
    for (int i = 0; i <= segments; ++i)
        values[i] = static_cast<float>(i) / static_cast<float>(segments);

    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, values.size() * sizeof(float), values.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return vbo;
}

inline void setMatrices(GLuint program, float time)
{
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * 0.18f, glm::vec3(0, 1, 0));
    glm::mat4 view = glm::lookAt(glm::vec3(0, 5, 32), glm::vec3(0), glm::vec3(0, 1, 0));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 900.0f / 700.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

inline void drawBezier(GLuint program, GLuint vao, int count,
                       const glm::vec3& p0, const glm::vec3& p1,
                       const glm::vec3& p2, const glm::vec3& p3,
                       const glm::vec3& color0, const glm::vec3& color1)
{
    glUniform3fv(glGetUniformLocation(program, "P0"), 1, glm::value_ptr(p0));
    glUniform3fv(glGetUniformLocation(program, "P1"), 1, glm::value_ptr(p1));
    glUniform3fv(glGetUniformLocation(program, "P2"), 1, glm::value_ptr(p2));
    glUniform3fv(glGetUniformLocation(program, "P3"), 1, glm::value_ptr(p3));
    glUniform3fv(glGetUniformLocation(program, "startColor"), 1, glm::value_ptr(color0));
    glUniform3fv(glGetUniformLocation(program, "endColor"), 1, glm::value_ptr(color1));
    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_STRIP, 0, count);
}
