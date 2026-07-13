// ============================================================
//  Practica 11 - Actividad C
//  Distorsion dinamica cruzada sobre la piramide reflejada.
//  shyz = cos(tiempo), shxy = sin(tiempo)
// ============================================================

#include "comun3d.hpp"

int main()
{
    GLFWwindow* window = createWindow("P11 C - Distorsion dinamica cruzada");
    if (!window) {
        return -1;
    }

    GLuint program = compileShaderProgram(shaderVert, shaderFrag);
    BuffEscena buffers;
    setupSceneGeometry(&buffers);

    const GLint transformLoc = glGetUniformLocation(program, "transformMatrix");
    const GLint colorLoc = glGetUniformLocation(program, "objectColor");

    float identity[16];
    float reflectionZ5[16];
    createIdentityMatrix(identity);
    createReflectionZEquals(reflectionZ5, 5.0f);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        const float time = static_cast<float>(glfwGetTime());
        float shear[16];
        float dynamicTransform[16];
        createShearing3D(shear, std::sin(time), 0.0f, 0.0f, std::cos(time), 0.0f, 0.0f);
        multiplyMatrices(shear, reflectionZ5, dynamicTransform);

        glClearColor(0.96f, 0.97f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        sendCamera(program);

        drawAxes(buffers, transformLoc, colorLoc);
        drawPyramid(buffers, transformLoc, colorLoc, identity, 0.72f, 0.72f, 0.72f);
        drawPyramid(buffers, transformLoc, colorLoc, reflectionZ5, 0.06f, 0.58f, 0.20f);
        drawPyramid(buffers, transformLoc, colorLoc, dynamicTransform, 0.88f, 0.28f, 0.06f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    destroyScene(window, program, &buffers);
    return 0;
}
