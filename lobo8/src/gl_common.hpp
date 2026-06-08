#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>

inline void configureGlfwForX11()
{
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
}

inline void createIdentityMatrix(float* mat)
{
    for (int i = 0; i < 16; ++i) {
        mat[i] = 0.0f;
    }
    mat[0] = 1.0f;
    mat[5] = 1.0f;
    mat[10] = 1.0f;
    mat[15] = 1.0f;
}

inline void createTranslationMatrix(float* mat, float tx, float ty)
{
    createIdentityMatrix(mat);
    mat[12] = tx;
    mat[13] = ty;
}

inline void createScaleMatrix(float* mat, float sx, float sy)
{
    createIdentityMatrix(mat);
    mat[0] = sx;
    mat[5] = sy;
}

inline void multiplyMatrices(const float* a, const float* b, float* result)
{
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            result[col * 4 + row] =
                a[0 * 4 + row] * b[col * 4 + 0] +
                a[1 * 4 + row] * b[col * 4 + 1] +
                a[2 * 4 + row] * b[col * 4 + 2] +
                a[3 * 4 + row] * b[col * 4 + 3];
        }
    }
}

inline void createRotationMatrix(float* mat, float theta)
{
    createIdentityMatrix(mat);
    const float cosine = cosf(theta);
    const float sine = sinf(theta);
    mat[0] = cosine;
    mat[1] = sine;
    mat[4] = -sine;
    mat[5] = cosine;
}

inline GLuint compileShaderProgram(const char* vertSrc, const char* fragSrc)
{
    const auto checkShader = [](GLuint shader, const char* label) {
        GLint status = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_TRUE) {
            return;
        }

        char log[1024];
        GLsizei length = 0;
        glGetShaderInfoLog(shader, sizeof(log), &length, log);
        std::cerr << "Error al compilar " << label << ": " << log << std::endl;
    };

    const auto checkProgram = [](GLuint program) {
        GLint status = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status == GL_TRUE) {
            return;
        }

        char log[1024];
        GLsizei length = 0;
        glGetProgramInfoLog(program, sizeof(log), &length, log);
        std::cerr << "Error al enlazar programa: " << log << std::endl;
    };

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertSrc, nullptr);
    glCompileShader(vs);
    checkShader(vs, "vertex shader");

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragSrc, nullptr);
    glCompileShader(fs);
    checkShader(fs, "fragment shader");

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    checkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}