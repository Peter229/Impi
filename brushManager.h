#pragma once

#include "brush.h"
#include <unordered_set>

class BrushManager {

public:
	BrushManager();
	~BrushManager();

	void addBrush(glm::vec3 min, glm::vec3 max);

	void selectBrush(glm::vec3 orig, glm::vec3 dir);

	void deleteBrush();

	void changeBrushTexture(int tex);

	void render(Shader* shader);

	void cleanUp();

	std::vector<glm::vec3> getColHull();

private:
	std::unordered_set<Brush*> brushes;
	
	Brush* selectedBrush;
};