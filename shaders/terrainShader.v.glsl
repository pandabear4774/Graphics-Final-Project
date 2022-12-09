#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;

// attribute inputs
in vec3 vPos;
in vec2 textureCord;

in vec3 vNormal;

// varying outputs
out vec2 fragTextureCord;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    //fun experimentations:
    //gl_Position.y = gl_Position.y * rand(vec2(vPos.x,vPos.z));


    vec3 normalVector = vNormal;

    fragTextureCord = textureCord;

}