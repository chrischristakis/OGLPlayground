#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>
#include <string>

namespace Texture {
    unsigned int load(std::string path, GLenum sourceType) {
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, sourceType, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        stbi_set_flip_vertically_on_load(false);

        glBindTexture(GL_TEXTURE_2D, 0);

        return texture;
    }
}

#endif