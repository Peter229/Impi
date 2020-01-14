#pragma once

#include "shader.h"
#include "glm/glm.hpp"
#include "collision.h"
#include "resource_manager.h"
#include <vector>

class Brush {

public:
	Brush();

	Brush(glm::vec3 min, glm::vec3 max);

	~Brush();

	void render(Shader* shader);
	void renderSelect(Shader *shader);

	void cleanUp();

	bool getShouldRender();

	void setTexture(int tex);

	std::vector<glm::vec3> getColTris();

	glm::vec3 getMin();
	glm::vec3 getMax();

private:
	struct Vert3DUVN {
		glm::vec3 pos;
		glm::vec2 uv;
		glm::vec3 normal;
	};

	int tex;
	unsigned int VAO, VBO;
	std::vector<Vert3DUVN> verts;
	std::vector<glm::vec3> colTris;

	glm::vec3 min, max;

	bool shouldRender;

	void genVAO();

	void genColMesh();
};