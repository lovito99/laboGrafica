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
float factorEscalaX = 1.0f;
float factorEscalaY = 1.0f;

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

// Construir matriz de escalamiento 2D (factores sx, sy)
void matrizEscalamiento(float sx, float sy, float M[9])
{
    // OpenGL usa column-major: M[col*3 + row]
    // col 0       col 1       col 2
    M[0] = sx;   M[3] = 0.0f; M[6] = 0.0f; // fila 0
    M[1] = 0.0f; M[4] = sy;   M[7] = 0.0f; // fila 1
    M[2] = 0.0f; M[5] = 0.0f; M[8] = 1.0f; // fila 2
}

// Construir matriz de escalamiento alrededor de un punto (cx, cy)
void matrizEscalamientoAlrededorDe(float sx, float sy, float cx, float
    cy, float M[9])
{
    // Matriz: T(cx,cy) * S(sx,sy) * T(-cx,-cy)
    // Para un punto p: p' = S * (p - centro) + centro = S*p + (centro -
    // S*centro)
    float tx = cx - (sx * cx);
    float ty = cy - (sy * cy);

    M[0] = sx;   M[3] = 0.0f; M[6] = tx;
    M[1] = 0.0f; M[4] = sy;   M[7] = ty;
    M[2] = 0.0f; M[5] = 0.0f; M[8] = 1.0f;
}

// Construir matriz de escalamiento uniforme (mismo factor en X y Y)
void matrizEscalamientoUniforme(float s, float M[9])
{
    matrizEscalamiento(s, s, M);
}

// -------------------------------------------------------
// Inicializacion de VAO/VBO con los vertices de una figura
// -------------------------------------------------------
void inicializarGeometria()
{
    // Crear un cuadrado para visualizar mejor el escalamiento
    float vertices[] = {
        // Cuadrado centrado en el origen
        -0.4f,  0.4f, // vertice 0 (superior izquierdo)
         0.4f,  0.4f, // vertice 1 (superior derecho)
         0.4f, -0.4f, // vertice 2 (inferior derecho)
        -0.4f, -0.4f, // vertice 3 (inferior izquierdo)

        // Una cruz interna para mejor referencia visual
         0.0f,  0.4f,
         0.0f, -0.4f,
        -0.4f,  0.0f,
         0.4f,  0.0f
    };

    // Indices para dibujar el cuadrado como dos triangulos
    unsigned int indices[] = {
        0, 1, 2, // Primer triangulo
        0, 2, 3  // Segundo triangulo
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    // Crear y configurar VBO para vertices
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
    GLFWwindow* window = glfwCreateWindow(600, 600, "Escalamiento 2D -- GLFW", NULL, NULL);
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

    float M[9];
    float escalaX = 1.0f;
    float escalaY = 1.0f;
    float escalaUniforme = 1.0f;

    std::cout << "=== CONTROLES ===" << std::endl;
    std::cout << "Q / A: Aumentar / Disminuir escala en X" << std::endl;
    std::cout << "W / S: Aumentar / Disminuir escala en Y" << std::endl;
    std::cout << "E / D: Aumentar / Disminuir escala uniforme" << std::endl;
    std::cout << "R: Reiniciar escalas" << std::endl;
    std::cout << "ESC: Salir" << std::endl;
    std::cout << "=================" << std::endl;

    // 5. Bucle principal de renderizado (Render Loop)
    while (!glfwWindowShouldClose(window))
    {
        // Procesar entradas
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Control de escala en X (Q = aumentar, A = disminuir)
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            escalaX += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            escalaX -= 0.01f;

        // Control de escala en Y (W = aumentar, S = disminuir)
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            escalaY += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            escalaY -= 0.01f;

        // Control de escala uniforme (E = aumentar, D = disminuir)
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            escalaUniforme += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            escalaUniforme -= 0.01f;

        // Reiniciar escalas con la tecla R
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            escalaX = 1.0f;
            escalaY = 1.0f;
            escalaUniforme = 1.0f;
        }

        // Limitar valores minimos para evitar inversion/colapso
        if (escalaX < 0.1f) escalaX = 0.1f;
        if (escalaY < 0.1f) escalaY = 0.1f;
        if (escalaUniforme < 0.1f) escalaUniforme = 0.1f;

        // Limpiar el buffer de color
        glClear(GL_COLOR_BUFFER_BIT);

        // Activar el programa shader
        glUseProgram(shaderProg);

        // --- Dibujar figura ORIGINAL (escala 1,1) en rojo ---
        matrizEscalamiento(1.0f, 1.0f, M);
        glUniformMatrix3fv(locMat, 1, GL_FALSE, M);
        glUniform4f(locColor, 1.0f, 0.0f, 0.0f, 0.5f); // RGBA rojo
        // semitransparente

        glBindVertexArray(vao);
        glLineWidth(2.0f);
        glDrawArrays(GL_QUADS, 0, 4); // Dibujar cuadrado original
        glDrawArrays(GL_LINES, 4, 4); // Dibujar cruz interna

        // --- Dibujar figura ESCALADA en X e Y (alrededor del origen)
        // en verde ---
        matrizEscalamiento(escalaX, escalaY, M);
        glUniformMatrix3fv(locMat, 1, GL_FALSE, M);
        glUniform4f(locColor, 0.0f, 1.0f, 0.0f, 0.7f); // RGBA verde
        // semitransparente
        glDrawArrays(GL_QUADS, 0, 4);
        glDrawArrays(GL_LINES, 4, 4);

        // --- Dibujar figura con ESCALAMIENTO UNIFORME (alrededor del
        // centro) en azul ---
        matrizEscalamientoUniforme(escalaUniforme, M);
        glUniformMatrix3fv(locMat, 1, GL_FALSE, M);
        glUniform4f(locColor, 0.0f, 0.2f, 0.9f, 0.7f); // RGBA azul
        // semitransparente
        glDrawArrays(GL_QUADS, 0, 4);
        glDrawArrays(GL_LINES, 4, 4);

        glBindVertexArray(0);

        // Mostrar valores de escala en consola cada cierto tiempo
        static int frameCount = 0;
        if (++frameCount % 60 == 0) {
            std::cout << "Escala X: " << escalaX << " | Escala Y: " <<
                escalaY
                << " | Escala Uniforme: " << escalaUniforme << std::endl;
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
