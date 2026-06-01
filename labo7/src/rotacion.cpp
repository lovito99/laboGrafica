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

// Construir matriz de rotacion alrededor de un punto (cx, cy)
void matrizRotacionAlrededorDe(float anguloGrados, float cx, float cy,
    float M[9])
{
    float rad = anguloGrados * M_PI / 180.0f;
    float cosA = cos(rad);
    float sinA = sin(rad);

    // Matriz: T(cx,cy) * R(theta) * T(-cx,-cy)
    // Para un punto p: p' = R * (p - centro) + centro = R*p + (centro -
    // R*centro)
    float tx = cx - (cosA * cx - sinA * cy);
    float ty = cy - (sinA * cx + cosA * cy);

    M[0] = cosA; M[3] = -sinA; M[6] = tx;
    M[1] = sinA; M[4] =  cosA; M[7] = ty;
    M[2] = 0.0f; M[5] =  0.0f; M[8] = 1.0f;
}

// -------------------------------------------------------
// Inicializacion de VAO/VBO con los vertices de una figura
// -------------------------------------------------------
void inicializarGeometria()
{
    // Crear un triangulo mas interesante para ver la rotacion
    float vertices[] = {
        // Triangulo
         0.0f,  0.5f, // vertice 0 (superior)
        -0.4f, -0.3f, // vertice 1 (inferior izquierdo)
         0.4f, -0.3f, // vertice 2 (inferior derecho)

        // Cuadrado (opcional, descomentar si se quiere)
        // -0.3f,  0.3f,
        //  0.3f,  0.3f,
        //  0.3f, -0.3f,
        // -0.3f, -0.3f
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
    GLFWwindow* window = glfwCreateWindow(600, 600, "Rotacion 2D -- GLFW",
        NULL, NULL);
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

    // 4. Compilar shaders y cargar geometria
    shaderProg = compilarShaders(vertSrc, fragSrc);
    inicializarGeometria();

    // Ubicaciones de los uniforms
    GLint locMat   = glGetUniformLocation(shaderProg, "uTransform");
    GLint locColor = glGetUniformLocation(shaderProg, "uColor");

    float M[9];
    float angulo = 0.0f;

    // 5. Bucle principal de renderizado (Render Loop)
    while (!glfwWindowShouldClose(window))
    {
        // Procesar entradas
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Rotar con las teclas R (derecha) y T (izquierda)
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            angulo += 1.0f;
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
            angulo -= 1.0f;

        // Limpiar el buffer de color
        glClear(GL_COLOR_BUFFER_BIT);

        // Activar el programa shader
        glUseProgram(shaderProg);

        // --- Dibujar figura ORIGINAL (sin rotacion) en rojo ---
        matrizRotacion(0.0f, M);
        glUniformMatrix3fv(locMat, 1, GL_FALSE, M);
        glUniform4f(locColor, 1.0f, 0.0f, 0.0f, 1.0f); // RGBA rojo

        glBindVertexArray(vao);
        glLineWidth(2.0f);
        glDrawArrays(GL_TRIANGLES, 0, 3); // Dibujar triangulo

        // --- Dibujar figura ROTADA (alrededor del origen) en verde ---
        matrizRotacion(angulo, M);
        glUniformMatrix3fv(locMat, 1, GL_FALSE, M);
        glUniform4f(locColor, 0.0f, 1.0f, 0.0f, 0.7f); // RGBA verde
        // semitransparente
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // --- Dibujar figura ROTADA alrededor de su centro (0,0) en
        // azul ---
        // Nota: Para este triangulo, el centro aproximado esta en (0,0)
        matrizRotacionAlrededorDe(angulo * 1.5f, 0.0f, 0.0f, M);
        glUniformMatrix3fv(locMat, 1, GL_FALSE, M);
        glUniform4f(locColor, 0.0f, 0.2f, 0.9f, 0.7f); // RGBA azul
        // semitransparente
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        // Mostrar angulo en consola cada cierto tiempo
        static int frameCount = 0;
        if (++frameCount % 60 == 0) {
            std::cout << "Angulo de rotacion (verde): " << angulo <<
                " grados" << std::endl;
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
