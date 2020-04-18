#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <Shader.hpp>

const unsigned int WIDTH = 1920;
const unsigned int HEIGHT = 1080;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void process_input(GLFWwindow *window);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
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

    // vsync
    glfwSwapInterval(1);

    // Shader
    Shader ourShader("../shaders/shader.vert", "../shaders/shader.frag");

    // mesh
    float vertices[] = {
            0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, //TR
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, //BR
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,//BL
            -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f //TL
    };

    unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) nullptr);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            {
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);

    // wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // delta time
    auto currentTime = (float) glfwGetTime();
    float deltaTime = 0.0f;
    float lastTime = 0.0f;
    int frameSamplesNum = 100;
    int frames = 0;
    float fpsAvg = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        currentTime = (float) glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (frames++ < frameSamplesNum) {
            fpsAvg += (1 / deltaTime);
        }
        if (frames == frameSamplesNum) {
            fpsAvg /= (float) frames;
            std::cout << "FPS: " << fpsAvg << std::endl;
            fpsAvg = 0.0f;
            frames = 0;
        }


        // input
        process_input(window);

        // rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // setting uniforms in shader
        auto timeValue = (float) glfwGetTime();
        float greenValue = ((float) sin(timeValue) / 2.0f) + 0.5f;

        ourShader.use();
        {
            ourShader.setFloat4("ourColor", 0.0f, greenValue, 0.0f, 1.0f);

            glBindVertexArray(VAO);
            {
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            }
        }

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

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
void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}