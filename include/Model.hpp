//
// Created by gioel on 25/04/2020.
//

#ifndef LEARNOPENGLCOM_MODEL_HPP
#define LEARNOPENGLCOM_MODEL_HPP

#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.hpp"

class Model {
public:
    explicit Model(const std::string &path);

    virtual ~Model();

    void Draw(const Shader &shader);

private:
    void loadModel(const std::string &path);

    void processNode(aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial *material, aiTextureType type, const std::string& typeName);

private:
    std::vector<Mesh> meshes_;
    std::string directory_;
    std::vector<Texture> loaded_textures_;
};


#endif //LEARNOPENGLCOM_MODEL_HPP
