#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"

const int WIDTH = 800, HEIGHT = 600;
Camera camera(glm::vec3(0, 0.2f, 2.5f), glm::vec3(0, 1, 0));

// ------------------- CALLBACKS -------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.mouseCallback(window, xpos, ypos);
}
// --------------------------------------------------


int main(void)
{
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    window = glfwCreateWindow(WIDTH, HEIGHT, "Depth Buffer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Make sure to initialize GLAD after context has been set */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glEnable(GL_DEPTH_TEST);

    // DATA
    float cubeVerts[] = {
         // pos               // uv
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    float planeVerts[] = {
        // positions          // uv
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    unsigned int cubeVAO, cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int planeVAO, planeVBO;
    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts), planeVerts, GL_STATIC_DRAW);

    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Shader init
    unsigned int vShader = Shaders::createShader(GL_VERTEX_SHADER, "shaders/shader.vert");
    unsigned int fShader = Shaders::createShader(GL_FRAGMENT_SHADER, "shaders/shader.frag");
    unsigned int program = Shaders::createAndLinkProgram({ vShader, fShader });
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    //Texture init
    unsigned int marbleTexture = Texture::load("assets/marble.jpg", GL_RGB);
    unsigned int metalTexture = Texture::load("assets/metal.png", GL_RGB);
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "tex"), 0);

    float far = 30.0f, near = 0.1f;
    glm::mat4 projection = glm::perspective(45.0f, (float)WIDTH / (float)HEIGHT, near, far);
    glUniform1f(glGetUniformLocation(program, "near"), near);
    glUniform1f(glGetUniformLocation(program, "far"), far);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set view matrix
        camera.update(window);

        glUseProgram(program);

        // Cube 1
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.01f, -2.0f));
        glm::mat4 mvp = projection * camera.getViewMatrix() * model;
        glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, marbleTexture);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(cubeVerts) / sizeof(float));

        // Cube 2
        model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.01f, 1.5f));
        mvp = projection * camera.getViewMatrix() * model;
        glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, sizeof(cubeVerts) / sizeof(float));

        // Plane
        mvp = projection * camera.getViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, metalTexture);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(planeVerts) / sizeof(float));

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}