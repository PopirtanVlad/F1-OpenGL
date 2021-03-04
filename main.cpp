#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Camera.hpp"
#include "Window.h"
#include "Shader.hpp"
#include "Model3D.hpp"

#include <iostream>
#include "SkyBox.hpp"

using namespace std;

// window
gps::Window myWindow;

// matrices
glm::mat4 carModel;
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLuint modelLoc;
GLuint viewLoc;
GLuint projectionLoc;
GLuint normalMatrixLoc;
GLuint lightDirLoc;
GLuint lightColorLoc;
GLuint viewPosLoc;
GLuint skyBoxID;
GLuint lightPosLoc;
// camera
Camera myCamera(glm::vec3(-2.83863f, 2.56613f, 43.3783));
GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];
float lastMouseY = 1024 / 2, lastMouseX = 768 / 2;
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
// models
gps::Model3D grass;
gps::Model3D lamps;
gps::Model3D track;
gps::Model3D terrain;
gps::Model3D castleExterior; gps::Model3D castleInterior; gps::Model3D castleDoors;
gps::Model3D castleBridge;
gps::Model3D trackSides;
gps::Model3D playerCar;
gps::Model3D buildings;
gps::Model3D fence1; gps::Model3D fence2; gps::Model3D fence3; gps::Model3D fence4; gps::Model3D fence5; gps::Model3D fence6;
gps::Model3D bannerPillars; gps::Model3D banner;
GLfloat angle;
std::vector < const GLchar*> faces;
// shaders
gps::Shader myBasicShader;
gps::Shader skyBoxShader;
int day = true;;
// skybox
gps::SkyBox mySkybox;
//Car
glm::vec3 carCoordinates=glm::vec3(-3.13745f, 2.8337f,43.303f);
glm::vec3 carTranslateValues = glm::vec3(0.0f, 0.0f, 0.0f);
GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (pressedKeys[GLFW_KEY_V]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (pressedKeys[GLFW_KEY_B]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (pressedKeys[GLFW_KEY_N]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos; 
    
    lastMouseX = xpos;
    lastMouseY = ypos;

    myCamera.ProcessMouseMovement(xoffset, yoffset);
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

void processMovement() {
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(MOVE_FORWARD, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(MOVE_BACKWARD, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(MOVE_LEFT, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(MOVE_RIGHT, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }
    if (pressedKeys[GLFW_KEY_UP]) {
         carTranslateValues.x += 0.02f;
         carCoordinates += carTranslateValues;
         carModel = glm::translate(carModel, carTranslateValues);
    }
    if (pressedKeys[GLFW_KEY_DOWN]) {
        carTranslateValues.x -= 0.02f;
        carCoordinates -= carTranslateValues;
        carModel = glm::translate(carModel, carTranslateValues);
              
    }
    if (pressedKeys[GLFW_KEY_LEFT]) {
        carModel = glm::translate(carModel, carCoordinates);
        carModel = glm::rotate(carModel, glm::radians(10.0f), glm::vec3(0, 1, 0));
        carModel = glm::translate(carModel, -carCoordinates);
        carTranslateValues.x -= 0.02f;
        carTranslateValues.z += 0.02f;
        carCoordinates += carTranslateValues;
    }
    if (pressedKeys[GLFW_KEY_RIGHT]) {
        carModel = glm::translate(carModel, carCoordinates);
        carModel = glm::rotate(carModel, glm::radians(-10.0f), glm::vec3(0, 1, 0));
        carModel = glm::translate(carModel, -carCoordinates);
        carTranslateValues.x += 0.02f;
        carTranslateValues.z -= 0.02f;
        carCoordinates += carTranslateValues;
    }
}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "F1");
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void setWindowCallbacks() {
    glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
   
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_BACK); // cull back face
    glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    track.LoadModel("models/track/track.obj");
    trackSides.LoadModel("models/trackSides/trackSides.obj");
    castleInterior.LoadModel("models/castle/castleInterior.obj");
    castleExterior.LoadModel("models/castle/castleExterior.obj");
    castleDoors.LoadModel("models/castle/doorsAndWindows.obj");
    castleBridge.LoadModel("models/castleSectionBridge/castleSectionBridge.obj");
    lamps.LoadModel("models/lamps/lamps.obj");
    grass.LoadModel("models/terrain/grass.obj");
    terrain.LoadModel("models/terrain/terrain.obj");
    playerCar.LoadModel("models/playerCar/playerCar.obj");
    fence1.LoadModel("models/fence/fence1.obj");
    fence2.LoadModel("models/fence/fence2.obj");
    fence3.LoadModel("models/fence/fence3.obj");
    fence4.LoadModel("models/fence/fence4.obj");
    fence5.LoadModel("models/fence/fence5.obj");
    fence6.LoadModel("models/fence/fence6.obj");
    banner.LoadModel("models/banner/banner.obj");
    bannerPillars.LoadModel("models/banner/bannerPillars.obj");
    faces.push_back("skybox/right.tga");
    faces.push_back("skybox/left.tga");
    faces.push_back("skybox/top.tga");
    faces.push_back("skybox/bottom.tga");
    faces.push_back("skybox/back.tga");
    faces.push_back("skybox/front.tga");

    buildings.LoadModel("models/buildings/buildings.obj");

}

void initShaders() {
    myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    skyBoxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
}

void initUniforms() {
    myBasicShader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 200.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
    //set light color
    lightColor = glm::vec3(0.2, 0.2f, 0.2f); 
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    // send viewer position
    viewPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "viewPos");
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(myCamera.Position));
    //set point lights
    glm::vec3 lightPos = glm::vec3(-0.044601f,5.6641f,41.923f);
    lightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[0]");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    
    lightPos = glm::vec3(-107.431f, 3.9074f, 24.314f); 
    lightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[1]");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

    lightPos = glm::vec3(-99.0141f, 4.5356f, 13.485f); 
    lightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLights[2]");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    //set point light color
    lightColor = glm::vec3(1.0f, 0.2f, 0.2f); //MidnightBlue
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightsColor[0]");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    
    lightColor = glm::vec3(0.133, 0.545, 0.133); //ForestGreen
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightsColor[1]");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightsColor[2]");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    skyBoxShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));

    projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width/ (float)myWindow.getWindowDimensions().width, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(projection));

}

void renderBanner(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    banner.Draw(shader);
    bannerPillars.Draw(shader);
}


void renderTerrain(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    
    terrain.Draw(shader);
}

void renderBuildings(gps::Shader shader) {
    shader.useShaderProgram();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    grass.Draw(shader);
    buildings.Draw(shader);
}

void renderLamps(gps::Shader shader) {
    shader.useShaderProgram();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    lamps.Draw(shader);
}


void renderFence(gps::Shader shader) {
    shader.useShaderProgram();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));


    fence1.Draw(shader);
    fence2.Draw(shader);
    fence3.Draw(shader);
    fence4.Draw(shader);
    fence5.Draw(shader);
    fence6.Draw(shader);
}
void renderCar(gps::Shader shader) {
    shader.useShaderProgram();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(carModel));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    playerCar.Draw(shader);
}

void renderCastle(gps::Shader shader) {
    shader.useShaderProgram();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    castleInterior.Draw(shader);
    castleDoors.Draw(shader);
    castleExterior.Draw(shader);
}

void renderCastleSectionBridge(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    castleBridge.Draw(shader);
}
void renderTrack(gps::Shader shader) {
    shader.useShaderProgram();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    trackSides.Draw(shader);
    track.Draw(shader);
}



void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //render the scene

    // render the teapot
    mySkybox.Draw(skyBoxShader,view,projection);
    renderTrack(myBasicShader);
    renderCastleSectionBridge(myBasicShader);
    renderCastle(myBasicShader);
    renderTerrain(myBasicShader);
    renderCar(myBasicShader);
    renderFence(myBasicShader);
    renderBanner(myBasicShader);
    renderBuildings(myBasicShader);
    renderLamps(myBasicShader);
    mySkybox.Load(faces);
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char* argv[]) {

    try {
        initOpenGLWindow();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
    initModels();
    initShaders();
    initUniforms();
    setWindowCallbacks();

    glCheckError();
    // application loop
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(myWindow.getWindow());

        glCheckError();
    }

    cleanup();

    return EXIT_SUCCESS;
}
