#pragma once

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

struct colInfo {
	std::vector<glm::vec3> cSimplex;
	glm::vec3 cD;
	bool containsOrigin;
};

class Collision {

public:
	static bool gjkCollision(std::vector<glm::vec3> obj1, std::vector<glm::vec3> obj2, glm::vec3 initialAxis);

	static bool satCollisionOOBTri(glm::vec3 cent, glm::vec3 exten, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);

	static bool satCollisionOOBTriStatic(glm::vec3 *cent, glm::vec3 exten, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);

	static void movePlayerAgainstWorld(glm::vec3 *cent, glm::vec3 exten, glm::vec3 *vel, std::vector<glm::vec3> colTriangles);

	static void toiAABBTri(glm::vec3 *cent, glm::vec3 exten, glm::vec3 vel, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);

	static bool pointExpandTri(glm::vec3 cent, glm::vec3 exten, glm::vec3 vel, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float *t);

	static bool rayAABB(glm::vec3 min, glm::vec3 max, glm::vec3 orig, glm::vec3 dir);

	static float rayAABBf(glm::vec3 min, glm::vec3 max, glm::vec3 orig, glm::vec3 dir);

	static float rayPlaneIntersection(glm::vec3 dir, glm::vec3 orig, glm::vec3 center, glm::vec3 normal);

	static bool rayTrif(glm::vec3 orig, glm::vec3 dir, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float *t);

private:

	static bool segmentTrif(glm::vec3 start, glm::vec3 end, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float *t);

	static bool satAxisProjection(glm::vec3 axis, glm::vec3 exten, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 u0, glm::vec3 u1, glm::vec3 u2);

	static bool satAxisProjectionStatic(glm::vec3 axis, glm::vec3 exten, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 u0, glm::vec3 u1, glm::vec3 u2, float *t);

	static float maxThree(float x, float y, float z);
	
	static float minThree(float x, float y, float z);

	static colInfo doSimplex(std::vector<glm::vec3> simplex);

	static glm::vec3 support(glm::vec3 dir, std::vector<glm::vec3> obj1);
};