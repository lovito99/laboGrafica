// ============================================================
//  Practica 10 - Actividad 2
//  Tetraedro rotado 45 grados alrededor de la linea P(2,1,0) Q(6,5,0).
// ============================================================

#include "p10base.hpp"

int main()
{
    GLFWwindow* vent = crearVent("P10 - Tetraedro rotado en eje P(2,1,0) Q(6,5,0)");
    if (!vent) {
        return -1;
    }

    GLuint prog = compilar();

    const float verts[] = {
        2.0f, 1.0f, 3.0f, 0.95f, 0.25f, 0.15f,
        0.0f, 0.0f, 0.0f, 0.12f, 0.55f, 0.95f,
        4.0f, 0.0f, 0.0f, 0.20f, 0.82f, 0.28f,
        2.0f, 4.0f, 0.0f, 0.96f, 0.78f, 0.16f
    };

    const unsigned int inds[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 1,
        1, 3, 2
    };

    Malla tetra = crearMalla(verts, sizeof(verts), inds, sizeof(inds), 12);

    const glm::vec3 p(2.0f, 1.0f, 0.0f);
    const glm::vec3 q(6.0f, 5.0f, 0.0f);
    const glm::mat4 rot = matRotLinea(p, q, glm::radians(45.0f));
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
        dibujar(tetra, modelLoc, glm::mat4(1.0f), GL_TRIANGLES);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        dibujar(tetra, modelLoc, rot, GL_TRIANGLES);

        glfwSwapBuffers(vent);
        glfwPollEvents();
    }

    limpiar(vent, prog, &tetra);
    return 0;
}
