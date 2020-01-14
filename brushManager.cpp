#include "brushManager.h"

BrushManager::BrushManager() {

	selectedBrush = NULL;
}

BrushManager::~BrushManager() {

	for (auto i : brushes) {

		i->cleanUp();

		delete i;
	}

	brushes.clear();
}

void BrushManager::addBrush(glm::vec3 min, glm::vec3 max) {

	brushes.insert(new Brush(min, max));
}

void BrushManager::selectBrush(glm::vec3 orig, glm::vec3 dir) {

	float closest = 1000.0f;
	
	bool found = false;

	for (auto i : brushes) {

		if (Collision::rayAABB(i->getMin(), i->getMax(), orig, dir)) {

			float t = Collision::rayAABBf(i->getMin(), i->getMax(), orig, dir);

			if ((t < closest) && t >= 0.0f) {

				if (selectedBrush != i) {
					closest = t;
					selectedBrush = i;
					found = true;
				}
			}
		}
	}

	if (!found) {
		selectedBrush = NULL;
	}
}

void BrushManager::deleteBrush() {

	for (auto i : brushes) {

		if (i == selectedBrush) {

			i->cleanUp();
			delete i;
			brushes.erase(i);
			selectedBrush = NULL;
			break;
		}
	}
}

void BrushManager::changeBrushTexture(int tex) {

	for (auto i : brushes) {

		if (i == selectedBrush) {

			i->setTexture(tex);
			break;
		}
	}
}

void BrushManager::render(Shader* shader) {

	for (auto i : brushes) {
		
		if (i->getShouldRender()) {

			if (i == selectedBrush) {
				//std::cout << i << "\n";
				i->renderSelect(shader);
			}
	
			i->render(shader);
		}
	}
}

void BrushManager::cleanUp() {

	for (auto i : brushes) {

		i->cleanUp();

		delete i;
	}

	brushes.clear();
}

std::vector<glm::vec3> BrushManager::getColHull() {

	std::vector<glm::vec3> colTris;

	for (auto i : brushes) {

		std::vector<glm::vec3> tempColTris = i->getColTris();

		colTris.insert(colTris.end(), tempColTris.begin(), tempColTris.end());
	}

	return colTris;
}