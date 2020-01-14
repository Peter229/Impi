#pragma once

#include "glm/glm.hpp"

class Player {

public:
	Player();
	Player(glm::vec3 sPos);
	~Player();

	void update();
	void updatePos(glm::vec3 iPos);
	void addVel(glm::vec3 iVel);
	glm::vec3 getPos();
	glm::vec3 getVel();

private:
	glm::vec3 pos;
	glm::vec3 vel;

};