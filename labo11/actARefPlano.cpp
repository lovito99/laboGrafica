// ============================================================
//  Practica 11 - Actividad A
//  Reflexion de una piramide sobre el plano 3x + 5y + 6z = 0.
// ============================================================

#include "comun3d.hpp"

int main()
{
    GLFWwindow* window = createWindow("P11 A - Reflexion plano 3x + 5y + 6z = 0");
    if (!window) {
        return -1;
    }

    GLuint program = compileShaderProgram(shaderVert, shaderFrag);
    BuffEscena buffers;
    setupSceneGeometry(&buffers);

    const GLint transformLoc = glGetUniformLocation(program, "transformMatrix");
    const GLint colorLoc = glGetUniformLocation(program, "objectColor");

    float identity[16];
    float reflection[16];
    createIdentityMatrix(identity);
    createReflectionPlaneThroughOrigin(reflection, 3.0f, 5.0f, 6.0f);

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
        drawPyramid(buffers, transformLoc, colorLoc, reflection, 0.05f, 0.52f, 0.85f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    destroyScene(window, program, &buffers);
    return 0;
}
