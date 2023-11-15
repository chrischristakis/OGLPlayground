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
bool wireframe = false;

glm::mat4 projection = glm::perspective(45.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 50.0f);

// VAOs
unsigned int cubeVAO, planeVAO;

//Textures
unsigned int marbleTexture, metalTexture;

// Shaders
unsigned int program, quadProgram;

// ------------------- CALLBACKS -------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        glLineWidth(2.0f);
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        wireframe = !wireframe;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.mouseCallback(window, xpos, ypos);
}
// --------------------------------------------------


void drawScene() {
    glUseProgram(program);

    // Cube
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.01f, 0));  // To avoid Z fighting
    glm::mat4 mvp = projection * camera.getViewMatrix() * model;
    glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);
    glBindVertexArray(cubeVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, marbleTexture);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(Constants::cubeVerts) / sizeof(float));

    // Plane
    mvp = projection * camera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &mvp[0][0]);
    glBindVertexArray(planeVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, metalTexture);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(Constants::planeVerts) / sizeof(float));
}


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

    unsigned int cubeVBO;
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Constants::cubeVerts), Constants::cubeVerts, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int planeVBO;
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

    //Texture init
    marbleTexture = Texture::load("assets/marble.jpg", GL_RGB);
    metalTexture = Texture::load("assets/metal.png", GL_RGB);
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "tex"), 0);


    // ------- FRAMEBUFFER STUFF ---------

    // Init framebuffer
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO); // Subsequent calls will be performed on this framebuffer, not the default one!

    // Gen colour and depth attachments. Color = Texture for sampling/reading, depth = renderbuffer since no reading
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0); // Attach the new texture we made as a color attachment

    // Now, lets attach a depth buffer.
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT); // We don't need stencil,s tore only depth
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check if FBO is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error, framebuffer is not complete." << std::endl;
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind FBO so we don't accidentally render to it, back to default now.

    // The quad to display our scene texture
    float quadVerts[] = {
        //pos         //uv
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f
    };

    // Set up VAO and VBO for our quad
    unsigned int quadVBO, quadVAO;
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Create shaders to render FBO quad, just rendering a quad nothing fancy
    unsigned int vQuadShader = Shaders::createShader(GL_VERTEX_SHADER, "shaders/quad.vert");
    unsigned int fQuadShader = Shaders::createShader(GL_FRAGMENT_SHADER, "shaders/quad.frag");
    quadProgram = Shaders::createAndLinkProgram({ vQuadShader, fQuadShader });
    glDeleteShader(vQuadShader);
    glDeleteShader(fQuadShader);

    glUseProgram(quadProgram);
    glUniform1i(glGetUniformLocation(quadProgram, "tex"), 0);
    glUseProgram(0);

    // ------------------------------------------------------

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Set view matrix
        camera.update(window);
        
        // Draw normal scene to texture (First render pass)

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.06f, 0.07f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        drawScene();

        //Draw the texture with our scene on it to a quad (Second RENDER pass)

        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Render to default frame buffer (to the screen)
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(quadProgram);
        glBindVertexArray(quadVAO);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(quadVerts) / sizeof(float));

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}