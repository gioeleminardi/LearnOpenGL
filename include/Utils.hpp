//
// Created by gioel on 25/04/2020.
//

#ifndef LEARNOPENGLCOM_UTILS_HPP
#define LEARNOPENGLCOM_UTILS_HPP

#include <string>
#include <glad/glad.h>
#include <iostream>
#include "stb_image.hpp"

unsigned int TextureFromFile(const std::string &path, const std::string &directory) {
    std::string filename = path;
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *textureData = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (textureData) {
        GLenum format;
        switch (nrComponents) {
            case 1: {
                format = GL_RED;
                break;
            }
            case 3: {
                format = GL_RGB;
                break;
            }
            case 4: {
                format = GL_RGBA;
                break;
            }
            default: {
                format = GL_RGB;
            }
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        {
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        stbi_image_free(textureData);
    } else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(textureData);
    }

    return textureID;
}

#endif //LEARNOPENGLCOM_UTILS_HPP
