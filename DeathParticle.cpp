#include "DeathParticle.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

using namespace std;

DeathParticle::DeathParticle( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation ) {

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;

    float x = -1 + (float)rand() / RAND_MAX * (1 - -1);
    float y = -1 + (float)rand() / RAND_MAX * (1 - -1);
    float z = -1 + (float)rand() / RAND_MAX * (1 - -1);
    _direction = glm::vec3(x,y,z);

    _lifeTime = rand() % 50 + 50;

    _position = glm::vec3(0,0,0);
}

void DeathParticle::update() {
    _position += _direction;
    _lifeTime--;

    if(_lifeTime <= 0){
        dead = true;
    }
}

void DeathParticle::drawParticle(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {
    glUseProgram( _shaderProgramHandle );

    modelMtx = glm::translate(modelMtx, _position);

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &glm::vec3(1,0,0)[0]);

    if(!dead)
    CSCI441::drawSolidSphere( 0.1 , 16, 16);

}

void DeathParticle::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}
