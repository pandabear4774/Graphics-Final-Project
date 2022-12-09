#version 410 core

// uniform inputs
uniform sampler2D textureMap;

// varying inputs
in vec2 fragTextureCord;


// fragment outputs
out vec4 fragColorOut;


float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec4 fragColor = texture(textureMap, fragTextureCord);

    //fun experimentations:
    fragColor = fragColor * rand(fragTextureCord);
    fragColor.w = 1.0f;

    fragColorOut = fragColor;
}
