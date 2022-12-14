/*
 *  CSCI 444, Advanced Computer Graphics, Fall 2021
 *
 *  Project: lab12
 *  File: billboardQuadShader.f.glsl
 *
 *  Description:
 *      Fragment Shader that applies color as diffuse texture
 *
 *  Author:
 *      Dr. Jeffrey Paone, Colorado School of Mines
 *
 *  Notes:
 *
 */

// we are using OpenGL 4.1 Core profile
#version 410 core

// ***** FRAGMENT SHADER UNIFORMS *****
uniform sampler2D image;
uniform float time;
// ***** FRAGMENT SHADER INPUT *****
// TODO #J add varying input
in vec2 texco;
// ***** FRAGMENT SHADER OUTPUT *****
out vec4 fragColorOut;


// ***** FRAGMENT SHADER HELPER FUNCTIONS *****


// ***** FRAGMENT SHADER MAIN FUNCTION *****
void main() {
    //animate the texture going in and out of redshift
    float timeComponent = abs(sin(3.0 * time));
    fragColorOut = vec4(texture(image, texco));
    fragColorOut.y = fragColorOut.y * timeComponent;
}
