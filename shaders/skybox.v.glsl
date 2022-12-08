#version 410 core

in vec3 aPos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    texCoords = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}