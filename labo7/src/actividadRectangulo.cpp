#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <cmath>

// -------------------------------------------------------
// Fuentes de los shaders (en crudo como cadenas)
// -------------------------------------------------------
const char* vertSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat3 uTransform;
void main() {
    vec3 p = uTransform * vec3(aPos, 1.0);
    gl_Position = vec4(p.xy, 0.0, 1.0);
}
)";

const char* fragSrc = R"(
#version 330 core
uniform vec4 uColor;
out vec4 fragColor;
void main() {
    fragColor = uColor;
}
)";

// -------------------------------------------------------
// Variables globales
// -------------------------------------------------------
GLuint shaderProg, vao, vbo;

// Funcion auxiliar para revisar errores de compilacion
void chequearErroresShader(GLuint shader, std::string tipo) {
    GLint success;
    GLchar infoLog[1024];
    if (tipo != "PROGRAMA") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR del tipo: " <<
                tipo << "\n" << infoLog << "\n";
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR del tipo: " <<
                tipo << "\n" << infoLog << "\n";
        }
    }
}

// Funcion auxiliar: compilar y enlazar shaders
GLuint compilarShaders(const char* vs, const char* fs)
{
    GLuint vid = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vid, 1, &vs, nullptr);
    glCompileShader(vid);
    chequearErroresShader(vid, "VERTEX");

    GLuint fid = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fid, 1, &fs, nullptr);
    glCompileShader(fid);
    chequearErroresShader(fid, "FRAGMENT");

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vid);
    glAttachShader(prog, fid);
    glLinkProgram(prog);
    chequearErroresShader(prog, "PROGRAMA");

    glDeleteShader(vid);
    glDeleteShader(fid);
    return prog;
}

// Construir matriz identidad 3x3 (column-major)
void matrizIdentidad(float M[9])
{
    M[0] = 1.0f; M[3] = 0.0f; M[6] = 0.0f;
    M[1] = 0.0f; M[4] = 1.0f; M[7] = 0.0f;
    M[2] = 0.0f; M[5] = 0.0f; M[8] = 1.0f;
}

// Construir matriz de traslacion 2D
void matrizTraslacion(float tx, float ty, float M[9])
{
    // OpenGL usa column-major: M[col*3 + row]
    M[0] = 1.0f; M[3] = 0.0f; M[6] = tx;
    M[1] = 0.0f; M[4] = 1.0f; M[7] = ty;
    M[2] = 0.0f; M[5] = 0.0f; M[8] = 1.0f;
}

// Construir matriz de rotacion 2D (angulo en grados)
void matrizRotacion(float anguloGrados, float M[9])
{
    float rad = anguloGrados * M_PI / 180.0f;
    float cosA = cos(rad);
    float sinA = sin(rad);

    // OpenGL usa column-major: M[col*3 + row]
    // col 0       col 1       col 2
    M[0] = cosA; M[3] = -sinA; M[6] = 0.0f; // fila 0
    M[1] = sinA; M[4] =  cosA; M[7] = 0.0f; // fila 1
    M[2] = 0.0f; M[5] =  0.0f; M[8] = 1.0f; // fila 2
}

// Multiplicar dos matrices 3x3 column-major: C = A * B
// Esto aplica B primero, luego A (orden de transformacion: B -> A)
void multiplicarMatrices(const float A[9], const float B[9], float C[9])
{
    for (int col = 0; col < 3; col++) {
        for (int fila = 0; fila < 3; fila++) {
            C[col * 3 + fila] =
                A[0 * 3 + fila] * B[col * 3 + 0] +
                A[1 * 3 + fila] * B[col * 3 + 1] +
                A[2 * 3 + fila] * B[col * 3 + 2];
        }
    }
}

// -------------------------------------------------------
// Inicializacion de VAO/VBO con los vertices del rectangulo
// -------------------------------------------------------
void inicializarGeometria()
{
    // Rectangulo centrado en el origen, dos triangulos (6 vertices)
    float vertices[] = {
        -0.3f,  0.15f, // vertice 0 (superior izquierdo)
         0.3f,  0.15f, // vertice 1 (superior derecho)
         0.3f, -0.15f, // vertice 2 (inferior derecho)
        -0.3f,  0.15f, // vertice 3 (superior izquierdo)
         0.3f, -0.15f, // vertice 4 (inferior derecho)
        -0.3f, -0.15f  // vertice 5 (inferior izquierdo)
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
        GL_STATIC_DRAW);

    // Atributo 0: vec2 aPos
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// -------------------------------------------------------
// Programa principal
// -------------------------------------------------------
int main()
{
    // 1. Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    // Configurar GLFW para usar OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Crear la ventana
    GLFWwindow* window = glfwCreateWindow(600, 600,
        "Actividad 2 - Traslacion y Rotacion de rectangulo", NULL, NULL);
    if (!window) {
        std::cerr << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 3. Inicializar GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error al inicializar GLEW" << std::endl;
        return -1;
    }

    // Configuracion inicial de OpenGL
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f); // fondo gris oscuro
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 4. Compilar shaders y cargar geometria
    shaderProg = compilarShaders(vertSrc, fragSrc);
    inicializarGeometria();

    // Ubicaciones de los uniforms
    GLint locMat   = glGetUniformLocation(shaderProg, "uTransform");
    GLint locColor = glGetUniformLocation(shaderProg, "uColor");

    float M[9], T[9], R[9], TR[9];
    float angulo = 0.0f;

    // Traslacion fija: desplazar el rectangulo
    const float TX = 0.4f;
    const float TY = 0.2f;

    std::cout << "=== Actividad 2: Traslacion + Rotacion de rectangulo ===" << std::endl;
    std::cout << "Rojo:  rectangulo original en el origen" << std::endl;
    std::cout << "Verde: rectangulo trasladado (" << TX << ", " << TY
              << ") y luego rotado" << std::endl;
    std::cout << "R: Aumentar angulo | T: Disminuir angulo | ESC: Salir" << std::endl;

    // 5. Bucle principal de renderizado (Render Loop)
    while (!glfwWindowShouldClose(window))
    {
        // Procesar entradas
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Rotar con las teclas R (incrementar) y T (decrementar)
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            angulo += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
            angulo -= 1.0f;

        // Limpiar el buffer de color
        glClear(GL_COLOR_BUFFER_BIT);

        // Activar el programa shader
        glUseProgram(shaderProg);

        glBindVertexArray(vao);

        // --- Dibujar rectangulo ORIGINAL en rojo ---
        matrizIdentidad(M);
        glUniformMatrix3fv(locMat, 1, GL_FALSE, M);
        glUniform4f(locColor, 1.0f, 0.0f, 0.0f, 0.6f); // rojo
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // --- Dibujar rectangulo TRASLADADO y luego ROTADO en verde ---
        // Orden de aplicacion: primero T (traslacion), despues R (rotacion)
        // En notacion matricial: combinado = R * T
        matrizTraslacion(TX, TY, T);
        matrizRotacion(angulo, R);
        multiplicarMatrices(R, T, TR); // TR = R * T => aplica T primero,
        // luego R
        glUniformMatrix3fv(locMat, 1, GL_FALSE, TR);
        glUniform4f(locColor, 0.0f, 1.0f, 0.0f, 0.7f); // verde
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);

        // Mostrar angulo en consola cada cierto tiempo
        static int frameCount = 0;
        if (++frameCount % 60 == 0) {
            std::cout << "Angulo de rotacion: " << angulo << " grados"
                      << std::endl;
        }

        // Intercambiar buffers y procesar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 6. Limpieza de memoria al salir
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProg);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
