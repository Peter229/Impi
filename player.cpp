#include "player.h"

Player::Player() {

	pos = glm::vec3(0.0f);
	vel = glm::vec3(0.0f);
}

Player::Player(glm::vec3 sPos) {

	pos = sPos;
	vel = glm::vec3(0.0f);
}

Player::~Player() {

}

void Player::update() {

	pos += vel;
	vel = glm::vec3(0.0f);
}

void Player::updatePos(glm::vec3 iPos) {

	pos = iPos;
	vel /= glm::vec3(1.01f, 1.01f, 1.01f);
	if (abs(vel.x) < 0.1f) {
		vel.x = 0.0f;
	}
	if (abs(vel.y) < 0.1f) {
		vel.y = 0.0f;
	}
	if (abs(vel.z) < 0.1f) {
		vel.z = 0.0f;
	}
	//vel = glm::vec3(0.0f);
}

void Player::addVel(glm::vec3 iVel) {

	vel += iVel;
}

glm::vec3 Player::getPos() {

	return pos;
}

glm::vec3 Player::getVel() {

	return vel;
}