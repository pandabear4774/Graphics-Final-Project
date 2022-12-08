#ifndef LAB05_LAB05_ENGINE_HPP
#define LAB05_LAB05_ENGINE_HPP

#include <CSCI441/FreeCam.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/ShaderProgram.hpp>
#include "CameraRound.h"
#include "Enemy.hpp"
#include "Coin.hpp"
#include "Plane.hpp"

#include <vector>

class A3Engine : public CSCI441::OpenGLEngine {
public:
    A3Engine();
    ~A3Engine();

    void run() final;

    /// \desc handle any key events inside the engine
    /// \param key key as represented by GLFW_KEY_ macros
    /// \param action key event action as represented by GLFW_ macros
    void handleKeyEvent(GLint key, GLint action);

    /// \desc handle any mouse button events inside the engine
    /// \param button mouse button as represented by GLFW_MOUSE_BUTTON_ macros
    /// \param action mouse event as represented by GLFW_ macros
    void handleMouseButtonEvent(GLint button, GLint action);

    /// \desc handle any cursor movement events inside the engine
    /// \param currMousePosition the current cursor position
    void handleCursorPositionEvent(glm::vec2 currMousePosition);

    /// \desc value off-screen to represent mouse has not begun interacting with window yet
    static constexpr GLfloat MOUSE_UNINITIALIZED = -9999.0f;

    /// variables describe the mouse state and how the user is moving the mouse
    GLint _leftMouseButtonState;
    glm::vec2 _mousePosition;
    float dy;
    int timer = 0;

    ///information about the keys
    static constexpr GLuint NUM_KEYS = GLFW_KEY_LAST;
    GLboolean _keys[NUM_KEYS];

    ///have a free cam variable which is actually an arcball camera
    CameraRound* _freeCam;
private:
    void _setupGLFW() final;
    void _setupOpenGL() final;
    void _setupShaders() final;
    void _setupBuffers() final;
    void _setupScene() final;

    void _cleanupBuffers() final;
    void _cleanupShaders() final;

    /// \desc draws everything to the scene from a particular point of view
    /// \param viewMtx the current view matrix for our camera
    /// \param projMtx the current projection matrix for our camera
    void _renderScene(glm::mat4 viewMtx, glm::mat4 projMtx) const;
    /// \desc handles moving our FreeCam as determined by keyboard input
    void _updateScene();
    void _updateScene2();
    float getHeight(int, int);
    int width, height;

    /// \desc tracks the number of different keys that can be present as determined by GLFW

    /// \desc boolean array tracking each key state.  if true, then the key is in a pressed or held
    /// down state.  if false, then the key is in a released state and not being interacted with


    /// \desc last location of the mouse in window coordinates

    /// \desc current state of the left mouse button


    GLuint _gridVAO;
    /// \desc tracks the number of vertices comprising the ground plane
    GLuint _numGridPoints;
    /// \desc color to draw the ground plane grid
    glm::vec3 _gridColor;



    /// \desc the static fixed camera in our world

    /// \desc pair of values to store the speed the camera can move/rotate.
    /// \brief x = forward/backward delta, y = rotational delta
    glm::vec2 _cameraSpeed;

    /// \desc our plane model
    Plane* _plane;

    std::vector<Enemy*> enemies;

    std::vector<Coin*> coins;

    /// \desc the size of the world (controls the ground size and locations of buildings)
    static constexpr GLfloat WORLD_SIZE = 55.0f;
    /// \desc VAO for our ground
    GLuint _groundVAO;
    /// \desc the number of points that make up our ground object
    GLsizei _numGroundPoints;

    /// \desc creates the ground VAO
    void _createGroundBuffers();

    /// \desc smart container to store information specific to each building we wish to draw
    struct BuildingData {
        /// \desc transformations to position and size the building
        glm::mat4 modelMatrix;
        /// \desc color to draw the building
        glm::vec3 color;
    };
    /// \desc information list of all the buildings to draw
    std::vector<BuildingData> _buildings;

    float heightMap[111][111];
    /// \desc generates building information to make up our scene
    void _generateEnvironment();

    /// \desc shader program that performs lighting
    CSCI441::ShaderProgram* _lightingShaderProgram = nullptr;   // the wrapper for our shader program
    /// \desc stores the locations of all of our shader uniforms
    struct LightingShaderUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint mvpMatrix;
        /// \desc material diffuse color location
        GLint materialColor;

        GLuint normMtx;

        GLuint spotlightPosition;

        GLuint modelMatrix;

        GLuint lightDirection;

        GLuint lightColor;

    } _lightingShaderUniformLocations;
    /// \desc stores the locations of all of our shader attributes
    struct LightingShaderAttributeLocations {
        /// \desc vertex position location
        GLint vPos;

        GLuint vecNormal;

    } _lightingShaderAttributeLocations;

    CSCI441::ShaderProgram* _textureShaderProgram = nullptr;   // the wrapper for our shader program
    /// \desc stores the locations of all of our shader uniforms
    struct TextureShaderUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint mvpMatrix;

        GLuint textMap;

    } _textureShaderUniformLocations;
    /// \desc stores the locations of all of our shader attributes
    struct TextureShaderAttributeLocations {
        /// \desc vertex position location
        GLint vPos;
        /// \desc vertex normal location
        /// \note not used in this lab
        GLint vNormal;

        GLuint textCord;

    } _textureShaderAttributeLocations;

    CSCI441::ShaderProgram* _skyboxShaderProgram = nullptr;
    struct SkyboxShaderProgramUniformLocations {
        /// \desc precomputed MVP matrix location
        GLint view;
        /// \desc the color to apply location
        GLint proj;

        GLint skybox;
    } _skyboxShaderProgramUniformLocations;

    unsigned int cubemapTexture;
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;


    int NUM_STRIPS;
    int NUM_VERTS_PER_STRIP;

    /// \desc total number of textures in our scene
    static constexpr GLuint NUM_TEXTURES = 2;
    /// \desc used to index through our texture array to give named access
    enum TEXTURE_ID {
        /// \desc metal texture
        METAL = 0,
        /// \desc Mines logo texture
        MINES = 1
    };
    /// \desc texture handles for our textures
    GLuint _texHandles[NUM_TEXTURES];

    GLuint _loadAndRegisterTexture(const char* FILENAME);

    /// \desc precomputes the matrix uniforms CPU-side and then sends them
    /// to the GPU to be used in the shader for each vertex.  It is more efficient
    /// to calculate these once and then use the resultant product in the shader.
    /// \param modelMtx model transformation matrix
    /// \param viewMtx camera view matrix
    /// \param projMtx camera projection matrix
    void _computeAndSendMatrixUniforms(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx) const;
};

void lab05_engine_keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods );
void lab05_engine_cursor_callback(GLFWwindow *window, double x, double y );
void lab05_engine_mouse_button_callback(GLFWwindow *window, int button, int action, int mods );

#endif // LAB05_LAB05_ENGINE_HPP
