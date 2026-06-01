#version 330 core

// Color enviado desde la CPU (RGBA)
uniform vec4 uColor;

// Salida: color final del fragmento
out vec4 fragColor;

void main()
{
    fragColor = uColor;
}
