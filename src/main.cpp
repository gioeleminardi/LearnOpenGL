#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

#include "Shader.hpp"
#include "Camera.hpp"
#include "stb_image.hpp"

const unsigned int WIDTH = 1600;
const unsigned int HEIGHT = 1200;

const std::string AppName{"LearnOpenGL"};

float mouseSensitivity{0.1f};
float yaw{-90.0f};
float pitch{0.0f};
float fov{45.0f};
bool firstMouse{true};
float lastX{(float) WIDTH / 2};
float lastY{(float) HEIGHT / 2};
Camera mainCamera(glm::vec3(0.0f, 0.0f, 4.0f));
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void process_input(GLFWwindow *window, const float &deltaTime);

void mouse_callback(GLFWwindow *window, double xPos, double yPos);

void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);

unsigned int load_texture(const char *filename);

unsigned int load_textureA(const char *filename);

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // depth test
    glEnable(GL_DEPTH_TEST);
    // vsync
    glfwSwapInterval(1);

    // Shader
    Shader lightShader("../shaders/light.vert", "../shaders/light.frag");
    Shader lampShader("../shaders/nolight.vert", "../shaders/nolight.frag");

    float vertices[] = {
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f, 0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,

            -0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,

            0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, -0.5f
    };

    // cube
    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);

    unsigned int cubeVBO;
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    {
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);
            glEnableVertexAttribArray(0);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

    // light source cube
    unsigned int lightSourceVAO;
    glGenVertexArrays(1, &lightSourceVAO);

    glBindVertexArray(lightSourceVAO);
    {
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);
            glEnableVertexAttribArray(0);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

    // wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float currentTime;
    float deltaTime;
    float lastTime{};
    float fpsAvg{};
    int frameSamplesNum = 50;
    int frames{};
    std::stringstream ss_fps;

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

        // input
        process_input(window, deltaTime);

        // rendering commands
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 projection = glm::perspective(glm::radians(mainCamera.getZoom()), (float) WIDTH / (float) HEIGHT,
                                                0.1f, 100.0f);
        glm::mat4 view = mainCamera.getViewMatrix();


        glm::mat4 model = glm::mat4(1.0f);

        lightShader.use();
        lightShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        lightShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("model", model);

        glBindVertexArray(cubeVAO);
        {
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);


        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        lampShader.setMat4("model", model);

        glBindVertexArray(lightSourceVAO);
        {
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightSourceVAO);
    glDeleteBuffers(1, &cubeVBO);

    glfwTerminate();

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

unsigned int load_texture(const char *filename) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *textureData = stbi_load(filename, &width, &height, &nrChannels, 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (textureData) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cerr << "Error loading texture\n";
        }
    }
    stbi_image_free(textureData);
    return texture;
}

unsigned int load_textureA(const char *filename) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *textureData = stbi_load(filename, &width, &height, &nrChannels, 0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (textureData) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cerr << "Error loading texture\n";
        }
    }
    stbi_image_free(textureData);
    return texture;
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















