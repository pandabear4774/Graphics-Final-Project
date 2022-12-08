//
// Created by Kurtis Quant on 12/6/22.
//
#ifndef LAB05_DEATHPARTICLE_HPP
#define LAB05_DEATHPARTICLE_HPP

#include <GL/glew.h>

#include <glm/glm.hpp>
class DeathParticle {
public:
    DeathParticle( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation );

    void drawParticle( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );

    glm::vec3 _direction;

    glm::vec3 _position;

    int _lifeTime;

    void update();

    bool dead = false;
private:

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

    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;

};


#endif //LAB05_DEATHPARTICLE_H
