#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H
#include <glad/glad.h>
#include <string>
#include <vector>

namespace Shaders {
	unsigned int createShader(GLenum type, std::string path);
	unsigned int createAndLinkProgram(std::vector<unsigned int> shaders);
}

#endif