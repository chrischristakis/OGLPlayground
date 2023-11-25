#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Constants.h"

const int WIDTH = 1200, HEIGHT = 1000;
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

    unsigned int cubeVAO, cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Constants::cubeVerts), Constants::cubeVerts, GL_STATIC_DRAW);

    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //  ------------------ SKYBOX --------------------

    float skyboxVertices[] = {
        // Pos                
        -1.0f,  1.0f, -1.0f,  
        -1.0f, -1.0f, -1.0f,  
         1.0f, -1.0f, -1.0f,  
         1.0f, -1.0f, -1.0f,  
         1.0f,  1.0f, -1.0f,  
        -1.0f,  1.0f, -1.0f,  

        -1.0f, -1.0f,  1.0f,  
        -1.0f, -1.0f, -1.0f,  
        -1.0f,  1.0f, -1.0f,  
        -1.0f,  1.0f, -1.0f,  
        -1.0f,  1.0f,  1.0f,  
        -1.0f, -1.0f,  1.0f,  

         1.0f, -1.0f, -1.0f,  
         1.0f, -1.0f,  1.0f,  
         1.0f,  1.0f,  1.0f,  
         1.0f,  1.0f,  1.0f,  
         1.0f,  1.0f, -1.0f,  
         1.0f, -1.0f, -1.0f,  

        -1.0f, -1.0f,  1.0f,  
        -1.0f,  1.0f,  1.0f,  
         1.0f,  1.0f,  1.0f,  
         1.0f,  1.0f,  1.0f,  
         1.0f, -1.0f,  1.0f,  
        -1.0f, -1.0f,  1.0f,  

        -1.0f,  1.0f, -1.0f,  
         1.0f,  1.0f, -1.0f,  
         1.0f,  1.0f,  1.0f,  
         1.0f,  1.0f,  1.0f,  
        -1.0f,  1.0f,  1.0f,  
        -1.0f,  1.0f, -1.0f,  

        -1.0f, -1.0f, -1.0f,  
        -1.0f, -1.0f,  1.0f,  
         1.0f, -1.0f, -1.0f,  
         1.0f, -1.0f, -1.0f,  
        -1.0f, -1.0f,  1.0f,  
         1.0f, -1.0f,  1.0f,  
    };

    // Just a regular cube.
    unsigned int skyboxVBO, skyboxVAO;
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Load the 6 faces of the cubemap into the 6 texture targets
    std::vector<std::string> skyboxPaths = {
        "assets/right.jpg",
        "assets/left.jpg",
        "assets/top.jpg",
        "assets/bottom.jpg",
        "assets/front.jpg",
        "assets/back.jpg"
    };

    // Create a texture to hold the images
    unsigned int cubemap;
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    int width, height, channels;
    for (int i = 0; i < skyboxPaths.size(); i++) {
        unsigned char *data = stbi_load(skyboxPaths[i].c_str(), &width, &height, &channels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, NULL, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "Error reading Cubemap from path: " << skyboxPaths[i] << std::endl;
            stbi_image_free(data);
            return -1;
        }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    // Cube shader
    unsigned int vShader = Shaders::createShader(GL_VERTEX_SHADER, "shaders/skybox.vert");
    unsigned int fShader = Shaders::createShader(GL_FRAGMENT_SHADER, "shaders/skybox.frag");
    unsigned int skyboxProgram = Shaders::createAndLinkProgram({ vShader, fShader });
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    //  ----------------------------------------------


    // Shader init
    vShader = Shaders::createShader(GL_VERTEX_SHADER, "shaders/shader.vert");
    fShader = Shaders::createShader(GL_FRAGMENT_SHADER, "shaders/shader.frag");
    unsigned int program = Shaders::createAndLinkProgram({ vShader, fShader });
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    glm::mat4 projection = glm::perspective(45.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 50.0f);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.06f, 0.07f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set view matrix
        camera.update(window);

        // Cube
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.01f, 0));  // To avoid Z fighting
        model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
        glm::mat4 mvp = projection * camera.getViewMatrix() * model;
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);
        glUniform3fv(glGetUniformLocation(program, "cameraPos"), 1, glm::value_ptr(camera.position));
        glBindVertexArray(cubeVAO);
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // SKYBOX RENDERED LAST
        glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.getViewMatrix()));
        mvp = projection * skyboxView;
        glUseProgram(skyboxProgram);
        glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "mvp"), 1, GL_FALSE, &mvp[0][0]);
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

        glDepthFunc(GL_LEQUAL);  // Skybox will have depth of 1.0, which fails with GL_LESS. GL_LEQUAL will pass.
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}