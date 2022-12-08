//
// Created by Kurtis Quant on 12/6/22.
//

#ifndef LAB05_ENEMY_HPP
#define LAB05_ENEMY_HPP

#include <vector>
#include <GL/glew.h>
#include "DeathParticle.hpp"
#include <glm/glm.hpp>

class Enemy {
    public:
        Enemy( GLuint shaderProgramHandle, GLint mvpMtxUniformLocation, GLint normalMtxUniformLocation, GLint materialColorUniformLocation );

        void drawEnemy( glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );

        void drawParticles(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx );

        void relocate();

        glm::vec3 _direction;

        glm::vec3 _position;

        int deathTimer = 0;

        void update();

        void spawnParticles();

        void checkDead();

        void respawn();

        bool dead = false;

        float chainSawAngle;

        float size;

        float consumedCount;

        float _angle;

        float speed;
    private:
        std::vector<DeathParticle> particles;

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


#endif //LAB05_ENEMY_HPP
