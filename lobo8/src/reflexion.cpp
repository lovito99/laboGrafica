// ============================================================
//  Practica 08 - TRANSFORMACION 2D: REFLEXION
//  UNSAAC - Computacion Grafica I
//
//  Implementacion EXACTA de la Seccion 3 del PDF
//  (Modern OpenGL: Vertex Shader y Fragment Shader)
// ============================================================

#include "gl_common.hpp"
#include <iostream>

// -------------------------------------------------------
//  Seccion 3.1 - Shaders Base para Transformaciones (PDF)
// -------------------------------------------------------

// Vertex Shader (vertex_shader.glsl)
const char* vertSrc = R"(
#version 330 core
layout ( location = 0) in vec2 aPos ;
uniform mat4 transform ; // Matriz de transformacion ( Escala / Reflexion / Traslacion )

void main () {
    // Convierte el vector 2D en homogeneo y aplica la matriz
    gl_Position = transform * vec4 (aPos , 0.0 , 1.0) ;
}
)";

// Fragment Shader (fragment_shader.glsl)
const char* fragSrc = R"(
#version 330 core
out vec4 FragColor ;
uniform vec3 objectColor ; // Color del objeto dibujado

void main () {
    FragColor = vec4 ( objectColor , 1.0) ;
}
)";

// -------------------------------------------------------
//  Logica de transformaciones (del PDF, Seccion 3.2)
// -------------------------------------------------------
void renderTransformations ( GLuint shaderProgram ) {
    glUseProgram ( shaderProgram );
    unsigned int transformLoc = glGetUniformLocation ( shaderProgram , "transform" );

    // --- 3.1. Escalamiento con Punto Fijo ---
    // Punto fijo (ej. el tercer vertice del triangulo en coordenadas normalizadas)
    float fixedX = 0.2f;
    float fixedY = 0.2f;

    // Matrices temporales
    float matTranslateToOrigin [16];
    float matScale [16];
    float matTranslateBack [16];
    float tempResult [16];
    float finalScaleMatrix [16];

    // 1. Matriz: Trasladar el punto fijo al origen (-x, -y)
    createTranslationMatrix ( matTranslateToOrigin , -fixedX , -fixedY );

    // 2. Matriz: Escalar por un factor (ej. Sx = 1.5, Sy = 1.5)
    createScaleMatrix ( matScale , 1.5f, 1.5f);

    // 3. Matriz: Trasladar de vuelta a la posicion original (+x, +y)
    createTranslationMatrix ( matTranslateBack , fixedX , fixedY );

    // Multiplicacion: Recordar que la transformacion se aplica de derecha a izquierda:
    // P' = T_back * Scale * T_origin * P
    // Primero multiplicamos: Scale * T_origin
    multiplyMatrices ( matScale , matTranslateToOrigin , tempResult );
    // Luego multiplicamos: T_back * tempResult
    multiplyMatrices ( matTranslateBack , tempResult , finalScaleMatrix );

    // Enviamos la matriz manual al Shader
    glUniformMatrix4fv ( transformLoc , 1, GL_FALSE , finalScaleMatrix );
    glDrawArrays ( GL_LINE_LOOP , 0, 3);


    // --- 3.2. Reflexion con respecto al eje X ---
    float finalReflectXMatrix [16];
    // La reflexion en X es un escalamiento de Sy = -1
    createScaleMatrix ( finalReflectXMatrix , 1.0f, -1.0f);

    glUniformMatrix4fv ( transformLoc , 1, GL_FALSE , finalReflectXMatrix );
    glDrawArrays ( GL_LINE_LOOP , 0, 3);


    // --- 3.3. Reflexion con respecto al eje Y ---
    float finalReflectYMatrix [16];
    // La reflexion en Y es un escalamiento de Sx = -1
    createScaleMatrix ( finalReflectYMatrix , -1.0f, 1.0f);

    glUniformMatrix4fv ( transformLoc , 1, GL_FALSE , finalReflectYMatrix );
    glDrawArrays ( GL_LINE_LOOP , 0, 3);
}

// -------------------------------------------------------
//  VAO y VBO del triangulo
// -------------------------------------------------------
GLuint shaderProgram, vao, vbo;

void inicializarGeometria()
{
    // Triangulo; el tercer vertice coincide con el punto fijo (0.2, 0.2)
    float vertices[] = {
        -0.5f, -0.4f,   // v0
         0.5f, -0.4f,   // v1
         0.2f,  0.2f    // v2 - punto fijo del escalamiento
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

// -------------------------------------------------------
//  main
// -------------------------------------------------------
int main()
{
    configureGlfwForX11();
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    // Configurar OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    // Crear ventana
    GLFWwindow* window = glfwCreateWindow(700, 700,
        "P08 - Reflexion 2D", NULL, NULL);
    if (!window) {
        std::cerr << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error al inicializar GLEW" << std::endl;
        return -1;
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    shaderProgram = compileShaderProgram(vertSrc, fragSrc);
    inicializarGeometria();

    // Uniform de color: blanco para todos los dibujos
    unsigned int colorLoc = glGetUniformLocation(shaderProgram, "objectColor");

    // Bucle principal de renderizado
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT);

        // Vincular el VAO del triangulo
        glBindVertexArray(vao);

        // Establecer color blanco y llamar a renderTransformations
        glUseProgram(shaderProgram);
        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

        renderTransformations(shaderProgram);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpieza
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
