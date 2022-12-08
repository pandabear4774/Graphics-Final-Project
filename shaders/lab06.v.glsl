#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;

// attribute inputs
in vec3 vPos;
in vec2 textureCord;

in vec3 vNormal;

// varying outputs
out vec2 fragTextureCord;



void main() {
    gl_Position = mvpMatrix * vec4(vPos, 1.0);


    vec3 normalVector = vNormal;

    fragTextureCord = textureCord;

}