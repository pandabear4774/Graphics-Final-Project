#version 410 core

out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{
    //maps the coloring in the texture to the correct spot
    FragColor = texture(skybox, texCoords);
}