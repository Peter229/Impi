#include "brush.h"

Brush::Brush() {
	
	min = max = glm::vec3(0.0f, 0.0f, 0.0f);
	shouldRender = false;
	tex = 0;
}

Brush::Brush(glm::vec3 min, glm::vec3 max) {

	this->min = min;
	this->max = max;

	//Top Face
	//First Triangle
	Vert3DUVN verta = { {min.x, max.y, min.z }, {max.x, min.z}, {0.0f, 1.0f, 0.0f} };
	Vert3DUVN vertb = { {min.x, max.y, max.z }, {max.x, max.z}, {0.0f, 1.0f, 0.0f} };
	Vert3DUVN vertc = { {max.x, max.y, max.z }, {min.x, max.z}, {0.0f, 1.0f, 0.0f} };
	verts.push_back(verta);
	verts.push_back(vertb);
	verts.push_back(vertc);
	//Second Triangle
	verta = { {min.x, max.y, min.z }, {max.x, min.z}, {0.0f, 1.0f, 0.0f} };
	vertb = { {max.x, max.y, max.z }, {min.x, max.z}, {0.0f, 1.0f, 0.0f} };
	vertc = { {max.x, max.y, min.z }, {min.x, min.z}, {0.0f, 1.0f, 0.0f} };
	verts.push_back(verta);
	verts.push_back(vertb);
	verts.push_back(vertc);

	//Bottom Face
	verta = { {min.x, min.y, min.z }, {min.x, min.z}, {0.0f, -1.0f, 0.0f} };
	vertb = { {max.x, min.y, max.z }, {max.x, max.z}, {0.0f, -1.0f, 0.0f} };
	vertc = { {min.x, min.y, max.z }, {min.x, max.z}, {0.0f, -1.0f, 0.0f} };
	verts.push_back(verta);
	verts.push_back(vertb);
	verts.push_back(vertc);

	verta = { {min.x, min.y, min.z }, {min.x, min.z}, {0.0f, -1.0f, 0.0f} };
	vertb = { {max.x, min.y, min.z }, {max.x, min.z}, {0.0f, -1.0f, 0.0f} };
	vertc = { {max.x, min.y, max.z }, {max.x, max.z}, {0.0f, -1.0f, 0.0f} };
	verts.push_back(verta);
	verts.push_back(vertb);
	verts.push_back(vertc);

	//Left Face
	verta = { {min.x, min.y, min.z }, {min.z, min.y}, {-1.0f, 0.0f, 0.0f} };
	vertb = { {min.x, min.y, max.z }, {max.z, min.y}, {-1.0f, 0.0f, 0.0f} };
	vertc = { {min.x, max.y, max.z }, {max.z, max.y}, {-1.0f, 0.0f, 0.0f} };
	verts.push_back(verta);
	verts.push_back(vertb);
	verts.push_back(vertc);

	verta = { {min.x, min.y, min.z }, {min.z, min.y}, {-1.0f, 0.0f, 0.0f} };
	vertb = { {min.x, max.y, max.z }, {max.z, max.y}, {-1.0f, 0.0f, 0.0f} };
	vertc = { {min.x, max.y, min.z }, {min.z, max.y}, {-1.0f, 0.0f, 0.0f} };
	verts.push_back(verta);
	verts.push_back(vertb);
	verts.push_back(vertc);

	//Right Face
	verta = { {max.x, min.y, min.z }, {max.z, min.y}, {1.0f, 0.0f, 0.0f} };
	vertb = { {max.x, max.y, max.z }, {min.z, max.y}, {1.0f, 0.0f, 0.0f} };
	vertc = { {max.x, min.y, max.z }, {min.z, min.y}, {1.0f, 0.0f, 0.0f} };
	verts.push_back(verta);
	verts.push_back(vertb);
	verts.push_back(vertc);

	verta = { {max.x, min.y, min.z }, {max.z, min.y}, {1.0f, 0.0f, 0.0f} };
	vertb = { {max.x, max.y, min.z }, {max.z, max.y}, {1.0f, 0.0f, 0.0f} };
	vertc = { {max.x, max.y, max.z }, {min.z, max.y}, {1.0f, 0.0f, 0.0f} };
	verts.push_back(verta);
	verts.push_back(vertb);
	verts.push_back(vertc);

	//Front Face
	verta = { {min.x, min.y, max.z }, {min.x, min.y}, {0.0f, 0.0f, 1.0f} };
	vertb = { {max.x, max.y, max.z }, {max.x, max.y}, {0.0f, 0.0f, 1.0f} };
	vertc = { {min.x, max.y, max.z }, {min.x, max.y}, {0.0f, 0.0f, 1.0f} };
	verts.push_back(verta);
	verts.push_back(vertb);
	verts.push_back(vertc);

	verta = { {min.x, min.y, max.z }, {min.x, min.y}, {0.0f, 0.0f, 1.0f} };
	vertb = { {max.x, min.y, max.z }, {max.x, min.y}, {0.0f, 0.0f, 1.0f} };
	vertc = { {max.x, max.y, max.z }, {max.x, max.y}, {0.0f, 0.0f, 1.0f} };
	verts.push_back(verta);
	verts.push_back(vertb);
	verts.push_back(vertc);

	//Back Face
	verta = { {min.x, min.y, min.z }, {max.x, min.y}, {0.0f, 0.0f, -1.0f} };
	vertb = { {min.x, max.y, min.z }, {max.x, max.y}, {0.0f, 0.0f, -1.0f} };
	vertc = { {max.x, max.y, min.z }, {min.x, max.y}, {0.0f, 0.0f, -1.0f} };
	verts.push_back(verta);
	verts.push_back(vertb);
	verts.push_back(vertc);

	verta = { {min.x, min.y, min.z }, {max.x, min.y}, {0.0f, 0.0f, -1.0f} };
	vertb = { {max.x, max.y, min.z }, {min.x, max.y}, {0.0f, 0.0f, -1.0f} };
	vertc = { {max.x, min.y, min.z }, {min.x, min.y}, {0.0f, 0.0f, -1.0f} };
	verts.push_back(verta);
	verts.push_back(vertb);
	verts.push_back(vertc);

	tex = 0;

	genColMesh();

	genVAO();
}

Brush::~Brush() {

	if (shouldRender) {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	if (verts.size() != 0) {
		verts.clear();
	}
}

void Brush::genColMesh() {

	for (int i = 0; i < verts.size(); i++) {

		int amountOfDupes = 1;
		glm::vec3 tempNormal = verts[i].normal;

		for (int j = 0; j < verts.size(); j++) {

			if (i != j) {

				if (verts[i].pos == verts[j].pos) {

					tempNormal += verts[j].normal;
					amountOfDupes += 1;
				}
			}
		}

		tempNormal /= (float)amountOfDupes;
		tempNormal = glm::normalize(tempNormal);

		glm::vec3 tempColVec = verts[i].pos + (tempNormal * 0.70710678118f);

		colTris.push_back(tempColVec);
	}
}

std::vector<glm::vec3> Brush::getColTris() {

	return colTris;
}

void Brush::render(Shader* shader) {
	glm::mat4 model = glm::mat4(1.0f);

	glm::vec3 center = glm::vec3(0.0f);
	glm::vec3 max = glm::vec3(-1000.0f);
	glm::vec3 min = glm::vec3(1000.0f);
	for (int i = 0; i < verts.size(); i++) {

		center += verts[i].pos;
		max.x = std::max(verts[i].pos.x, max.x);
		max.y = std::max(verts[i].pos.y, max.y);
		max.z = std::max(verts[i].pos.z, max.z);

		min.x = std::min(verts[i].pos.x, min.x);
		min.y = std::min(verts[i].pos.y, min.y);
		min.z = std::min(verts[i].pos.z, min.z);
	}

	center /= verts.size();
	model = glm::translate(model, center);
	//model = glm::scale(model, glm::vec3(1.15f, 1.5f, 1.15f));
	model = glm::translate(model, -center);

	shader->setMat4("model", model);
	shader->setInt("texIndex", tex);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, verts.size());
}

void Brush::renderSelect(Shader* shader) {

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	shader->setInt("texIndex", 3);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, verts.size());
}

void Brush::cleanUp() {

	if (shouldRender) {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	if (verts.size() != 0) {
		verts.clear();
	}
}

bool Brush::getShouldRender() {

	return shouldRender;
}

glm::vec3 Brush::getMin() {

	return min;
}

glm::vec3 Brush::getMax() {

	return max;
}

void Brush::setTexture(int tex) {

	this->tex = tex;
}

void Brush::genVAO() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vert3DUVN) * verts.size(), &verts[0], GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//UV attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	shouldRender = true;
}