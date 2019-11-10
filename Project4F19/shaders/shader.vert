#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 aNormal;

out vec3 normal;

// Uniform variables can be updated by fetching their location and passing values to that location
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;

void main()
{
	normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(vec3(model * vec4(position, 1.0)), 1.0);
}
