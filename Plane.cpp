#include "Plane.hpp"
#include "DeathParticle.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <CSCI441/objects.hpp>
#include <iostream>
#include <CSCI441/OpenGLUtils.hpp>

#ifndef M_PI
#define M_PI 3.14159265
#endif
using namespace std;

Plane::Plane( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation, GLint modelMatrixUniformLocation) {
    _propAngle = 0.0f;
    _propAngleRotationSpeed = M_PI / 16.0f;

    _shaderProgramHandle                            = shaderProgramHandle;
    _shaderProgramUniformLocations.mvpMtx           = mvpMtxUniformLocation;
    _shaderProgramUniformLocations.normalMtx        = normalMtxUniformLocation;
    _shaderProgramUniformLocations.materialColor    = materialColorUniformLocation;
    _shaderProgramUniformLocations.modelMtx = modelMatrixUniformLocation;

    _rotatePlaneAngle = M_PI / 2.0f;

    _colorBody = glm::vec3( 1.0f, 0.0f, 0.0f );
    _scaleBody = glm::vec3( 1.0f, 1.5f, 0.5f );

    _colorWing = glm::vec3( 1.0f, 0.0f, 0.0f );
    _scaleWing = glm::vec3( 0.5f, 1.25f, 0.5f );

    _colorHead = glm::vec3( 1.0f, 1.0f, 1.0f );
    _scaleHead = glm::vec3( 1.0f, 1.0f, 1.0f );

    _colorArm = glm::vec3( 1.0f, 0.0f, 0.0f );
    _scaleArm = glm::vec3( 0.4f, 1.0f, 0.5f );

    _planeLocation = glm::vec3(0,0,0);
    _direction = 0;
    size = 0.4;

}

void Plane::drawPlane( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    glUseProgram( _shaderProgramHandle );

    modelMtx = glm::rotate(modelMtx, -_legAngle, CSCI441::Y_AXIS);

    _drawLegs(true,modelMtx, viewMtx, projMtx);
    _drawLegs(false,modelMtx, viewMtx, projMtx);

    _drawArms(true,modelMtx, viewMtx, projMtx);
    _drawArms(false,modelMtx, viewMtx, projMtx);

    _drawPlaneBody(modelMtx, viewMtx, projMtx);        // the body of our plane
    _drawHead(modelMtx, viewMtx, projMtx);        // the body of our plane
}

void Plane::flyForward() {
    //change x cord based on the cos of the direction
    _planeLocation.x += speed * cos(_direction);



    //update the z axis as well
    _planeLocation.z += speed * sin(_direction);


}
void Plane::flyBackward() {
    //change x cord based on the cos of the direction
    _planeLocation.x -= speed * cos(_direction);


    //update the z axis as well
    _planeLocation.z -= speed * sin(_direction);


}
void Plane::flyRight() {
    //change x cord based on the cos of the direction
    _planeLocation.x += speed * cos(_direction + M_PI / 2);

    //update the z axis as well
    _planeLocation.z += speed * sin(_direction + M_PI / 2);

}

void Plane::flyLeft() {
    //change x cord based on the cos of the direction
    _planeLocation.x += speed * cos(_direction - M_PI / 2);

    //update the z axis as well
    _planeLocation.z += speed * sin(_direction - M_PI / 2);

}

void Plane::moveLegs(){
    //animate the propeller
    _propAngle += _propAngleRotationSpeed;

    float maxAngle = M_PI * (4.0f / 10.0f );
    if( _propAngle > maxAngle  ) {
        _propAngleRotationSpeed *= -1;
        _propAngle = maxAngle;
    }
    if( _propAngle < -maxAngle ) {
        _propAngleRotationSpeed *= -1;
        _propAngle = -maxAngle;
    }

    //check to make sure that the plane is inside the bounds
    if(_planeLocation.x > 55.0f){
        dead = true;
    } else if(_planeLocation.x < -55.0f){
        dead = true;
    }

    //make sure the z axis is also within the bounds
    if(_planeLocation.z > 55.0f){
        dead = true;
    } else if(_planeLocation.z < -55.0f){
        dead = true;
    }
}
void Plane::spawnParticles() {
    cout << "SPAWNING" << endl;
    for(int i = 0; i < 25; i++){
        particles.push_back( DeathParticle(_shaderProgramHandle,
                          _shaderProgramUniformLocations.mvpMtx,
                          _shaderProgramUniformLocations.normalMtx,
                          _shaderProgramUniformLocations.materialColor));
    }
}

void Plane::checkDead(){

    if(dead){
        if(deathTimer == 0){
            cout << "YOU DIED! I SHALL REVIVE YOU" << endl;
            spawnParticles();
        } else {
            for(int i = 0; i < particles.size(); i++){
                particles[i].update();
            }
        }
        deathTimer++;
        if(deathTimer > 250){
            dead = false;
            _planeLocation = glm::vec3(0,0,0);
            deathTimer = 0;
            _direction = M_PI;
            particles.clear();
        }
    }
}
void Plane::drawParticles( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) {
    glUseProgram( _shaderProgramHandle );

    for(int i = 0; i < particles.size(); i++){
        particles[i].drawParticle(modelMtx,viewMtx,projMtx);
    }
}
void Plane::jump()  {
    dy = 0.4;
}
void Plane::gravity(float height){
    if(_planeLocation.y > height){
        dy -= 0.01;
    }
    _planeLocation.y += dy;
    if(_planeLocation.y < height){
        _planeLocation.y = height;
        dy  = 0;
    }
}
void Plane::_drawPlaneBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, size*_scaleWing.y + size*_scaleBody.y / 2.0f, 0.0f));
    modelMtx = glm::scale( modelMtx, _scaleBody );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);

    CSCI441::drawSolidCube( size );
}
void Plane::_drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, size*_scaleWing.y + size*_scaleBody.y + size * _scaleHead.y / 2.0f, 0.0f));
    modelMtx = glm::scale( modelMtx, _scaleHead );

    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorBody[0]);

    CSCI441::drawSolidCube( size );
}

void Plane::_drawLegs(bool left, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {

    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, size * _scaleWing.y / 2.0f, 0.0f));

    if( left ) {
        modelMtx = glm::rotate(modelMtx, _propAngle, CSCI441::X_AXIS);
        modelMtx = glm::scale( modelMtx, _scaleWing );
        modelMtx = glm::translate(modelMtx, glm::vec3(size * _scaleWing.x, 0.0f, 0.0f));
    } else {
        modelMtx = glm::rotate(modelMtx, -_propAngle, CSCI441::X_AXIS);
        modelMtx = glm::scale( modelMtx, _scaleWing );
        modelMtx = glm::translate(modelMtx, glm::vec3(-size * _scaleWing.x, 0.0f, 0.0f));

    }


    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorWing[0]);

    CSCI441::drawSolidCube( size );
}

void Plane::_drawArms(bool left, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const {

    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, size*_scaleWing.y + size*_scaleBody.y - size * _scaleArm.y / 2.0f , 0.0f));



    if( left ) {
        modelMtx = glm::rotate(modelMtx, -_propAngle, CSCI441::X_AXIS);
        modelMtx = glm::scale( modelMtx, _scaleWing );
        modelMtx = glm::translate(modelMtx, glm::vec3(size * _scaleBody.x + size * _scaleArm.x, 0.0f, 0.0f));
    } else {
        modelMtx = glm::rotate(modelMtx, _propAngle, CSCI441::X_AXIS);
        modelMtx = glm::scale( modelMtx, _scaleWing );
        modelMtx = glm::translate(modelMtx, glm::vec3(-size * _scaleBody.x -size * _scaleArm.x, 0.0f, 0.0f));

    }


    _computeAndSendMatrixUniforms(modelMtx, viewMtx, projMtx);

    glUniform3fv(_shaderProgramUniformLocations.materialColor, 1, &_colorWing[0]);

    CSCI441::drawSolidCube( size );
}

void Plane::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.mvpMtx, 1, GL_FALSE, &mvpMtx[0][0] );
    glProgramUniformMatrix4fv( _shaderProgramHandle, _shaderProgramUniformLocations.modelMtx, 1, GL_FALSE, &modelMtx[0][0] );
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 20.0f, 0.0f));

    glm::mat3 normalMtx = glm::mat3( glm::transpose( glm::inverse( modelMtx )));
    glProgramUniformMatrix3fv( _shaderProgramHandle, _shaderProgramUniformLocations.normalMtx, 1, GL_FALSE, &normalMtx[0][0] );
}
