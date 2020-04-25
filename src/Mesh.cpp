//
// Created by gioel on 25/04/2020.
//

#include "Mesh.hpp"

#include <utility>

#include <glad/glad.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) : vertices_(
        std::move(
                vertices)), indices_(std::move(indices)), textures_(std::move(textures)), VAO(0), VBO(0), EBO(0) {
    setupMesh();
}

Mesh::~Mesh() {

}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        {
            glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(),
                         GL_STATIC_DRAW);
        }

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(2);
    }
    glBindVertexArray(0);
}

void Mesh::Draw(const Shader &shader) {
    unsigned int diffuseNum = 1;
    unsigned int specularNum = 1;
    for (int i = 0; i < textures_.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        {
            std::string number;
            std::string name = textures_[i].type;

            if (name == "texture_diffuse")
                number = std::to_string(diffuseNum++);
            else if (name == "texture_specular")
                number = std::to_string(specularNum++);

            shader.setInt(std::string("material.").append(name).append(number), i);
            glBindTexture(GL_TEXTURE_2D, textures_[i].id);
        }
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    {
        glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
    }
    glBindVertexArray(0);
}

