//
// Created by gioel on 25/04/2020.
//

#ifndef LEARNOPENGLCOM_MESH_HPP
#define LEARNOPENGLCOM_MESH_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Shader.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    virtual ~Mesh();

    void Draw(const Shader &shader) const;

private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();

private:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<Texture> textures_;
};


#endif //LEARNOPENGLCOM_MESH_HPP
