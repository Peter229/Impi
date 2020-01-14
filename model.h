#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "glm/glm.hpp"
#include "includesGL.h"
#include "shader.h"

class Model {

public:
	Model();
	Model(std::string path);
	~Model();

	void render(Shader *shader);
	void cleanUp();
	std::vector<float> getPosBuffer();

private:

	GLuint GLVAO;
	GLuint GLPosBuffer;
	GLuint GLIndBuffer;
	GLuint GLNorBuffer;
	GLuint GLUvBuffer;

	std::vector<float> PosBuffer;
	std::vector<float> NorBuffer;
	std::vector<float> UvBuffer;
	std::vector<GLuint> IndBuffer;

	void genVAO();
};