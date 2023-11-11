#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

Camera camera(glm::vec3(0,0,0), glm::vec3(0, 1, 0), glm::vec3(0,0,-1));

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    camera.mouseCallback(window, xpos, ypos);
}

unsigned int loadShader(const char* path, GLenum shaderType) {
    std::ifstream stream(path);
    std::string source;
    if (stream) {
        std::stringstream shaderStream;
        shaderStream << stream.rdbuf(); //Read buffer into the string stream
        stream.close();
        source = shaderStream.str();
    }
    else {
        std::cout << "Error reading shader file " << path << std::endl;
        return -1;
    }

    const char* sourcestr = source.c_str();
    unsigned int shaderID = glCreateShader(shaderType);

    glShaderSource(shaderID, 1, &sourcestr, NULL);
    glCompileShader(shaderID);

    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        std::cout << "Error compiling shader: " << infoLog << std::endl;
    }

    return shaderID;
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Make sure to initialize GLAD after context has been set */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // Load our shaders into a program
    unsigned int vertShader = loadShader("shaders/triangle.vert", GL_VERTEX_SHADER);
    unsigned int fragShader = loadShader("shaders/triangle.frag", GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Error linking program: " << infoLog << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    // Initialize buffers

    const float data[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)900 / (GLfloat)800, 1.0f, 150.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5.0f));

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        camera.update(window);

        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Rendering here
        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);

        glm::mat4 mvp = projection * camera.getViewMatrix() * model;
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mvp"), 1, GL_FALSE, &mvp[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, sizeof(data)/sizeof(float));

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}