#ifndef MESH_H
#define MESH_H
#include <glad/glad.h>
#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Texture {
	unsigned int id;
	enum Type {
		DIFFUSE,
		SPECULAR,
		NORMAL,
		HEIGHT
	} type;
};

/*
* A mesh contains all the relevant vertex data for that particular mesh, and how to draw it.
*/
class Mesh {
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<unsigned int> indices;

	unsigned int VAO, VBO, EBO;

	void init() {
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
public:
	Mesh(
		std::vector<Vertex> vertices, 
		std::vector<Texture> textures, 
		std::vector<unsigned int> indices
	): vertices(vertices), textures(textures), indices(indices) {
		init();
	}

	void draw(unsigned int shaderProgram) {
		glBindVertexArray(VAO);
		
		// Texture binding, this assumes samplers are titled '<type>_texture<num>' in the shader.
		int diffuse = 1, specular = 1, normal = 1, height = 1;
		for (int i = 0; i < textures.size(); i++) {
			
			std::string type = "";
			int num = 0;

			if (textures[i].type == Texture::DIFFUSE) {
				type = "diffuse";
				num = diffuse++;
			}
			else if (textures[i].type == Texture::SPECULAR) {
				type = "specular";
				num = specular++;
			}
			else if (textures[i].type == Texture::NORMAL) {
				type = "normal";
				num = normal++;
			}
			else if (textures[i].type == Texture::HEIGHT) {
				type = "height";
				num = height++;
			}

			// Set the sampler and bind to the texture unit.
			glUniform1i(glGetUniformLocation(shaderProgram, (type + "_texture" + std::to_string(num)).c_str()), i);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
};

#endif