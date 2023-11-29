#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ShaderProgram.h"
#include "Camera.h"
#include "Constants.h"
#include "Texture.h"
#include "Model.h"

const int WIDTH = 1200, HEIGHT = 1000;
Camera camera(glm::vec3(0, 0.2f, 2.5f), glm::vec3(0, 1, 0));
bool wireframe = false;

// ------------------- CALLBACKS -------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, wireframe? GL_LINE : GL_FILL);
        wireframe = !wireframe;
    }
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

    //  ---------------------- MODEL LOADING STUFF ----------------------
    Model backpackModel("assets/backpack/backpack.obj");
    // ------------------------------------------------------------------ 

    unsigned int vShader = Shaders::createShader(GL_VERTEX_SHADER, "shaders/model.vert");
    unsigned int fShader = Shaders::createShader(GL_FRAGMENT_SHADER, "shaders/model.frag");
    unsigned int program = Shaders::createAndLinkProgram({ vShader, fShader });
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    glm::mat4 projection = glm::perspective(45.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 50.0f);

    glm::vec3 lightDir = glm::vec3(-0.2f, -1.0f, -0.3f);
    glm::vec3 lightColor = glm::vec3(0.5f, 0.68f, 0.65f);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set view matrix
        camera.update(window);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 mvp = projection * camera.getViewMatrix() * model;
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
        glUniform3fv(glGetUniformLocation(program, "lightDir"), 1, &lightDir[0]);
        glUniform3fv(glGetUniformLocation(program, "viewPos"), 1, &camera.position[0]);
        glUniform3fv(glGetUniformLocation(program, "lightColor"), 1, &lightColor[0]);
        backpackModel.draw(program);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}