// ============================================================
//  Practica 10 - Actividad 1
//  Cubo rotado 30 grados alrededor de la linea A(0,0,0) B(7,7,7).
// ============================================================

#include "p10base.hpp"

int main()
{
    GLFWwindow* vent = crearVent("P10 - Cubo rotado en eje A(0,0,0) B(7,7,7)");
    if (!vent) {
        return -1;
    }

    GLuint prog = compilar();

    const float verts[] = {
        -1.0f, -1.0f, -1.0f, 0.90f, 0.15f, 0.12f,
         1.0f, -1.0f, -1.0f, 0.12f, 0.55f, 0.95f,
         1.0f,  1.0f, -1.0f, 0.20f, 0.80f, 0.25f,
        -1.0f,  1.0f, -1.0f, 0.98f, 0.78f, 0.18f,
        -1.0f, -1.0f,  1.0f, 0.65f, 0.25f, 0.86f,
         1.0f, -1.0f,  1.0f, 0.10f, 0.75f, 0.72f,
         1.0f,  1.0f,  1.0f, 0.94f, 0.38f, 0.12f,
        -1.0f,  1.0f,  1.0f, 0.75f, 0.75f, 0.75f
    };

    const unsigned int inds[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        0, 4, 7, 0, 7, 3,
        1, 5, 6, 1, 6, 2,
        3, 2, 6, 3, 6, 7,
        0, 1, 5, 0, 5, 4
    };

    Malla cubo = crearMalla(verts, sizeof(verts), inds, sizeof(inds), 36);

    const glm::vec3 a(0.0f, 0.0f, 0.0f);
    const glm::vec3 b(7.0f, 7.0f, 7.0f);
    const glm::mat4 rot = matRotLinea(a, b, glm::radians(30.0f));
    const GLint modelLoc = glGetUniformLocation(prog, "model");

    while (!glfwWindowShouldClose(vent)) {
        if (glfwGetKey(vent, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(vent, true);
        }

        glClearColor(0.08f, 0.09f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(prog);
        enviarCam(prog);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        dibujar(cubo, modelLoc, glm::mat4(1.0f), GL_TRIANGLES);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        dibujar(cubo, modelLoc, rot, GL_TRIANGLES);

        glfwSwapBuffers(vent);
        glfwPollEvents();
    }

    limpiar(vent, prog, &cubo);
    return 0;
}
