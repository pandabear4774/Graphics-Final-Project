#include "Enemy.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif

Enemy::Enemy( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation, GLint modelMatrixUniformLocation ) {

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;
    _shaderProgramUniformLocations.modelMtx = modelMatrixUniformLocation;

    respawn();

}

void Enemy::update() {
    float mag = sqrt(_direction.x * _direction.x + _direction.z * _direction.z);

    _direction.x /= mag;
    _direction.z /= mag;

    _position -= speed * 0.8f * _direction;
    _position.y = 0;

    //check to make sure that the plane is inside the bounds
    if(_position.x > 55.0f){
        dead = true;
    } else if(_position.x < -55.0f){
        dead = true;
    }

    //make sure the z axis is also within the bounds
    if(_position.z > 55.0f){
        dead = true;
    } else if(_position.z < -55.0f){
        dead = true;
    }

    chainSawAngle -= 0.5;
}

void Enemy::checkDead(){
    if(dead){
        if(deathTimer == 0){
            spawnParticles();
        } else {
            for(int i = 0; i < particles.size(); i++){
                particles[i].update();
            }
        }
        deathTimer++;

        if(deathTimer > 200){
            respawn();
        }
    }
}
void Enemy::relocate(){
    chainSawAngle = 0;

    float x = 20 + rand() % 30;
    float y = 0.0f;
    float z = 20 + rand() % 30;

    if(rand()% 2 == 0){
        x *= -1;
    }
    if(rand()% 2 == 0){
        z *= -1;
    }

    _position = glm::vec3(x,y,z);
}
void Enemy::respawn() {
    deathTimer = 0;
    dead = false;

    size = 0.5f;

    consumedCount = 1;

    chainSawAngle = 0;

    float x = 20 + rand() % 30;
    float y = 0.0f;
    float z = 20 + rand() % 30;

    if(rand()% 2 == 0){
        x *= -1;
    }
    if(rand()% 2 == 0){
        z *= -1;
    }

    _position = glm::vec3(x,y,z);

    speed = 0.05f;

    particles.clear();
}
void Enemy::spawnParticles() {
    for(int i = 0; i < 10; i++){
        particles.push_back( DeathParticle(_shaderProgramHandle,
                                           _shaderProgramUniformLocations.mvpMtx,
                                           _shaderProgramUniformLocations.normalMtx,
                                           _shaderProgramUniformLocations.materialColor));
    }
}

void Enemy::drawEnemy(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) {
    glUseProgram( _shaderProgramHandle );

    modelMtx = glm::rotate(modelMtx, -_angle, CSCI441::Y_AXIS);

    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, size, 0.0f));

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &glm::vec3(0.2,0.8,0.2)[0]);


    if(!dead){
        CSCI441::drawSolidSphere( size,16,16);
    }

    modelMtx = glm::scale(modelMtx, glm::vec3(1.0,1.0,0.1f));

    modelMtx = glm::rotate(modelMtx, chainSawAngle, CSCI441::Z_AXIS);

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &glm::vec3(0.9,0.7,0.05)[0]);


    if(!dead){
        CSCI441::drawSolidCube( size * 2.0f);
    }
}
void Enemy::drawParticles( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    glUseProgram( _shaderProgramHandle );

    for(int i = 0; i < particles.size(); i++){
        particles[i].drawParticle(modelMtx,viewMtx,projMtx);
    }
}

void Enemy::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.modelMtx, 1, GL_FALSE, &modelMtx[0][0] );


    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}
