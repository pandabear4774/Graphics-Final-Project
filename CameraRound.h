//
// Created by Kurtis Quant on 10/7/22.
//

#ifndef LAB05_CAMERAROUND_H
#define LAB05_CAMERAROUND_H

#include <CSCI441/Camera.hpp>

class CameraRound : public CSCI441::Camera {
public:
    void recomputeOrientation() final;

    void updateLocation(glm::vec3 newLocation);

    void moveForward(GLfloat movementFactor) final;
    void moveBackward(GLfloat movementFactor) final;

    float zoom;
};



#endif //LAB05_CAMERAROUND_H
