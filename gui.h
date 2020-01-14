#pragma once

#include "glm/glm.hpp"
#include "shader.h"

#include <iostream>
#include <vector>
#include <unordered_map>

class GUI {

public:
	GUI();
	~GUI();

	void addWindow(std::string window, glm::vec2 pos);
	void addOption(std::string window, std::string txt, int type);

	void cleanUp();

	void render(Shader *shader);

	void update(glm::vec2 mousePos, bool lPressed);

private:
	struct Vert2DC {

		glm::vec2 pos;
		glm::vec3 colour;
	};

	struct Option {
		
		std::string name;
		int type;
	};

	struct Window {

		glm::vec2 pos;
		int width;
		std::vector<Option*> options;
		bool active;
	};

	std::unordered_map<std::string, Window*> windows;

	std::vector<Vert2DC> verts;

	unsigned int VAO, VBO;

	float optionHeight;
	float optionPadding;

	float scale;

	void genVerts();

	void genVAO();
};