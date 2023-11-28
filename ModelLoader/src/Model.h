#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <iostream>
#include <glm/common.hpp>
#include <unordered_map>
#include "Mesh.h"
#include "Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/*
* A model contains many meshes.
*/
class Model {
	std::vector<Mesh> meshes;
	std::string directory;  // Directory of the model.
	std::unordered_map<std::string, Texture> loadedTextures; // So we dont reload the same texture

	void processNode(aiNode *node, const aiScene *scene) {
		for (int i = 0; i < node->mNumMeshes; i++) {
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

			meshes.push_back(processMesh(mesh, scene));
		}
		
		// Recursvely iterate children and process their meshes too.
		for (int i = 0; i < node->mNumChildren; i++)
			processNode(node->mChildren[i], scene);
	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
		std::vector<Vertex> vertices;
		// process all vertex properties
		for (int i = 0; i < mesh->mNumVertices; i++) {
			Vertex v;

			v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

			if (mesh->HasNormals())
				v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

			if (mesh->mTextureCoords[0] != nullptr)
				v.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);

			vertices.push_back(v);
		}

		// Process indices
		std::vector<unsigned int> indices;
		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];

			for (int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// Now process materials
		
		std::vector<Texture> textures;
		if (scene->HasMaterials()) {
			// Each mesh links to one material
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
			
			processTexture(aiTextureType_DIFFUSE, material, textures);
			processTexture(aiTextureType_SPECULAR, material, textures);
			processTexture(aiTextureType_HEIGHT, material, textures);
			processTexture(aiTextureType_AMBIENT, material, textures);
		}

		return Mesh(vertices, textures, indices);
	}

	void processTexture(aiTextureType type, aiMaterial *material, std::vector<Texture> &textures) {
		for (int i = 0; i < material->GetTextureCount(type); i++) {

			aiString path;
			material->GetTexture(type, i, &path);
			std::string fullPath = directory + "/" + path.C_Str();

			// Check if texture already exists
			if (loadedTextures.find(fullPath) != loadedTextures.end()) {
				Texture found = loadedTextures.find(fullPath)->second;
				textures.push_back(found);
				continue;
			}

			unsigned int texID = TextureUtil::load(fullPath);

			Texture texture;
			texture.id = texID;
			if (type == aiTextureType_DIFFUSE)
				texture.type = Texture::DIFFUSE;
			else if (type == aiTextureType_SPECULAR)
				texture.type = Texture::SPECULAR;
			else if (type == aiTextureType_HEIGHT)
				texture.type = Texture::NORMAL;
			else if (type == aiTextureType_AMBIENT)
				texture.type = Texture::HEIGHT;

			textures.push_back(texture);
			loadedTextures.insert({ fullPath, texture });
		}
	}
public:
	Model(std::string const &path) {
		directory = path.substr(0, path.find_last_of('/'));

		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "Error loading Assimp scene: " << importer.GetErrorString() << std::endl;
			return;
		}

		processNode(scene->mRootNode, scene);
	}

	void draw(unsigned int shaderProgram) {
		for (int i = 0; i < meshes.size(); i++)
			meshes[i].draw(shaderProgram);
	}
};

#endif