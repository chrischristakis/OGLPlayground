#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace TextureUtil {
    unsigned int load(std::string path, bool flipUv = false) {
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int width, height, channels;
        stbi_set_flip_vertically_on_load(flipUv);
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        stbi_set_flip_vertically_on_load(false);

        if (data) {
            GLenum format;
            if (channels == 1)
                format = GL_RED;
            else if (channels == 3)
                format = GL_RGB;
            else if (channels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        else {
            std::cout << "Couldn't load texture '" << path << "'" << std::endl;
        }
        
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
        return texture;
    }
}

#endif