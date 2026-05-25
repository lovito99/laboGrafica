#version 330 core

// Atributo: posición 2D del vértice (enviada desde el VBO)
layout(location = 0) in vec2 aPos;

// Uniform: matriz de traslación 3x3 en coordenadas homogéneas
// enviada desde la CPU con glUniformMatrix3fv()
uniform mat3 uTranslation;

void main()
{
    // Convertir a coordenadas homogéneas: (x, y) -> (x, y, 1)
    vec3 posHomogenea = vec3(aPos, 1.0);

    // Aplicar traslación: P2 = T * P1
    vec3 posTransladada = uTranslation * posHomogenea;

    // Pasar al pipeline; z=0 (espacio 2D), w=1
    gl_Position = vec4(posTransladada.xy, 0.0, 1.0);
}

