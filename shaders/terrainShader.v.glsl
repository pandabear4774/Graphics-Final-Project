#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;
uniform float time;

// attribute inputs
in vec3 vPos;
in vec2 textureCord;

in vec3 vNormal;

// varying outputs
out vec2 fragTextureCord;


void main() {

    vec3 v = vPos;
    //start moving the map after the time reaches 5
    if (time > 5.0){
        v.y = vPos.y + sin(time * 7 + vPos.x - vPos.z) / 2.0;
    }
    gl_Position = mvpMatrix * vec4(v, 1.0);

    vec3 normalVector = vNormal;

    fragTextureCord = textureCord;

}