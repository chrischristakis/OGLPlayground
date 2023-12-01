#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utils.h"
#include "Constants.h"

// ------------------- CALLBACKS -------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    window = glfwCreateWindow(900, 800, "Window", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    /* Make sure to initialize GLAD after context has been set */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // VBO STUFF
    unsigned int quadVBO;
    unsigned int quadVAO;

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Constants::quadVerts), Constants::quadVerts, GL_STATIC_DRAW);

    // Positions for each quad (From the center of the quad)
    glm::vec2 quad_positions[100];
    int index = 0;
    float offset = 0.1f;
    float space_between = 0.15f;
    for (int x = -5; x < 5; x++)
        for (int y = -5; y < 5; y++)
            quad_positions[index++] = glm::vec2(x * space_between + offset, y * space_between + offset);

    glm::mat4 instance_model_matrices[100];
    float scale_factor = 0.5f; // Make every quad 2x bigger.
    for (int i = 0; i < 100; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(quad_positions[i], 0.0));
        model = glm::rotate(model, glm::radians((float)i * 10.0f), glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(scale_factor, scale_factor, scale_factor));
        instance_model_matrices[i] = model;
    }

    unsigned int matrixVBO;
    glGenBuffers(1, &matrixVBO);
    glBindBuffer(GL_ARRAY_BUFFER, matrixVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instance_model_matrices), instance_model_matrices, GL_STATIC_DRAW);

    std::cout << sizeof(instance_model_matrices) << std::endl;

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, matrixVBO);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(4 * 0 * sizeof(float)));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(4 * 1 * sizeof(float)));
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(4 * 2 * sizeof(float)));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(4 * 3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(quadVAO);
    

    // Shader stuff
    unsigned int vShader = Utils::createShader(GL_VERTEX_SHADER, "shaders/shader.vs");
    unsigned int fShader = Utils::createShader(GL_FRAGMENT_SHADER, "shaders/shader.fs");
    unsigned int program = Utils::createAndLinkProgram({ vShader, fShader });
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        // Rendering here
        glUseProgram(program);
        glBindVertexArray(quadVAO); 
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}