//
// Created by gioel on 25/04/2020.
//

#include <iostream>
#include "Model.hpp"
#include "Utils.hpp"

Model::Model(const std::string &path) {
    loadModel(path);
}

Model::~Model() {

}

void Model::Draw(const Shader &shader) {
    for (const auto &mesh : meshes_)
        mesh.Draw(shader);
}

void Model::loadModel(const std::string &path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    }
    directory_ = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // process all node's meshes
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes_.push_back(processMesh(mesh, scene));
    }

    // recurse on children node
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex{};

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        // check if mesh has texture coordinates
        if (mesh->mTextureCoords[0]) {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseTextures = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

        std::vector<Texture> specularTextures = loadMaterialTextures(material, aiTextureType_SPECULAR,
                                                                     "texture_specular");
        textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture>
Model::loadMaterialTextures(aiMaterial *material, aiTextureType type, const std::string &typeName) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < material->GetTextureCount(type); ++i) {
        aiString texturePath;
        material->GetTexture(type, i, &texturePath);

        bool skip = false;

        for (const auto &loadedTexture : loaded_textures_) {
            if (std::strcmp(loadedTexture.path.data(), texturePath.C_Str()) == 0) {
                // texture already loaded
                textures.push_back(loadedTexture);
                skip = true;
                break;
            }
        }

        if (!skip) {
            // texture not loaded already
            Texture texture;
            texture.id = TextureFromFile(texturePath.C_Str(), directory_);
            texture.type = typeName;
            texture.path = texturePath.C_Str();
            textures.push_back(texture);
            loaded_textures_.push_back(texture);
        }
    }

    return textures;
}






















