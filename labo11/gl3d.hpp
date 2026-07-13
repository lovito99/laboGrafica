#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>

inline void configureGlfwForX11()
{
#if defined(GLFW_PLATFORM) && defined(GLFW_PLATFORM_X11)
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif
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

inline void multiplyMatrices(const float* a, const float* b, float* result)
{
    float temp[16];

    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            temp[col * 4 + row] =
                a[0 * 4 + row] * b[col * 4 + 0] +
                a[1 * 4 + row] * b[col * 4 + 1] +
                a[2 * 4 + row] * b[col * 4 + 2] +
                a[3 * 4 + row] * b[col * 4 + 3];
        }
    }

    for (int i = 0; i < 16; ++i) {
        result[i] = temp[i];
    }
}

inline void createTranslationMatrix(float* mat, float tx, float ty, float tz)
{
    createIdentityMatrix(mat);
    mat[12] = tx;
    mat[13] = ty;
    mat[14] = tz;
}

inline void createReflectionPlaneThroughOrigin(float* mat, float a, float b, float c)
{
    createIdentityMatrix(mat);

    const float denominator = a * a + b * b + c * c;
    if (denominator == 0.0f) {
        return;
    }

    const float n[3] = {a, b, c};
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            const float identity = row == col ? 1.0f : 0.0f;
            mat[col * 4 + row] = identity - (2.0f * n[row] * n[col]) / denominator;
        }
    }
}

inline void createReflectionZEquals(float* mat, float zPlane)
{
    createIdentityMatrix(mat);
    mat[10] = -1.0f;
    mat[14] = 2.0f * zPlane;
}

inline void createShearing3D(float* mat, float shxy, float shxz, float shyx, float shyz, float shzx, float shzy)
{
    createIdentityMatrix(mat);
    mat[4] = shxy;
    mat[8] = shxz;
    mat[1] = shyx;
    mat[9] = shyz;
    mat[2] = shzx;
    mat[6] = shzy;
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
