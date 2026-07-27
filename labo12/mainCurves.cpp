#include "bezierComun.hpp"

int main()
{
    GLFWwindow* window = createBezierWindow("Practica 12 - Curva de Bezier cubica");
    if (!window) return -1;

    constexpr int segments = 200;
    GLuint vao = 0;
    GLuint vbo = createParameterBuffer(vao, segments);
    GLuint program = createCurveProgram();

    const glm::vec3 p0(-10, -5, 0), p1(-5, 10, 5);
    const glm::vec3 p2(5, -10, -5), p3(10, 5, 0);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);
        setMatrices(program, static_cast<float>(glfwGetTime()));
        drawBezier(program, vao, segments + 1, p0, p1, p2, p3,
                   glm::vec3(0, 1, 1), glm::vec3(1, 0, 1));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
