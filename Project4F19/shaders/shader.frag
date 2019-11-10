#version 330 core

in vec3 normal;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
    fragColor = vec4((normalize(normal) + vec3(1.0)) / vec3(2.0), 1.0);
}
