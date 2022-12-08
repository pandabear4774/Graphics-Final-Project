//
// Created by Kurtis Quant on 10/7/22.
//

#include "CameraRound.h"
#include <cmath>
#include <iostream>

void CameraRound::recomputeOrientation() {
    //use the lookat point and the direction camera is facing to compute the position that the camera should be located at.
    float x,y,z;

    x =  zoom * sin(_phi) * sin(_theta);

    y = zoom * -1 * cos(_phi);

    z = zoom * -1 * sin(_phi) * cos(_theta);

    glm::vec3 cords = glm::vec3(x,y,z);

    _direction = cords;

    _position = _lookAtPoint - _direction;

    //dont want the camera to be below the world because that makes the world look weird and glitchy
    if(_position.y < 0.1){
        _position.y = 0.1;
    }

    computeViewMatrix();
}

//since its a arcball camera we want the camera to always be looking at a certain point where the plane is at
void CameraRound::updateLocation(glm::vec3 newLocation) {
    _lookAtPoint = newLocation;
    recomputeOrientation();
    computeViewMatrix();
}

//need move forward because its abstract method
void CameraRound::moveForward(GLfloat movementFactor) {
    _position += movementFactor * _direction;
    if(_position.y < 5){
        _position.y = 5;
    }
    _lookAtPoint += movementFactor * _direction;
    if(_lookAtPoint.y < 5){
        _lookAtPoint.y = 5;
    }
    computeViewMatrix();
}

//need mvoe backward because its abstract method
void CameraRound::moveBackward(GLfloat movementFactor) {

    _position -= movementFactor * _direction;
    _lookAtPoint -= movementFactor * _direction;
    computeViewMatrix();
}