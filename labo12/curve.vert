#version 330 core
layout (location = 0) in float aT;

uniform vec3 P0;
uniform vec3 P1;
uniform vec3 P2;
uniform vec3 P3;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float tVector;

void main()
{
    float t = aT;
    float u = 1.0 - t;
    float b0 = u * u * u;
    float b1 = 3.0 * u * u * t;
    float b2 = 3.0 * u * t * t;
    float b3 = t * t * t;
    vec3 evaluatedPos = b0 * P0 + b1 * P1 + b2 * P2 + b3 * P3;
    gl_Position = projection * view * model * vec4(evaluatedPos, 1.0);
    tVector = t;
}
