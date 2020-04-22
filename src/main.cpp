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
    Shader ourShader("../shaders/shader.vert", "../shaders/shader.frag");

    float vertices[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
    };

    unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) nullptr);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

    glm::vec3 cubePositions[] = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f, 2.0f, -2.5f),
            glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    // wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // generate texture
    unsigned int texture1 = load_texture("../textures/container.jpg");
    unsigned int texture2 = load_textureA("../textures/awesomeface.png");

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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // clip space
        glm::mat4 projection(1.0f);
        projection = glm::perspective(glm::radians(mainCamera.getZoom()), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);

        ourShader.use();
        {
            ourShader.setInt("texture1", 0);
            ourShader.setInt("texture2", 1);
            ourShader.setMat4("view", mainCamera.getViewMatrix());
            ourShader.setMat4("projection", projection);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2);

            glBindVertexArray(VAO);
            {
                for (unsigned int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); ++i) {
                    glm::mat4 model(1.0f);
                    model = glm::translate(model, cubePositions[i]);
                    model = glm::rotate(model, (float) glfwGetTime() * glm::radians(20.0f * i + 10),
                                        glm::vec3(0.5f, 1.0f, 0.0f));
                    ourShader.setMat4("model", model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

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















