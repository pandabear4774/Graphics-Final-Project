#version 410 core

// uniform inputs
uniform sampler2D textureMap;

// varying inputs
in vec2 fragTextureCord;


// fragment outputs
out vec4 fragColorOut;

void main() {
    vec4 fragColor = texture(textureMap, fragTextureCord);

    fragColorOut = fragColor;
}