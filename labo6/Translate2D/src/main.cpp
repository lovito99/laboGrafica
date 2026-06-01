#include <GL/glew.h>
#include <GLFW/glfw3.h> // Reemplaza a <GL/glut.h>
#include <iostream>
#include <string>

// -------------------------------------------------------
// Fuentes de los shaders (en crudo como cadenas)
// -------------------------------------------------------
const char* vertSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat3 uTranslation;
void main() {
    vec3 p = uTranslation * vec3(aPos, 1.0);
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
constexpr GLsizei kVertexCount = 24;

// Función auxiliar para revisar errores de compilación
void chequearErroresShader(GLuint shader, std::string tipo) {
    GLint success;
    GLchar infoLog[1024];
    if (tipo != "PROGRAMA") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR del tipo: " << tipo << "\n" << infoLog << "\n";
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR del tipo: " << tipo << "\n" << infoLog << "\n";
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

// Construir matriz de traslacion 3x3 (column-major para OpenGL)
void matrizTraslacion(float tx, float ty, float M[9])
{
    // OpenGL usa column-major: M[col*3 + row]
    //  col 0       col 1       col 2
    M[0]=1; M[3]=0; M[6]=tx;  // fila 0
    M[1]=0; M[4]=1; M[7]=ty;  // fila 1
    M[2]=0; M[5]=0; M[8]=1;   // fila 2
}

// -------------------------------------------------------
// Inicializacion de VAO/VBO con los vertices de la letra E
// -------------------------------------------------------
void inicializarGeometria()
{
    // La letra E se construye con cuatro rectangulos: una barra vertical
    // y tres barras horizontales. Cada rectangulo se divide en dos triangulos.
    float vertices[] = {
        // Barra vertical izquierda
        -0.80f,  0.55f,  -0.60f,  0.55f,  -0.60f, -0.55f,
        -0.80f,  0.55f,  -0.60f, -0.55f,  -0.80f, -0.55f,

        // Barra superior
        -0.60f,  0.55f,  -0.10f,  0.55f,  -0.10f,  0.35f,
        -0.60f,  0.55f,  -0.10f,  0.35f,  -0.60f,  0.35f,

        // Barra central
        -0.60f,  0.10f,  -0.28f,  0.10f,  -0.28f, -0.10f,
        -0.60f,  0.10f,  -0.28f, -0.10f,  -0.60f, -0.10f,

        // Barra inferior
        -0.60f, -0.35f,  -0.10f, -0.35f,  -0.10f, -0.55f,
        -0.60f, -0.35f,  -0.10f, -0.55f,  -0.60f, -0.55f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Atributo 0: vec2 aPos
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
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
    GLFWwindow* window = glfwCreateWindow(600, 600, "Traslacion 2D -- GLFW", NULL, NULL);
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

    // Configuración inicial de OpenGL
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f); // fondo gris oscuro

    // 4. Compilar shaders y cargar geometría
    shaderProg = compilarShaders(vertSrc, fragSrc);
    inicializarGeometria();

    // Ubicaciones de los uniforms
    GLint locMat   = glGetUniformLocation(shaderProg, "uTranslation");
    GLint locColor = glGetUniformLocation(shaderProg, "uColor");

    // 5. Bucle principal de renderizado (Render Loop)
    while (!glfwWindowShouldClose(window))
    {
        // Procesar entradas (por ejemplo, si el usuario presiona ESC para cerrar)
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Limpiar el buffer de color
        glClear(GL_COLOR_BUFFER_BIT);

        // Activar el programa shader
        glUseProgram(shaderProg);

        float M[9];

        // Dibujar la letra E con la traslacion solicitada en la tarea.
        matrizTraslacion(0.4f, -0.3f, M);
        glUniformMatrix3fv(locMat, 1, GL_FALSE, M);
        glUniform4f(locColor, 0.72f, 0.72f, 0.72f, 1.0f);
        
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, kVertexCount);

        glBindVertexArray(0);

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
