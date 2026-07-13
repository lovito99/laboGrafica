// ============================================================
//  Practica 11 - Actividad B
//  Reflexion geometrica estricta sobre el plano infinito Z = 5.
// ============================================================

#include "comun3d.hpp"

int main()
{
    GLFWwindow* window = createWindow("P11 B - Reflexion estricta Z = 5");
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

        glClearColor(0.96f, 0.97f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        sendCamera(program);

        drawAxes(buffers, transformLoc, colorLoc);
        drawPyramid(buffers, transformLoc, colorLoc, identity, 0.90f, 0.12f, 0.10f);
        drawPyramid(buffers, transformLoc, colorLoc, reflectionZ5, 0.06f, 0.58f, 0.20f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    destroyScene(window, program, &buffers);
    return 0;
}
