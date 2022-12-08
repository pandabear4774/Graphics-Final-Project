#include "Coin.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

Coin::Coin( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation, GLint modelMatrixLocation ) {

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;
    _shaderProgramUniformLocations.modelMtx = modelMatrixLocation;

    respawn();

}

void Coin::update() {
    _angle += 0.1;
}

void Coin::respawn() {
    dead = false;

    size = 0.5f;

    _angle = 0;

    float x = rand() % 55;
    float y = 0.0f;
    float z = rand() % 55;

    if(rand()% 2 == 0){
        x *= -1;
    }
    if(rand()% 2 == 0){
        z *= -1;
    }

    _position = glm::vec3(x,y,z);
}

void Coin::drawCoin(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {
    glUseProgram( _shaderProgramHandle );

    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, size * 1.25, 0.0f));

    modelMtx = glm::rotate(modelMtx, _angle, CSCI441::Y_AXIS);

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &glm::vec3(0.5,0.8,0.9)[0]);


    if(!dead){
        CSCI441::drawSolidTorus(size/2, size, 16,16);

    }
}
void Coin::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.modelMtx, 1, GL_FALSE, &modelMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}
