#ifndef LAB05_PLANE_HPP
#define LAB05_PLANE_HPP

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include "DeathParticle.hpp"

class Plane {
public:
    /// \desc creates a simple plane that gives the appearance of flight
    /// \param shaderProgramHandle shader program handle that the plane should be drawn using
    /// \param mvpMtxUniformLocation uniform location for the full precomputed MVP matrix
    /// \param normalMtxUniformLocation uniform location for the precomputed Normal matrix
    /// \param materialColorUniformLocation uniform location for the material diffuse color
    Plane( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation );

    /// \desc draws the model plane for a given MVP matrix
    /// \param modelMtx existing model matrix to apply to plane
    /// \param viewMtx camera view matrix to apply to plane
    /// \param projMtx camera projection matrix to apply to plane
    /// \note internally uses the provided shader program and sets the necessary uniforms
    /// for the MVP and Normal Matrices as well as the material diffuse color
    void drawPlane( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );

    void drawParticles(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );
    /// \desc simulates the plane flying forward by rotating the propeller clockwise
    void flyForward();
    /// \desc simulates the plane flying backward by rotating the propeller counter-clockwise
    void flyBackward();

    void flyLeft();

    void flyRight();

    void jump();

    void gravity(float height);

    void moveLegs();

    void checkDead();

    void spawnParticles();

    float size;
    GLfloat _legAngle;
    glm::vec3 _planeLocation;
    GLfloat _direction;
    GLfloat speed;
    GLfloat dy = 0;
    bool dead = false;
    int deathTimer = 0;
    bool onGround = false;


private:
    std::vector<DeathParticle> particles;
    /// \desc current angle of rotation for the propeller
    GLfloat _propAngle;
    /// \desc one rotation step
    GLfloat _propAngleRotationSpeed;

    /// \desc handle of the shader program to use when drawing the plane
    GLuint _shaderProgramHandle;
    /// \desc stores the uniform locations needed for the plan information
    struct ShaderProgramUniformLocations {
        /// \desc location of the precomputed ModelViewProjection matrix
        GLint mvpMtx;
        /// \desc location of the precomputed Normal matrix
        GLint normalMtx;
        /// \desc location of the material diffuse color
        GLint materialColor;

    } _shaderProgramUniformLocations;

    /// \desc angle to rotate our plane at
    GLfloat _rotatePlaneAngle;

    /// \desc color the plane's body
    glm::vec3 _colorBody;
    /// \desc amount to scale the plane's body by
    glm::vec3 _scaleBody;

    /// \desc color the plane's wing
    glm::vec3 _colorWing;
    /// \desc amount to scale the plane's wing by
    glm::vec3 _scaleWing;

    /// \desc color the plane's wing
    glm::vec3 _colorHead;
    /// \desc amount to scale the plane's wing by
    glm::vec3 _scaleHead;

    /// \desc color the plane's wing
    glm::vec3 _colorArm;
    /// \desc amount to scale the plane's wing by
    glm::vec3 _scaleArm;

    void _drawPlaneBody(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _drawHead(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _drawLegs(bool isLeftWing, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _drawArms(bool isLeftWing, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx ) const;

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
};


#endif //LAB05_PLANE_HPP
