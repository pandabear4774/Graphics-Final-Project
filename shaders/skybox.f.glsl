#version 410 core

out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;
uniform float time;

void main()
{
    float timeComponent = sin(time) + 0.75;
    if(timeComponent > 1.0){
        timeComponent = 1.0;
    }
    //maps the coloring in the texture to the correct spot
    FragColor = texture(skybox, texCoords) * timeComponent;
}