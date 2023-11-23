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
#include "Constants.h"

const int WIDTH = 1200, HEIGHT = 1000;
Camera camera(glm::vec3(0, 0.2f, 2.5f), glm::vec3(0, 1, 0));

glm::mat4 projection = glm::perspective(45.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 50.0f);

unsigned int cubeVAO, cubeVBO;
unsigned int planeVAO, planeVBO;
unsigned int program, outlineProgram;
unsigned int marbleTexture, metalTexture;

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

void drawPlane() {
    glUseProgram(program);
    glm::mat4 mvp = projection * camera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);
    glBindVertexArray(planeVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, metalTexture);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(Constants::planeVerts) / sizeof(float));
    glBindVertexArray(0);
}

void drawCube(glm::vec3 position) {
    glUseProgram(program);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);  // To avoid Z fighting
    glm::mat4 mvp = projection * camera.getViewMatrix() * model;
    glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);
    glBindVertexArray(cubeVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, marbleTexture);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(Constants::cubeVerts) / sizeof(float));
    glBindVertexArray(0);
}

void drawScaledCube(glm::vec3 position, float scaleFactor=1.1f) {
    glUseProgram(outlineProgram);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position) * 
                      glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor, scaleFactor, scaleFactor));
    glm::mat4 mvp = projection * camera.getViewMatrix() * model;
    glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);
    glBindVertexArray(cubeVAO);

    glDrawArrays(GL_TRIANGLES, 0, sizeof(Constants::cubeVerts) / sizeof(float));
    glBindVertexArray(0);
}

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    window = glfwCreateWindow(WIDTH, HEIGHT, "Stencil Buffer", NULL, NULL);
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

    // Cube
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Constants::cubeVerts), Constants::cubeVerts, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Plane
    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Constants::planeVerts), Constants::planeVerts, GL_STATIC_DRAW);

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
    program = Shaders::createAndLinkProgram({ vShader, fShader });
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    vShader = Shaders::createShader(GL_VERTEX_SHADER, "shaders/outline.vert");
    fShader = Shaders::createShader(GL_FRAGMENT_SHADER, "shaders/outline.frag");
    outlineProgram = Shaders::createAndLinkProgram({ vShader, fShader });
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    //Texture init
    marbleTexture = Texture::load("assets/marble.jpg", GL_RGB);
    metalTexture = Texture::load("assets/metal.png", GL_RGB);
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "tex"), 0);

    glm::vec3 cube1Pos = glm::vec3(0.0f, 0.2f, 0.0f);
    glm::vec3 cube2Pos = glm::vec3(1.0f, 0.2f, 3.0f);

    // Render loop
    glClearColor(0.06f, 0.07f, 0.08f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        camera.update(window);

        glStencilMask(0xFF);  // allow glClear to write to the stencil buffer and clear it
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE); // Replace if stencil & depth test passes

        // Normal scene without outlines
        drawPlane();

        // 1st render pass:
        // Draw out things we'd like to outline and write to stencil buffer
        // ------------------------------------------------------------------------------
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // Always pass, write a 1 where our fragments are.
        glStencilMask(0xFF); // Enable writing to stencil
        drawCube(cube1Pos);
        drawCube(cube2Pos);
        glStencilMask(0x00);

        // 2nd render pass:
        // Draw scaled up single-colour version of cubes, do not draw on top of stencil buffer.
        // ------------------------------------------------------------------------------
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // Only draw when stencil != 1
        glDepthFunc(GL_ALWAYS);  // Draw ontop of everything else
        drawScaledCube(cube1Pos);
        drawScaledCube(cube2Pos);
        glDepthFunc(GL_LESS); // Default depth testing

        glDisable(GL_STENCIL_TEST); // We're done with stencil test.

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}