#version 330 core
in float tVector;
out vec4 FragColor;

void main()
{
    vec3 color = mix(vec3(0.0, 1.0, 1.0), vec3(1.0, 0.0, 1.0), tVector);
    FragColor = vec4(color, 1.0);
}
