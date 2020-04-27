#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>
#include <Model.hpp>

#include "Shader.hpp"
#include "Camera.hpp"
#include "stb_image.hpp"

const unsigned int WIDTH = 1600;
const unsigned int HEIGHT = 1200;

const std::string AppName{"LearnOpenGL"};

int display_w, display_h;
float mouseSensitivity{0.1f};
float yaw{-90.0f};
float pitch{0.0f};
float fov{45.0f};
bool firstMouse{true};
float lastX{(float) WIDTH / 2};
float lastY{(float) HEIGHT / 2};
Camera mainCamera(glm::vec3(0.0f, 3.0f, 8.0f), glm::vec3(0.0f, 1.0f, 0.0f), Camera::YAW, -20.0f);
glm::vec3 lightPos(1.2f, 1.0f, 10.0f);


void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void process_input(GLFWwindow *window, const float &deltaTime);

void mouse_callback(GLFWwindow *window, double xPos, double yPos);

void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, AppName.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // vsync
    glfwSwapInterval(1);

    // Model
    //Model nanosuit("../models/nanosuit/nanosuit.obj");
    //Model sponza("../models/sponza/sponza.obj");

    Model planet("../models/planet/planet.obj");
    Model rock("../models/rock/rock.obj");


    // Shader
    Shader modelLoadingShader("../shaders/model_loading.vert", "../shaders/model_loading.frag");

    // wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    unsigned int amount = 3000;
    glm::mat4 *modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // initialize random seed
    float radius = 50.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float) i / (float) amount * 360.0f;
        float displacement = (rand() % (int) (2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int) (2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int) (2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    float currentTime;
    float deltaTime;
    float lastTime{};
    float fpsAvg{};
    int frameSamplesNum = 50;
    int frames{};
    std::stringstream ss_fps;
    glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        currentTime = (float) glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (frames++ < frameSamplesNum) {
            fpsAvg += (1 / deltaTime);
        }
        if (frames == frameSamplesNum) {
            ss_fps.str(std::string());
            fpsAvg /= (float) frames;
            ss_fps << AppName << " - ";
            ss_fps << fpsAvg << " fps";
            glfwSetWindowTitle(window, ss_fps.str().c_str());
            fpsAvg = 0.0f;
            frames = 0;
        }

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // input
        process_input(window, deltaTime);

        glm::mat4 projection = glm::perspective(glm::radians(mainCamera.getZoom()),
                                                (float) display_w / (float) display_h,
                                                0.1f, 1000.0f);
        glm::mat4 view = mainCamera.getViewMatrix();

        modelLoadingShader.use();
        modelLoadingShader.setMat4("projection", projection);
        modelLoadingShader.setMat4("view", view);

        modelLoadingShader.setVec3("viewPos", mainCamera.getPosition());
        modelLoadingShader.setFloat("material.shininess", 32.0f);

        modelLoadingShader.setVec3("dLight_.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        modelLoadingShader.setVec3("dLight_.base.ambient", glm::vec3(0.3f, 0.3f, 0.3f));
        modelLoadingShader.setVec3("dLight_.base.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        modelLoadingShader.setVec3("dLight_.base.specular", glm::vec3(0.8f, 0.8f, 0.8f));

        modelLoadingShader.setVec3("pLights_[0].position", glm::vec3(0.7f, 0.2f, 2.0f));
        modelLoadingShader.setVec3("pLights_[0].base.ambient", glm::vec3(0.3f, 0.3f, 0.3f));
        modelLoadingShader.setVec3("pLights_[0].base.diffuse", glm::vec3(0.7f, 0.7f, 0.7f));
        modelLoadingShader.setVec3("pLights_[0].base.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        modelLoadingShader.setFloat("pLights_[0].attenuation.constant", 1.0f);
        modelLoadingShader.setFloat("pLights_[0].attenuation.linear", 0.09f);
        modelLoadingShader.setFloat("pLights_[0].attenuation.quadratic", 0.032f);

        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
        //modelAngle += rotatingSpeed * deltaTime;
        //modelAngle = glm::mod(modelAngle, 360.0f);
        //model = glm::rotate(model, glm::radians(modelAngle), glm::vec3(0.0f, -1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.2f));

        // draw planet
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        modelLoadingShader.setMat4("model", model);
        planet.Draw(modelLoadingShader);

        // draw meteorites
        for (unsigned int i = 0; i < amount; i++) {
            modelLoadingShader.setMat4("model", modelMatrices[i]);
            rock.Draw(modelLoadingShader);
        }

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

/**
 * Called every time the window is resized
 * @param window
 * @param width
 * @param height
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    glfwGetFramebufferSize(window, &display_w, &display_h);
}

/**
 * Called every frame for processing input queue
 * @param window
 */
void process_input(GLFWwindow *window, const float &deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(Camera::RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow *window, double xPos, double yPos) {
    if (firstMouse) {
        lastX = (float) xPos;
        lastY = (float) yPos;
        firstMouse = false;
    }
    float xOffset = (float) xPos - lastX;
    float yOffset = lastY - (float) yPos;

    lastX = (float) xPos;
    lastY = (float) yPos;

    mainCamera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow *window, double xOffset, double yOffset) {
    mainCamera.ProcessMouseScroll((float) yOffset);
}















