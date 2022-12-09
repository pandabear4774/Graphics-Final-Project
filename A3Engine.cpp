#include "A3Engine.hpp"

#include <CSCI441/objects.hpp>
#include <CSCI441/SimpleShader.hpp>
#import <iostream>
#include <stb_image.h>

//*************************************************************************************
//
// Helper Functions

#ifndef M_PI
#define M_PI 3.14159265
#endif

using namespace std;
/// \desc Simple helper function to return a random number between 0.0f and 1.0f.
GLfloat getRand() {
    return (GLfloat)rand() / (GLfloat)RAND_MAX;
}
float skyboxVertices[] =
        {
                -55.0f, -55.0f, 54.0f,
                54.0f, -55.0f, 54.0f,
                54.0f, -55.0f, -55.0f,
                -55.0f, -55.0f, -55.0f,
                -55.0f, 55.0f, 54.0f,
                54.0f, 55.0f, 54.0f,
                54.0f, 55.0f, -55.0f,
                -55.0f, 55.0f,-55.0f
        };

unsigned int skyboxIndices[] =
        {
                // Right
                1, 2, 6,
                6, 5, 1,
                // Left
                0, 4, 7,
                7, 3, 0,
                // Top
                4, 5, 6,
                6, 7, 4,
                // Bottom
                0, 3, 2,
                2, 1, 0,
                // Back
                0, 1, 5,
                5, 4, 0,
                // Front
                3, 7, 6,
                6, 2, 3
        };

//*************************************************************************************
//
// Public Interface

A3Engine::A3Engine()
         : CSCI441::OpenGLEngine(4, 1,
                                 640, 480,
                                 "Lab05: Flight Simulator v0.41 alpha") {
    for(auto& _key : _keys) _key = GL_FALSE;

    _mousePosition = glm::vec2(MOUSE_UNINITIALIZED, MOUSE_UNINITIALIZED );
    _leftMouseButtonState = GLFW_RELEASE;

}

A3Engine::~A3Engine() {
    delete _freeCam;
}

void A3Engine::handleKeyEvent(GLint key, GLint action) {
    if(key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if(action == GLFW_PRESS) {
        switch( key ) {
            // quit!
            case GLFW_KEY_Q:
            case GLFW_KEY_ESCAPE:
                setWindowShouldClose();
                break;

            default: break; // suppress CLion warning
        }
    }
}

void A3Engine::handleMouseButtonEvent(GLint button, GLint action) {
    // if the event is for the left mouse button
    if( button == GLFW_MOUSE_BUTTON_LEFT ) {
        // update the left mouse button's state
        _leftMouseButtonState = action;
    }
}

void A3Engine::handleCursorPositionEvent(glm::vec2 currMousePosition) {
    // if mouse hasn't moved in the window, prevent camera from flipping out
    if(_mousePosition.x == MOUSE_UNINITIALIZED) {
        _mousePosition = currMousePosition;
    }

    if (_currCam == 0) {
        _freeCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                         (_mousePosition.y - currMousePosition.y) * 0.005f);
    }

    if (_currCam == 1) {
        _freeCam2->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                         (_mousePosition.y - currMousePosition.y) * 0.005f);
    }

    if(_freeCam->getPhi() > 1.4){
        _freeCam->setPhi(1.4);
    }

    // update the mouse position
    _mousePosition = currMousePosition;
}

//*************************************************************************************
//
// Engine Setup

void A3Engine::_setupGLFW() {
    CSCI441::OpenGLEngine::_setupGLFW();

    // set our callbacks
    glfwSetKeyCallback(_window, lab05_engine_keyboard_callback);
    glfwSetMouseButtonCallback(_window, lab05_engine_mouse_button_callback);
    glfwSetCursorPosCallback(_window, lab05_engine_cursor_callback);
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

void A3Engine::_setupOpenGL() {
    glEnable( GL_DEPTH_TEST );					                        // enable depth testing
    glDepthFunc( GL_LESS );							                // use less than depth test

    glEnable(GL_BLEND);									            // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	    // use one minus blending equation

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	        // clear the frame buffer to black
}

void A3Engine::_setupShaders() {
    _billboardShaderProgram = new CSCI441::ShaderProgram( "shaders/billboardQuadShader.v.glsl",
                                                          "shaders/billboardQuadShader.g.glsl",
                                                          "shaders/billboardQuadShader.f.glsl" );
    _billboardShaderProgramUniforms.mvMatrix            = _billboardShaderProgram->getUniformLocation( "mvMatrix");
    _billboardShaderProgramUniforms.projMatrix          = _billboardShaderProgram->getUniformLocation( "projMatrix");
    _billboardShaderProgramUniforms.image               = _billboardShaderProgram->getUniformLocation( "image");
    // get attribute locations
    _billboardShaderProgramAttributes.vPos              = _billboardShaderProgram->getAttributeLocation( "vPos");
    // set static uniforms
    _billboardShaderProgram->setProgramUniform( _billboardShaderProgramUniforms.image, 0 );

    //all the lighting shaders are setup
    _lightingShaderProgram = new CSCI441::ShaderProgram("shaders/objectShader.v.glsl", "shaders/objectShader.f.glsl" );
    _lightingShaderUniformLocations.mvpMatrix      = _lightingShaderProgram->getUniformLocation("mvpMatrix");
    _lightingShaderUniformLocations.materialColor  = _lightingShaderProgram->getUniformLocation("materialColor");

    _lightingShaderUniformLocations.normMtx        = _lightingShaderProgram->getUniformLocation("normMatrix");
    _lightingShaderUniformLocations.lightDirection = _lightingShaderProgram->getUniformLocation("lightDir");
    _lightingShaderUniformLocations.lightColor     = _lightingShaderProgram->getUniformLocation("lightColor");
    _lightingShaderUniformLocations.spotlightPosition= _lightingShaderProgram->getUniformLocation("spotLightPosition");
    _lightingShaderUniformLocations.modelMatrix     = _lightingShaderProgram->getUniformLocation("modelMatrix");
    _lightingShaderUniformLocations.time     = _lightingShaderProgram->getUniformLocation("time");


    _lightingShaderAttributeLocations.vPos         = _lightingShaderProgram->getAttributeLocation("vPos");

    _lightingShaderAttributeLocations.vecNormal    = _lightingShaderProgram->getAttributeLocation("vecNormal");



    _textureShaderProgram = new CSCI441::ShaderProgram("shaders/terrainShader.v.glsl", "shaders/terrainShader.f.glsl" );
    // query uniform locations
    _textureShaderUniformLocations.mvpMatrix      = _textureShaderProgram->getUniformLocation("mvpMatrix");
    _textureShaderUniformLocations.textMap = _textureShaderProgram->getUniformLocation("textureMap");
    _textureShaderUniformLocations.time = _textureShaderProgram->getUniformLocation("time");


    // query attribute locations
    _textureShaderAttributeLocations.vPos         = _textureShaderProgram->getAttributeLocation("vPos");
    _textureShaderAttributeLocations.vNormal      = _textureShaderProgram->getAttributeLocation("vNormal");
    _textureShaderAttributeLocations.textCord      = _textureShaderProgram->getAttributeLocation("textureCord");


    // set static uniforms
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.textMap, 0);

    CSCI441::setVertexAttributeLocations(_textureShaderAttributeLocations.vPos,
                                         _textureShaderAttributeLocations.textCord );


    //setup skybox shaders
    _skyboxShaderProgram = new CSCI441::ShaderProgram( "shaders/skybox.v.glsl", "shaders/skybox.f.glsl" );
    // get uniform locations
    _skyboxShaderProgramUniformLocations.proj             = _skyboxShaderProgram->getUniformLocation("projection");
    _skyboxShaderProgramUniformLocations.view                 = _skyboxShaderProgram->getUniformLocation("view");
    _skyboxShaderProgramUniformLocations.skybox = _skyboxShaderProgram->getUniformLocation("skybox");
    _skyboxShaderProgramUniformLocations.time = _skyboxShaderProgram->getUniformLocation("time");

}

void A3Engine::_setupBuffers() {
    _deathParticles = (glm::vec3*)malloc(sizeof(glm::vec3) * 25);
    _deathParticleDirection = (glm::vec3*)malloc(sizeof(glm::vec3) * 25);
    _deathParticleIndices = (GLushort*)malloc(sizeof(GLushort) * 25);
    _distances = (GLfloat*)malloc(sizeof(GLfloat) * 25);
    glGenVertexArrays(1, _particleVAO);
    glGenBuffers(1, _particleVBO);
    glGenBuffers(1, _particleIBO);
    _numParticlePoints[0] = 25;
    for (int i = 0; i < 25; i++){
        GLfloat MAX = 1.0;
        glm::vec3 pos(rand() / (GLfloat)RAND_MAX * MAX * 2.0 - MAX, rand() / (GLfloat)RAND_MAX * MAX * 2.0 - MAX, rand() / (GLfloat)RAND_MAX * MAX * 2.0 - MAX);
        _deathParticles[i] = pos;
        _deathParticleIndices[i] = i;
    }
    glBindVertexArray(_particleVAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, _particleVBO[0]);
    glBufferData( GL_ARRAY_BUFFER, _numParticlePoints[0] * sizeof(glm::vec3), _deathParticles, GL_STATIC_DRAW );

    glEnableVertexAttribArray( _billboardShaderProgramAttributes.vPos );
    glVertexAttribPointer( _billboardShaderProgramAttributes.vPos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0 );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _particleIBO[0]);
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, _numParticlePoints[0] * sizeof(GLushort), _deathParticleIndices, GL_STATIC_DRAW );

    // Create VAO, VBO, and EBO for the skybox
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(skyboxVAO);

    //connect the vbo and ibos to the vertices and indexes
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);

    //each point has 3 floats so skip 3 float and start at the beginning
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //place where the skybox images are stored
    std::string facesCubemap[6] =
            {
                    "pictures/posx.png",
                    "pictures/negx.png",
                    "pictures/posy.png",
                    "pictures/negy.png",
                    "pictures/posz.png",
                    "pictures/negz.png"
            };


    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    //want to fill each side of the cube so linear makes the most sense cause no stretching
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // These are very important to prevent seams
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //read in and allign each side of the skybox
    for (unsigned int i = 0; i < 6; i++)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D
                    (
                            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                            0,
                            GL_RGB,
                            width,
                            height,
                            0,
                            GL_RGB,
                            GL_UNSIGNED_BYTE,
                            data
                    );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
            stbi_image_free(data);
        }
    }



    _texHandles[TEXTURE_ID::METAL] = _loadAndRegisterTexture("metal.jpg");

    _texHandles[TEXTURE_ID::MINES] = _loadAndRegisterTexture("mines.png");

    _texHandles[TEXTURE_ID::SNOWFLAKE] = _loadAndRegisterTexture("snowflake.png");


    CSCI441::setVertexAttributeLocations( _lightingShaderAttributeLocations.vPos,  _lightingShaderAttributeLocations.vecNormal);

    _plane = new Plane(_lightingShaderProgram->getShaderProgramHandle(),
                       _lightingShaderUniformLocations.mvpMatrix,
                       _lightingShaderUniformLocations.normMtx,
                       _lightingShaderUniformLocations.materialColor,
                       _lightingShaderUniformLocations.modelMatrix);

    //create the enemies
    for(int i = 0; i < 10; i++){
        enemies.push_back(new Enemy(_lightingShaderProgram->getShaderProgramHandle(),
                               _lightingShaderUniformLocations.mvpMatrix,
                               _lightingShaderUniformLocations.normMtx,
                               _lightingShaderUniformLocations.materialColor,
                                    _lightingShaderUniformLocations.modelMatrix));
    }

    //create the coins
    for(int i = 0; i < 10; i++){
        coins.push_back(new Coin(_lightingShaderProgram->getShaderProgramHandle(),
                                    _lightingShaderUniformLocations.mvpMatrix,
                                    _lightingShaderUniformLocations.normMtx,
                                    _lightingShaderUniformLocations.materialColor,
                                    _lightingShaderUniformLocations.modelMatrix));
    }

    _createGroundBuffers();
    _generateEnvironment();
}
float A3Engine::getHeight(int x, int z) {
    x +=55;
    z += 55;

    if(x < 0 || x > height || z < 0 || z > height){
        return 0;
    }

    return heightMap[x][z];
}
void A3Engine::_createGroundBuffers() {
    struct VertexNormalTextured {
        GLfloat x, y, z;
        GLfloat nx, ny, nz;
        GLfloat s,t;
    };

    // create our platform
    VertexNormalTextured platformVertices[12100];

    int nChannels;

    //read in data from the height map
    unsigned char *data = stbi_load("iceland_heightmap.png",
                                    &width, &height, &nChannels,
                                    0);


    float yScale = 64.0f / 256.0f, yShift = 16.0f;  // apply a scale+shift to the height data
    int count = 0;

    //fill the height map 2D array
    for(unsigned int i = 0; i < height; i++)
    {
        for(unsigned int j = 0; j < width; j++)
        {
            // retrieve texel for (i,j) tex coord
            unsigned char* texel = data + (j + width * i) * nChannels;
            // raw height at coordinate
            unsigned char y = texel[0];

            int x = -height/2.0f + i;
            float py = (int)y * yScale - yShift;
            float min = 0;
            float max = 10;
            float mapPY = (py - -14) / (40 - -14);
            float resultY = min + (max - min) * mapPY;

            int z = -width/2.0f + j;

            heightMap[x + 55][z + 55] = resultY;


        }
    }

    //create all vertices based on the height map
    for(unsigned int i = 0; i < height; i++)
    {
        for(unsigned int j = 0; j < width; j++)
        {
            int x = -height/2.0f + i;
            int z = -width/2.0f + j;



            float heightLeft = getHeight(x-1,z);
            float heightRight = getHeight(x+1,z);
            float heightDown = getHeight(x,z-1);
            float heightUp = getHeight(x,z+1);

            glm::vec3 normalVector(heightLeft - heightRight,2.0f,heightDown - heightUp);

            glm::normalize(normalVector);

            platformVertices[count] = {-height/2.0f + i,heightMap[x + 55][z + 55],-width/2.0f + j,normalVector.x,normalVector.y,normalVector.z,(float)i / ((float)height - 1), (float)j / ((float) height - 1)};
            count++;
        }
    }

    //done with the data
    stbi_image_free(data);

    //set the coins y position based on height map
    for(int i = 0; i < 10; i++){
        coins[i]->_position.y = 1.0f + heightMap[static_cast<int>(coins[i]->_position.x + 55.0f)][static_cast<int>(coins[i]->_position.z + 55.0f)];
    }

    //calculate all the indices for the IBO
    NUM_STRIPS = height-1;
    NUM_VERTS_PER_STRIP = width*2;
    GLushort platformIndices[(height-1) * width * 2];

    int counter = 0;
    for(unsigned int i = 0; i < height-1; i++)       // for each row a.k.a. each strip
    {
        for(unsigned int j = 0; j < width; j++)      // for each column
        {
            for(unsigned int k = 0; k < 2; k++)      // for each side of the strip
            {
                platformIndices[counter] = j + width * (i + k);
                counter++;
            }
        }
    }

    //number of total vertices
    _numGroundPoints = (height-1) * width * 2;

    glGenVertexArrays(1, &_groundVAO);
    glBindVertexArray(_groundVAO);

    GLuint vbods[2];       // 0 - VBO, 1 - IBO
    glGenBuffers(2, vbods);
    glBindBuffer(GL_ARRAY_BUFFER, vbods[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(platformVertices), platformVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(_textureShaderAttributeLocations.vPos);
    glVertexAttribPointer(_textureShaderAttributeLocations.vPos, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured), (void*)0);

    glEnableVertexAttribArray(_textureShaderAttributeLocations.vNormal);
    glVertexAttribPointer(_textureShaderAttributeLocations.vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured), (void*)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray( _textureShaderAttributeLocations.textCord );
    glVertexAttribPointer( _textureShaderAttributeLocations.textCord, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormalTextured), (void*) (sizeof(GLfloat) * 6) );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbods[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(platformIndices), platformIndices, GL_STATIC_DRAW);
}

void A3Engine::_generateEnvironment() {
    //******************************************************************
    // parameters to make up our grid size and spacing, feel free to
    // play around with this
    const GLfloat GRID_WIDTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_LENGTH = WORLD_SIZE * 1.8f;
    const GLfloat GRID_SPACING_WIDTH = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    // precomputed parameters based on above
    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5.0f;
    //******************************************************************

    //srand( time(0) );                                                   // seed our RNG

    // psych! everything's on a grid.
    for(int i = LEFT_END_POINT; i < RIGHT_END_POINT; i += GRID_SPACING_WIDTH) {
        for(int j = BOTTOM_END_POINT; j < TOP_END_POINT; j += GRID_SPACING_LENGTH) {
            // don't just draw a building ANYWHERE.
            if( i % 2 && j % 2 && getRand() < 0.4f ) {
                // translate to spot
                glm::mat4 transToSpotMtx = glm::translate( glm::mat4(1.0), glm::vec3(i, 0.0f, j) );

                // compute random height
                GLdouble height = powf(getRand(), 2.5)*10 + 1;
                // scale to building size
                glm::mat4 scaleToHeightMtx = glm::scale( glm::mat4(1.0), glm::vec3(1, height, 1) );

                // translate up to grid
                glm::mat4 transToHeight = glm::translate( glm::mat4(1.0), glm::vec3(0, height/2.0f, 0) );

                // compute full model matrix
                glm::mat4 modelMatrix = transToHeight * scaleToHeightMtx * transToSpotMtx;

                // compute random color
                glm::vec3 color( getRand(), getRand(), getRand() );
                // store building properties
                BuildingData currentBuilding = {modelMatrix, color};
                _buildings.emplace_back( currentBuilding );
            }
        }
    }
}

void A3Engine::_setupScene() {
    _freeCam = new CameraRound();
    _freeCam->setPosition( glm::vec3(60.0f, 5.0f, 30.0f) );
    _freeCam->setTheta( -M_PI / 2.0f);
    _freeCam->setPhi( M_PI / 2.8f );
    _freeCam->zoom = 8;

    _freeCam2 = new CSCI441::FreeCam();
    _freeCam2->setPosition(glm::vec3(60.0f, 5.0f, 30.0f));
    _freeCam2->setLookAtPoint(glm::vec3(55.0f, 5.0f, 30.0f));
    _freeCam2->setTheta( -M_PI / 2.0f );
    _freeCam2->setPhi( M_PI / 2.8f );
    _freeCam2->recomputeOrientation();

    _plane->_planeLocation = glm::vec3(0,0,0);
    _plane->_direction = -M_PI;
    _freeCam->recomputeOrientation();
    _cameraSpeed = glm::vec2(0.25f, 0.02f);


    glm::vec3 lightDirection = glm::vec3(-1,-1,-1);
    glm::vec3 lightColor = glm::vec3(0.5,0.5,0.5);

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightDirection,1,&lightDirection[0]);

    glProgramUniform3fv(_lightingShaderProgram->getShaderProgramHandle(), _lightingShaderUniformLocations.lightColor,1,&lightColor[0]);

    std::vector< glm::vec3 > points;
    // draw horizontal lines
    const GLfloat GRID_WIDTH = 100;
    const GLfloat GRID_LENGTH = 100;
    const GLfloat GRID_SPACING_WIDTH = 1.0f;
    const GLfloat GRID_SPACING_LENGTH = 1.0f;
    // precomputed parameters based on above
    const GLfloat LEFT_END_POINT = -GRID_WIDTH / 2.0f - 5.0f;
    const GLfloat RIGHT_END_POINT = GRID_WIDTH / 2.0f + 5.0f;
    const GLfloat BOTTOM_END_POINT = -GRID_LENGTH / 2.0f - 5.0f;
    const GLfloat TOP_END_POINT = GRID_LENGTH / 2.0f + 5.0f;

    for(GLfloat i = LEFT_END_POINT; i <= RIGHT_END_POINT; i += GRID_SPACING_WIDTH) {
        points.emplace_back( glm::vec3(i, 0, BOTTOM_END_POINT) );
        points.emplace_back( glm::vec3(i, 0, TOP_END_POINT) );
    }
    // draw vertical lines
    for(GLfloat j = BOTTOM_END_POINT; j <= TOP_END_POINT; j += GRID_SPACING_LENGTH) {
        points.emplace_back( glm::vec3(LEFT_END_POINT, 0, j) );
        points.emplace_back( glm::vec3(RIGHT_END_POINT, 0, j) );
    }
    _gridVAO = CSCI441::SimpleShader3::registerVertexArray(points, std::vector<glm::vec3>(points.size()));
    _numGridPoints = points.size();
    _gridColor = glm::vec3(1.0f, 1.0f, 1.0f);

    _skyboxShaderProgram->setProgramUniform(_skyboxShaderProgramUniformLocations.skybox,0);


}

//*************************************************************************************
//
// Engine Cleanup

void A3Engine::_cleanupShaders() {
    fprintf( stdout, "[INFO]: ...deleting Shaders.\n" );
    delete _lightingShaderProgram;
    delete _textureShaderProgram;
    delete _skyboxShaderProgram;
}

void A3Engine::_cleanupBuffers() {
    fprintf( stdout, "[INFO]: ...deleting VAOs....\n" );
    CSCI441::deleteObjectVAOs();
    glDeleteVertexArrays( 1, &_groundVAO );

    fprintf( stdout, "[INFO]: ...deleting VBOs....\n" );
    CSCI441::deleteObjectVBOs();

    fprintf( stdout, "[INFO]: ...deleting models..\n" );
    delete _plane;
}

//*************************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

void A3Engine::_renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const {
    float time = (float)timer / 100.0;


    if (_plane->dead){
        _billboardShaderProgram->useProgram();
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), _plane->_planeLocation);
        modelMatrix = glm::rotate(modelMatrix, _particleSystemAngle, CSCI441::Y_AXIS);
        glm::mat4 mvMatrix = viewMtx * modelMatrix;

        _billboardShaderProgram->setProgramUniform( _billboardShaderProgramUniforms.mvMatrix, mvMatrix );
        _billboardShaderProgram->setProgramUniform( _billboardShaderProgramUniforms.projMatrix, projMtx );

        glBindVertexArray( _particleVAO[0] );
        glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::SNOWFLAKE]);

        glm::vec3 normalizedViewVector = _freeCam->getLookAtPoint() - _freeCam->getPosition();
        for(int i = 0; i < 25; i++){
            glm::vec4 currentSprite = glm::vec4(_deathParticles[_deathParticleIndices[i]],0);
            glm::vec4 worldSpace = modelMatrix * currentSprite;
            glm::vec3 point = worldSpace;
            glm::vec3 ep = point - _freeCam->getPosition();
            float vpLength = glm::dot(ep,normalizedViewVector);
            _distances[i] = vpLength;
        }
        for(int i = 0; i < 25 - 1; i++){
            float currentHighestDistance = _distances[i];
            for(int j = i + 1; j < 25; j++){
                if(_distances[j] > currentHighestDistance){
                    _distances[i] = _distances[j];
                    _distances[j] = currentHighestDistance;
                    currentHighestDistance = _distances[i];
                    int index = _deathParticleIndices[i];
                    _deathParticleIndices[i] = _deathParticleIndices[j];
                    _deathParticleIndices[j] = index;
                }
            }
        }

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _particleIBO[0] );
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort) * 25, _deathParticleIndices);
        glDrawElements( GL_POINTS, _numParticlePoints[0], GL_UNSIGNED_SHORT, (void*)0 );
    }



    _skyboxShaderProgram->useProgram();
    _skyboxShaderProgram->setProgramUniform(_skyboxShaderProgramUniformLocations.time, time);

    // We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
    // The last row and column affect the translation of the skybox (which we don't want to affect)
    _skyboxShaderProgram->setProgramUniform(_skyboxShaderProgramUniformLocations.proj, projMtx);
    _skyboxShaderProgram->setProgramUniform(_skyboxShaderProgramUniformLocations.view, viewMtx);


    // Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
    // where an object is present (a depth of 1.0f will always fail against any object's depth value)
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //// START DRAWING THE GROUND PLANE ////

    _textureShaderProgram->useProgram();
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.time, time);

    glm::mat4 mvpMtx = projMtx * viewMtx * glm::mat4(1.0f);
    _textureShaderProgram->setProgramUniform(_textureShaderUniformLocations.mvpMatrix, mvpMtx);

    glBindTexture(GL_TEXTURE_2D, _texHandles[TEXTURE_ID::METAL]);

    glBindVertexArray( _groundVAO );

// render the mesh triangle strip by triangle strip - each row at a time
    for(unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
    {
        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                       NUM_VERTS_PER_STRIP, // number of indices to render
                       GL_UNSIGNED_SHORT,     // index data type
                       (void*)(sizeof(unsigned short)
                               * NUM_VERTS_PER_STRIP
                               * strip)); // offset to starting index
    }


    _lightingShaderProgram->useProgram();
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.time, time);


    //// END DRAWING THE GROUND PLANE ////

    //// BEGIN DRAWING THE PLANE ////
    glm::mat4 modelMtx(1.0f);
    // we are going to cheat and use our look at point to place our plane so that it is always in view

    modelMtx = glm::translate( modelMtx, _plane->_planeLocation );
    // rotate the plane with our camera theta direction (we need to rotate the opposite direction so that we always look at the back)
    float pie2 = M_PI / 2.0f;

    modelMtx = glm::rotate( modelMtx,  pie2, CSCI441::Y_AXIS );
    // draw our plane now
    glm::vec3 spotLight = _plane->_planeLocation;
    spotLight.y += 20;
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.spotlightPosition, spotLight);


    if(_plane->dead == false){
        _plane->drawPlane( modelMtx, viewMtx, projMtx );
    } else {
        _plane->drawParticles(modelMtx,viewMtx,projMtx);
    }

    //// END DRAWING THE PLANE ////

    //// BEGIN DRAWING THE ENEMIES ////


    for(int i = 0; i < enemies.size(); i++){
        modelMtx = glm::mat4(1.0f);

        modelMtx = glm::translate( modelMtx, enemies[i]->_position );

        if(enemies[i]->dead == false){
            enemies[i]->drawEnemy(modelMtx,viewMtx,projMtx);
        } else {
            enemies[i]->drawParticles(modelMtx,viewMtx,projMtx);
        }
    }


    //// BEGIN DRAWING THE COINS ////

    for(int i = 0; i < coins.size(); i++){
        modelMtx = glm::mat4(1.0f);

        modelMtx = glm::translate( modelMtx, coins[i]->_position );

        if(coins[i]->dead == false){
            coins[i]->drawCoin(modelMtx,viewMtx,projMtx);
        }

    }
}

void A3Engine::_updateScene() {

    //Change cameras
    if(_keys[GLFW_KEY_1]) {
        _currCam = 0;
    }
    if(_keys[GLFW_KEY_2]) {
        _currCam = 1;
        _freeCam2->setPosition(_freeCam->getLookAtPoint()+glm::vec3(0.0f,2.0f,0.0f));
        _freeCam2->recomputeOrientation();
    }


    _particleSystemAngle += 0.01f;
    if(_particleSystemAngle >= 6.28f) {
        _particleSystemAngle -= 6.28f;
    }
    //every 100 frames we want to create a new enemy to increase the difficulty
    timer++;
    if(timer % 100 == 0){
        enemies.push_back(new Enemy(_lightingShaderProgram->getShaderProgramHandle(),
                                    _lightingShaderUniformLocations.mvpMatrix,
                                    _lightingShaderUniformLocations.normMtx,
                                    _lightingShaderUniformLocations.materialColor,
                                    _lightingShaderUniformLocations.modelMatrix));
    }

    //make sure the camera is always following the plane
    _freeCam->recomputeOrientation();

    //update the coin animation
    for(int i = 0; i < coins.size(); i++){
        coins[i]->update();
    }


    float numEnemies = enemies.size();

    //update all enemies and also check collisions with other enemies
    for(int i = 0; i < numEnemies; i++){
        if(enemies[i]->dead == false){
            //update direction to face hero
            enemies[i]->_direction = enemies[i]->_position - _plane->_planeLocation;

            //update position based on direction
            enemies[i]->update();

            //update their height based on the height map
            float height = heightMap[static_cast<int>(enemies[i]->_position.x + 55.0f)][static_cast<int>(enemies[i]->_position.z + 55.0f)];

            enemies[i]->_position.y = height;

            //calcute the angle they must rotate in order to face the hero
            glm::vec3 vectorEnemyToHero = enemies[i]->_position - _plane->_planeLocation;
            glm::vec3 origin = glm::vec3(1,0,0);
            enemies[i]->_angle = atan2(vectorEnemyToHero.z,vectorEnemyToHero.x) - atan2(origin.z,origin.x) + M_PI;
        }
        //update particles if they are dead
        enemies[i]->checkDead();

        //do enemy collision detection within eachother
        for(int j = 0; j < numEnemies; j++){
            if(j == i){
                continue;
            }

            //get the cloest distance to where if any father they aren't touching
            float distance = pow(enemies[i]->size + enemies[j]->size,2);

            //actual distance between the two objects
            float distance2 = pow(enemies[i]->_position.x - enemies[j]->_position.x,2) +
                              pow(enemies[i]->_position.y + enemies[i]->size - enemies[j]->size - enemies[j]->_position.y,2) +
                              pow(enemies[i]->_position.z - enemies[j]->_position.z,2);

            //this means the actual distance is closer than the theorical distance so they are touching
            if(distance2 <= distance){
                //make larger enemy grow larger and faster
                enemies[i]->consumedCount += enemies[j]->consumedCount;
                enemies[i]->size = sqrt(enemies[i]->consumedCount / 2.0f);
                enemies[i]->speed += 0.005;

                //erase smaller enemy
                enemies.erase(enemies.begin() + j);
                numEnemies = enemies.size();
            }
        }
    }


    if(_plane->dead == false){
        //calculate the direction based on the arcball camera angle which is controlled by the mouse
        glm::vec3 d = _freeCam->getPosition() - _freeCam->getLookAtPoint();
        glm::vec3 origin = glm::vec3(1,0,0);
        _plane->_direction = atan2(d.z,d.x) - atan2(origin.z,origin.x) + M_PI;

        //check if the hero is moving
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
        bool move = false;

        //for all WASD set the fact it is moving to true then perform the action and update the camera location
        //move right
        if( _keys[GLFW_KEY_D] && _currCam == 0) {
            move = true;
            _plane->flyRight();
            _freeCam->updateLocation(_plane->_planeLocation);
            right = true;
        }
        // move left
        if( _keys[GLFW_KEY_A] && _currCam == 0) {
            move = true;
            _plane->flyLeft();
            _freeCam->updateLocation(_plane->_planeLocation);
            left = true;
        }
        // move up
        if( _keys[GLFW_KEY_W] && _currCam == 0) {
            move = true;
            _plane->flyForward();
            _freeCam->updateLocation(_plane->_planeLocation);
            up = true;
        }
        // move down
        if( _keys[GLFW_KEY_S] && _currCam == 0) {
            move = true;
            _plane->flyBackward();
            _freeCam->updateLocation(_plane->_planeLocation);
            down = true;
        }

        //get the height of the terrain
        float height = heightMap[static_cast<int>(_plane->_planeLocation.x + 55.0f)][static_cast<int>(_plane->_planeLocation.z + 55.0f)];

        //if hero is on the ground then they can jump
        if( _keys[GLFW_KEY_SPACE] && _plane->_planeLocation.y <= height) {
            _plane->jump();
        }

        //sprint when holding shift
        if( _keys[GLFW_KEY_LEFT_SHIFT] ) {
            _plane->speed = 0.2;
        } else {
            _plane->speed = 0.1;
        }

        //apply gravity to bring hero to the ground
        _plane->gravity(height);
        _freeCam->updateLocation(_plane->_planeLocation);

        //change hero angle based on the buttons pressed
        if(left && up){
            _plane->_legAngle = _plane->_direction - M_PI/4.0;
        } else if(left && down){
            _plane->_legAngle = _plane->_direction - 3*M_PI/4.0;
        } else if(right && up){
            _plane->_legAngle = _plane->_direction + M_PI/4.0;
        } else if(right && down){
            _plane->_legAngle = _plane->_direction + 3*M_PI/4.0;
        } else if(left){
            _plane->_legAngle = _plane->_direction - M_PI / 2.0;
        } else if(right){
            _plane->_legAngle = _plane->_direction + M_PI / 2.0;
        } else if(up){
            _plane->_legAngle = _plane->_direction;
        } else if(down){
            _plane->_legAngle = _plane->_direction - M_PI;
        } else{
            _plane->_legAngle = _plane->_direction;
        }

        //animate the hero is he is moving
        if(move){
            _plane->moveLegs();
        }

        //update particles is dead
        _plane->checkDead();

        _freeCam->setLookAtPoint(_plane->_planeLocation);

        //collision detection with enemies
        for(int j = 0; j < numEnemies; j++){
            //theoretical smallest distance till touching
            float distance = pow(enemies[j]->size * 1.4 + 0.25,2);

            //actual distance till touching
            float distance2 = pow(_plane->_planeLocation.x - enemies[j]->_position.x,2)
                            + pow(_plane->_planeLocation.y + 0.4 - enemies[j]->_position.y - enemies[j]->size,2)
                            + pow(_plane->_planeLocation.z - enemies[j]->_position.z,2);

            //if touching enemy then the hero dies
            if(distance2 <= distance){
                _plane->dead = true;
                _plane->checkDead();
            }
        }

        //collission detection with coins
        int numCoins = coins.size();

        for(int j = 0; j < numCoins; j++){
            //theoretical smallest distance till touching
            float distance = pow(coins[j]->size * 1.25 + 0.5,2);

            //actual distance till touching
            float distance2 = pow(_plane->_planeLocation.x - coins[j]->_position.x,2)
                              + pow(_plane->_planeLocation.y + 1.0 - coins[j]->_position.y - coins[j]->size * 1.25,2)
                              + pow(_plane->_planeLocation.z - coins[j]->_position.z,2);

            //kill the coin if touching
            if(distance2 <= distance){
                coins[j]->dead = true;
                coins.erase(coins.begin() + j);
                numCoins = coins.size();
            }
        }

    } else {
        //if plane is dead update particles and then reset the enemy locations
        _plane->checkDead();
        if(_plane->dead == false){
            for(int i = 0; i < enemies.size(); i++){
                enemies[i]->relocate();
            }
        }
    }

    if(_currCam == 1) {
        //for all WASD set the fact it is moving to true then perform the action and update the camera location
        // move forward
        if( _keys[GLFW_KEY_W] ) {
            _freeCam2->moveForward(0.2f);
        }
        // move backward
        if( _keys[GLFW_KEY_S] ) {
            _freeCam2->moveBackward(0.2f);
        }
    }

    //win once there are no coins left
    if(coins.size() == 0){
        cout << "YOU WIN" << endl;
    }
}

void A3Engine::run() {
    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(_window) ) {	        // check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				        // work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore,
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( _window, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );

        // set the projection matrix based on the window size
        // use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
        glm::mat4 projectionMatrix = glm::perspective( 45.0f, (GLfloat) framebufferWidth / (GLfloat) framebufferHeight, 0.001f, 1000.0f );

        // set up our look at matrix to position our camera
        glm::mat4 viewMatrix;
        if(_currCam == 0) {
            viewMatrix = _freeCam->getViewMatrix();
        }
        if(_currCam == 1) {
            viewMatrix = _freeCam2->getViewMatrix();
        }

        // draw everything to the window
        _renderScene(viewMatrix, projectionMatrix);

        _updateScene();
        glfwSwapBuffers(_window);                       // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                // check for any events and signal to redraw screen


    }
}

//*************************************************************************************
//
// Private Helper FUnctions
GLuint A3Engine::_loadAndRegisterTexture(const char* FILENAME) {
    // our handle to the GPU
    GLuint textureHandle = 0;

    // enable setting to prevent image from being upside down
    stbi_set_flip_vertically_on_load(true);

    // will hold image parameters after load
    GLint imageWidth, imageHeight, imageChannels;
    // load image from file
    GLubyte* data = stbi_load( FILENAME, &imageWidth, &imageHeight, &imageChannels, 0);

    // if data was read from file
    if( data ) {
        const GLint STORAGE_TYPE = (imageChannels == 4 ? GL_RGBA : GL_RGB);

        glGenTextures(1, &textureHandle);
        glBindTexture(GL_TEXTURE_2D, textureHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D,0,STORAGE_TYPE,imageWidth,imageHeight,0,STORAGE_TYPE,GL_UNSIGNED_BYTE,data);

        fprintf( stdout, "[INFO]: %s texture map read in with handle %d\n", FILENAME, textureHandle);

        // release image memory from CPU - it now lives on the GPU
        stbi_image_free(data);
    } else {
        // load failed
        fprintf( stderr, "[ERROR]: Could not load texture map \"%s\"\n", FILENAME );
    }

    // return generated texture handle
    return textureHandle;
}
void A3Engine::_computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const {
    // precompute the Model-View-Projection matrix on the CPU
    glm::mat4 mvpMtx = projMtx * viewMtx * modelMtx;
    // then send it to the shader on the GPU to apply to every vertex
    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.mvpMatrix, mvpMtx);

    glm::mat3 normalMtx = glm::mat3(glm::transpose(glm::inverse(modelMtx)));

    _lightingShaderProgram->setProgramUniform(_lightingShaderUniformLocations.normMtx, normalMtx);
}

//*************************************************************************************
//
// Callbacks

void lab05_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods ) {
    auto engine = (A3Engine*) glfwGetWindowUserPointer(window);

    // pass the key and action through to the engine
    engine->handleKeyEvent(key, action);
}

void lab05_engine_cursor_callback(GLFWwindow *window, double x, double y ) {
    auto engine = (A3Engine*) glfwGetWindowUserPointer(window);

    //get the difference in mousePositions from the last one recorded.
    float lastMousePosition = engine->dy;

    glm::vec2 currMousePosition = glm::vec2(x, y);
    //engine->_mousePosition = currMousePosition;


    // pass the cursor position through to the engine
    //dont want to be able to move the arcball camera and be able to zoom in at the same time cause that looks weird
    engine->handleCursorPositionEvent(glm::vec2(x, y));
}

void lab05_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods ) {
    auto engine = (A3Engine*) glfwGetWindowUserPointer(window);

    // pass the mouse button and action through to the engine
    engine->handleMouseButtonEvent(button, action);
}
