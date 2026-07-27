#include "bezierComun.hpp"

struct Interaction {
    glm::vec3* selected;
    bool keys[GLFW_KEY_LAST + 1]{};
};

static void keyCallback(GLFWwindow* window, int key, int, int action, int)
{
    auto* state = static_cast<Interaction*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key <= GLFW_KEY_LAST) state->keys[key] = action != GLFW_RELEASE;
}

int main()
{
    GLFWwindow* window = createBezierWindow("P12 - Bezier compuesta C1 | 1:A0 2:B3");
    if (!window) return -1;

    constexpr int segments = 240;
    GLuint vao = 0;
    GLuint vbo = createParameterBuffer(vao, segments);
    GLuint program = createCurveProgram();

    glm::vec3 a0(-12, -4, 0), a1(-9, 8, 4), a2(-4, 7, -3), a3(0, 0, 0);
    glm::vec3 b2(7, -7, 4), b3(12, 4, 0);
    Interaction input{&a0};
    glfwSetWindowUserPointer(window, &input);
    glfwSetKeyCallback(window, keyCallback);

    double previous = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        const double now = glfwGetTime();
        const float step = 6.0f * static_cast<float>(now - previous);
        previous = now;
        if (input.keys[GLFW_KEY_1]) input.selected = &a0;
        if (input.keys[GLFW_KEY_2]) input.selected = &b3;
        if (input.keys[GLFW_KEY_LEFT])  input.selected->x -= step;
        if (input.keys[GLFW_KEY_RIGHT]) input.selected->x += step;
        if (input.keys[GLFW_KEY_UP])    input.selected->y += step;
        if (input.keys[GLFW_KEY_DOWN])  input.selected->y -= step;
        if (input.keys[GLFW_KEY_W])     input.selected->z += step;
        if (input.keys[GLFW_KEY_S])     input.selected->z -= step;

        // Restriccion estricta: B0=A3 y B1=A3+(A3-A2).
        const glm::vec3 b0 = a3;
        const glm::vec3 b1 = a3 + (a3 - a2);

        glClearColor(0.055f, 0.065f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);
        setMatrices(program, static_cast<float>(now));
        drawBezier(program, vao, segments + 1, a0, a1, a2, a3,
                   glm::vec3(0.05f, 0.95f, 1.0f), glm::vec3(0.25f, 1.0f, 0.35f));
        drawBezier(program, vao, segments + 1, b0, b1, b2, b3,
                   glm::vec3(0.25f, 1.0f, 0.35f), glm::vec3(1.0f, 0.1f, 0.8f));
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
