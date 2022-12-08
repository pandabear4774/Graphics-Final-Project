#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
uniform mat3 normMatrix;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 spotLightPosition;

uniform vec3 materialColor;             // the material color for our vertex (& whole object)

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
in vec3 vecNormal;

// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex

void main() {
    color = vec3(0,0,0);

    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    //ambient lighting
    float ambientStrength = 0.05;
    color += ambientStrength * lightColor;

    vec3 reverseLightVector = normalize(-1 * lightDir);
    vec3 trueVecNormal = vecNormal * normMatrix;
    vec3 diffuseComponent = lightColor * max(dot(reverseLightVector, trueVecNormal), 0);

    color += diffuseComponent;



    //spot light

    vec3 spotLightDirection = vec3(0,-1,0);
    float cutoffAngle = 0.4;

    vec3 worldCords = vec3(mvpMatrix * vec4(vPos, 1.0));

    vec3 lightDir3 = normalize(spotLightPosition - worldCords);


    float theta = dot(spotLightDirection, normalize(-lightDir3));

    if(theta > cutoffAngle){
        // diffuse shading
        float diff2 = max(dot(vecNormal, lightDir3), 0.0);

        // combine results
        vec3 ambient2  = vec3(1.0,1.0,1.0) * 0.5;
        vec3 diffuse2  = lightColor * diff2;

        float distance2    = length(spotLightPosition - worldCords);
        float attenuation2 = 1.0 / (1 + 2 * distance2 +
                     1 * (distance2 * distance2));

        diffuse2  *= attenuation2 * 600;

        color += 0 * (diffuse2 + ambient2);
    }


    color = materialColor * color;

}
