#include "ShaderProgram.h"
#include <sstream>
#include <fstream>
#include <iostream>

namespace Shaders {
    unsigned int createShader(GLenum type, std::string path) {

        // Read in our shader code
        std::ifstream stream(path);
        std::string source;
        if (stream) {
            std::stringstream shaderStream;
            shaderStream << stream.rdbuf(); // Read buffer into the string stream
            stream.close();
            source = shaderStream.str();
        }
        else {
            std::cerr << "Error reading shader file " << path << std::endl;
            return -1;
        }

        const char* sourcestr = source.c_str();
        unsigned int shaderID = glCreateShader(type);

        glShaderSource(shaderID, 1, &sourcestr, NULL);
        glCompileShader(shaderID);

        int success;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
            std::cerr << "Error compiling shader at " << path << ": " << infoLog << std::endl;
            return -1;
        }

        return shaderID;
    }

    unsigned int createAndLinkProgram(std::vector<unsigned int> shaders) {
        // First, attach all the shaders
        unsigned int program = glCreateProgram();
        for (unsigned int id : shaders)
            glAttachShader(program, id);

        glLinkProgram(program);

        int success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            std::cerr << "Error linking program: " << infoLog << std::endl;
            return -1;
        }

        return program;
    }
}