#include "collision.h"

void printVector(glm::vec3 vec) {

	std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")\n";

}

float vectorAngle(glm::vec3 v0, glm::vec3 v1) {

	return acosf(glm::dot(v0, v1) / (glm::length(v0) * glm::length(v1))) * (180.0f / 3.14159265359f);
}

bool Collision::gjkCollision(std::vector<glm::vec3> obj1, std::vector<glm::vec3> obj2, glm::vec3 initialAxis) {

	glm::vec3 A = support(initialAxis, obj1) - support(-initialAxis, obj2);
	std::vector<glm::vec3> simplex;
	simplex.push_back(A);
	glm::vec3 D = -A;
	bool colFound = false;
	bool loop = true;

	colInfo info;

	while (loop) {

		A = support(D, obj1) - support(-D, obj2);
		if (glm::dot(A, D) < 0) {
			colFound = false;
			loop = false;
		}
		else {
			simplex.push_back(A);
			info = doSimplex(simplex);
			if (info.containsOrigin) {
				colFound = true;
				loop = false;
			}
			else {
				simplex = info.cSimplex;
				D = info.cD;
			}
		}
	}

	return colFound;
}

colInfo Collision::doSimplex(std::vector<glm::vec3> simplex) {

	colInfo out;

	out.containsOrigin = false;

	if (simplex.size() == 2) { //Line MIGHT NEED TO REWIND OUTPUT SIMPLEX

		if (glm::dot(simplex[0] - simplex[1], glm::vec3(0.0f, 0.0f, 0.0f) - simplex[1]) > 0.0f) {

			out.cSimplex.push_back(simplex[1]);
			out.cSimplex.push_back(simplex[0]);
			out.cD = glm::cross(glm::cross(simplex[0] - simplex[1], glm::vec3(0.0f, 0.0f, 0.0f) - simplex[1]), simplex[0] - simplex[1]);
		}
		else {
			out.cSimplex.push_back(simplex[1]);
			out.cD = glm::vec3(0.0f, 0.0f, 0.0f) - simplex[1];
		}
	}
	else if (simplex.size() == 3) { //Triangle

		glm::vec3 A0 = glm::vec3(0.0f, 0.0f, 0.0f) - simplex[2];
		glm::vec3 AC = simplex[0] - simplex[2];
		glm::vec3 AB = simplex[1] - simplex[2];

		glm::vec3 ABC = glm::cross(AB, AC);

		if (glm::dot(glm::cross(ABC, AC), A0) > 0.0f) {

			if (glm::dot(AC, A0) > 0.0f) {

				out.cSimplex.push_back(simplex[2]);
				out.cSimplex.push_back(simplex[0]);
				out.cD = glm::cross(glm::cross(AC, A0), AC);
			}
			else {

				if (glm::dot(AB, A0) > 0.0f) {

					out.cSimplex.push_back(simplex[2]);
					out.cSimplex.push_back(simplex[1]);
					out.cD = glm::cross(glm::cross(AB, A0), AB);
				}
				else {
					out.cSimplex.push_back(simplex[2]);
					out.cD = A0;
				}
			}
		}
		else {
			
			if (glm::dot(glm::cross(AB, ABC), A0) > 0.0f) {

				if (glm::dot(AB, A0) > 0.0f) {

					out.cSimplex.push_back(simplex[2]);
					out.cSimplex.push_back(simplex[1]);
					out.cD = glm::cross(glm::cross(AB, A0), AB);
				}
				else {
					out.cSimplex.push_back(simplex[2]);
					out.cD = A0;
				}
			}
			else {

				if (glm::dot(ABC, A0) > 0.0f) {

					out.cSimplex.push_back(simplex[2]);
					out.cSimplex.push_back(simplex[1]);
					out.cSimplex.push_back(simplex[0]);
					out.cD = ABC;
				}
				else {

					out.cSimplex.push_back(simplex[2]);
					out.cSimplex.push_back(simplex[0]);
					out.cSimplex.push_back(simplex[1]);
					out.cD = -ABC;
				}
			}
		}
	}
	else if (simplex.size() == 4) { //Tetrahedron
	
		glm::vec3 A0 = glm::vec3(0.0f, 0.0f, 0.0f) - simplex[3];
		glm::vec3 AD = simplex[0] - simplex[3];
		glm::vec3 AC = simplex[1] - simplex[3];
		glm::vec3 AB = simplex[2] - simplex[3];

		glm::vec3 ABC = glm::cross(AB, AC);
		glm::vec3 ACD = glm::cross(AC, AD);
		glm::vec3 ADB = glm::cross(AD, AB);
		
		glm::vec3 ABCD = glm::cross(ABC, ACD);

		if (glm::dot(ABC, A0) < 0.0f) {

			out.cSimplex.push_back(simplex[3]);
			out.cSimplex.push_back(simplex[1]);
			out.cSimplex.push_back(simplex[2]);
			out.cD = -ABC;
		}
		else if (glm::dot(ACD, A0) < 0.0f) {

			out.cSimplex.push_back(simplex[3]);
			out.cSimplex.push_back(simplex[0]);
			out.cSimplex.push_back(simplex[1]);
			out.cD = -ACD;
		}
		else if (glm::dot(ADB, A0) < 0.0f) {

			out.cSimplex.push_back(simplex[3]);
			out.cSimplex.push_back(simplex[0]);
			out.cSimplex.push_back(simplex[2]);
			out.cD = -ADB;
		}
		else {
			out.cSimplex.push_back(simplex[0]);
			out.cSimplex.push_back(simplex[0]);
			out.cSimplex.push_back(simplex[0]);
			out.cD = -ADB;
			out.containsOrigin = true;
		}
	}

	return out;
}

glm::vec3 Collision::support(glm::vec3 d, std::vector<glm::vec3> obj) {

	float max = -999.0f;

	glm::vec3 maxPoint = glm::vec3(0.0f);

	for (int i = 0; i < obj.size(); i++) {

		float t = glm::dot(obj[i], d);

		if (t > max) {
			maxPoint = obj[i];
			max = t;
		}
	}

	return maxPoint;
}

void Collision::movePlayerAgainstWorld(glm::vec3 *cent, glm::vec3 exten, glm::vec3 *vel, std::vector<glm::vec3> colTriangles) {

	float t = 0.0f;
	float smallestMove = 1.0f;

	bool colHappened = false;

	float veryCloseDistance = 0.01f;

	for (int i = 0; i < colTriangles.size(); i += 3) {

		if (pointExpandTri(*cent, exten, *vel, colTriangles[i], colTriangles[i + 1], colTriangles[i + 2], &t)) {

			smallestMove = glm::min(t, smallestMove);
			colHappened = true;
		}
	}

	if (colHappened) {

		//std::cout << smallestMove << "\n";
		glm::vec3 v = glm::normalize(*vel);
		glm::vec3 intersectionPoint = *cent + (smallestMove * v);

		glm::vec3 newPosition = *cent;
		glm::vec3 destPoint = *cent + *vel;

		if (smallestMove >= -veryCloseDistance) {

			v = v * (smallestMove - veryCloseDistance);
			newPosition += v;
			*cent += (destPoint - newPosition) * smallestMove;
			*vel = glm::vec3(0.0f, 0.0f, 0.0f);

			v = glm::normalize(v);
			intersectionPoint -= veryCloseDistance * v;
		}

		/*glm::vec3 slidePlaneOrigin = intersectionPoint;
		glm::vec3 slidePlaneNormal = newPosition - intersectionPoint;
		slidePlaneNormal = glm::normalize(slidePlaneNormal);

		float x = slidePlaneOrigin.x;
		float y = slidePlaneOrigin.y;
		float z = slidePlaneOrigin.z;

		float A = slidePlaneNormal.x;
		float B = slidePlaneNormal.y;
		float C = slidePlaneNormal.z;
		float D = -((A*x) + (B*y) + (C*z));

		float planeConstant = D;

		float signedDistFromDestPointToSlidingPlane = glm::dot(destPoint, slidePlaneNormal) + planeConstant;

		glm::vec3 newDestPoint = destPoint - signedDistFromDestPointToSlidingPlane * slidePlaneNormal;

		glm::vec3 newVelocity = newDestPoint - intersectionPoint;


		*cent = newPosition;
		*vel = newVelocity;
		//*cent += ((*cent + *vel) - *cent) * t;
		*vel = glm::vec3(0.0f);
		*/
	}
	else {

		*cent += *vel;
	}
}

bool Collision::pointExpandTri(glm::vec3 cent, glm::vec3 exten, glm::vec3 vel, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float *t) {

	bool foundColl = false;

	glm::vec3 sP = cent;
	glm::vec3 eP = cent + vel;

	glm::vec3 triNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

	glm::vec3 extendS = triNormal * exten;
	
	glm::vec3 triCenter = (v0 + v1 + v2) / 3.0f;

	//std::cout << triCenter.x << " " << triCenter.y << " " << triCenter.z << "\n";

	//glm::vec3 v0tC = glm::normalize(v0 - triCenter);
	//glm::vec3 v1tC = glm::normalize(v1 - triCenter);
	//glm::vec3 v2tC = glm::normalize(v2 - triCenter);

	//std::cout << "A ";
	//printVector(v0 - v1);
	//std::cout << "B ";
	//printVector(v2 - v0);

	//float a0 = vectorAngle(v1 - v0, v2 - v0);
	//float a1 = vectorAngle(v0 - v1, v2 - v1);
	//float a2 = vectorAngle(v0 - v2, v1 - v2);
	//v0tC = v0tC * (0.70710678118f * (90.0f / a0));
	//v1tC = v1tC * (0.70710678118f * (90.0f / a1));
	//v2tC = v2tC * (0.70710678118f * (90.0f / a2));

	//std::cout << glm::normalize(v2 - v0).x << " " << glm::normalize(v2 - v0).y << " " << glm::normalize(v2 - v0).z << "\n";
	//float am1 = glm::dot(glm::normalize(v0 - v1), glm::normalize(v2 - v0));
	//float am2 = glm::dot(glm::normalize(v1 - v0), glm::normalize(v2 - v1));
	//float am3 = glm::dot(glm::normalize(v2 - v0), glm::normalize(v1 - v2));

	//std::cout << "coc " << am1 << " " << am2 << " " << am3 << "\n";


	//v0tC += (v0tC * exten) / 3.0f;
	//v1tC += (v1tC * exten) / 3.0f;
	//v2tC += (v2tC * exten) / 3.0f;

	//glm::vec3 eTri0 = extendS + v0tC + v0;
	//glm::vec3 eTri1 = extendS + v1tC + v1;
	//glm::vec3 eTri2 = extendS + v2tC + v2;

	glm::vec3 eTri0 = v0;
	glm::vec3 eTri1 = v1;
	glm::vec3 eTri2 = v2;

	//std::cout << eTri0.x << " " << eTri0.y << " " << eTri0.z << "\n";
	//std::cout << eTri1.x << " " << eTri1.y << " " << eTri1.z << "\n";
	//std::cout << eTri2.x << " " << eTri2.y << " " << eTri2.z << "\n";

	//float t = 0.0f;
	
	if (glm::dot(vel, triNormal) >= 0.0f) {
		//std::cout << "NO COLL\n";
		//cent = eP;
		return false;
	}
	else {
		//if (segmentTrif(sP, eP, eTri0, eTri1, eTri2, &t)) {
		if (segmentTrif(sP, eP, eTri0, eTri1, eTri2, t)) {
			//std::cout << "COLL\n";
			//std::cout << t << "\n";
			//if (t != 0) {
				//cent += (eP - sP) * t;
			//}
			return true;
		}
		else {
			return false;
		}
	}
}

bool Collision::segmentTrif(glm::vec3 start, glm::vec3 end, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float *t) {

	glm::vec3 segDir = end - start;
	float segExtent = 0.5f * glm::length(glm::normalize(segDir));
	glm::vec3 segOrigin = 0.5f * (start + end);

	glm::vec3 diff = segOrigin - v0;
	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;
	glm::vec3 normal = glm::cross(edge1, edge2);

	bool collFound = false;

	float DdN = glm::dot(segDir, normal);
	float sign;
	if (DdN > 0.0f) {

		sign = 1.0f;
	}
	else if (DdN < 0.0f) {

		sign = -1.0f;
		DdN = -DdN;
	}
	else {

		return false;
	}

	float DdQxE2 = sign * glm::dot(segDir, glm::cross(diff, edge2));
	if (DdQxE2 >= 0.0f) {

		float DdE1xQ = sign * glm::dot(segDir, glm::cross(edge1, diff));
		if (DdE1xQ >= 0.0f) {

			if (DdQxE2 + DdE1xQ <= DdN) {

				float QdN = (-sign) * glm::dot(diff, normal);
				float extDdN = segExtent * DdN;
				if (-extDdN <= QdN && QdN <= extDdN) {

					collFound = true;
					float inv = 1.0f / DdN;
					//std::cout << " A " << QdN << " B " << inv << " S " << sign << "\n";
					*t = QdN * inv;

					return true;
				}
			}
		}
	}

	return false;
}

void Collision::toiAABBTri(glm::vec3 *cent, glm::vec3 exten, glm::vec3 vel, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {

	glm::vec3 v1v0 = v1 - v0;
	glm::vec3 v2v0 = v2 - v0;

	glm::vec3 triN = glm::cross(v1v0, v2v0);

	bool bColl = false;

	if (glm::dot(vel, triN) >= 0.0f) {
		//std::cout << "NO COLL\n";
	}
	else {
		std::vector<glm::vec3> raysAABB;
		glm::vec3 c = *cent;

		if (vel.y < 0.0f) {
			raysAABB.push_back(c - glm::vec3(0.0f, exten.y, 0.0f) + glm::vec3(exten.x, 0.0f, 0.0f));
			raysAABB.push_back(c - glm::vec3(0.0f, exten.y, 0.0f) - glm::vec3(exten.x, 0.0f, 0.0f));
			raysAABB.push_back(c - glm::vec3(0.0f, exten.y, 0.0f) + glm::vec3(0.0f, 0.0f, exten.z));
			raysAABB.push_back(c - glm::vec3(0.0f, exten.y, 0.0f) - glm::vec3(0.0f, 0.0f, exten.z));
		}
		else if (vel.y > 0.0f) {
			raysAABB.push_back(c + glm::vec3(0.0f, exten.y, 0.0f) + glm::vec3(exten.x, 0.0f, 0.0f));
			raysAABB.push_back(c + glm::vec3(0.0f, exten.y, 0.0f) - glm::vec3(exten.x, 0.0f, 0.0f));
			raysAABB.push_back(c + glm::vec3(0.0f, exten.y, 0.0f) + glm::vec3(0.0f, 0.0f, exten.z));
			raysAABB.push_back(c + glm::vec3(0.0f, exten.y, 0.0f) - glm::vec3(0.0f, 0.0f, exten.z));
		}

		if (vel.x < 0.0f) {
			raysAABB.push_back(c - glm::vec3(exten.x, 0.0f, 0.0f) + glm::vec3(0.0f, exten.y, 0.0f));
			raysAABB.push_back(c - glm::vec3(exten.x, 0.0f, 0.0f) - glm::vec3(0.0f, exten.y, 0.0f));
			raysAABB.push_back(c - glm::vec3(exten.x, 0.0f, 0.0f) + glm::vec3(0.0f, 0.0f, exten.z));
			raysAABB.push_back(c - glm::vec3(exten.x, 0.0f, 0.0f) - glm::vec3(0.0f, 0.0f, exten.z));
		}
		else if (vel.x > 0.0f) {
			raysAABB.push_back(c + glm::vec3(exten.x, 0.0f, 0.0f) + glm::vec3(0.0f, exten.y, 0.0f));
			raysAABB.push_back(c + glm::vec3(exten.x, 0.0f, 0.0f) - glm::vec3(0.0f, exten.y, 0.0f));
			raysAABB.push_back(c + glm::vec3(exten.x, 0.0f, 0.0f) + glm::vec3(0.0f, 0.0f, exten.z));
			raysAABB.push_back(c + glm::vec3(exten.x, 0.0f, 0.0f) - glm::vec3(0.0f, 0.0f, exten.z));
		}

		if (vel.z < 0.0f) {
			raysAABB.push_back(c - glm::vec3(0.0f, 0.0f, exten.z) + glm::vec3(0.0f, exten.y, 0.0f));
			raysAABB.push_back(c - glm::vec3(0.0f, 0.0f, exten.z) - glm::vec3(0.0f, exten.y, 0.0f));
			raysAABB.push_back(c - glm::vec3(0.0f, 0.0f, exten.z) + glm::vec3(exten.x, 0.0f, 0.0f));
			raysAABB.push_back(c - glm::vec3(0.0f, 0.0f, exten.z) - glm::vec3(exten.x, 0.0f, 0.0f));
		}
		else if (vel.z > 0.0f) {
			raysAABB.push_back(c + glm::vec3(0.0f, 0.0f, exten.z) + glm::vec3(0.0f, exten.y, 0.0f));
			raysAABB.push_back(c + glm::vec3(0.0f, 0.0f, exten.z) - glm::vec3(0.0f, exten.y, 0.0f));
			raysAABB.push_back(c + glm::vec3(0.0f, 0.0f, exten.z) + glm::vec3(exten.x, 0.0f, 0.0f));
			raysAABB.push_back(c + glm::vec3(0.0f, 0.0f, exten.z) - glm::vec3(exten.x, 0.0f, 0.0f));
		}

		float shortest = 9999.9f;
		float shortDistance = 1.0f;
		float t = 10000.0f;
		int index = -1;
		for (int i = 0; i < raysAABB.size(); i++) {

			if (rayTrif(raysAABB[i], vel, v0, v1, v2, &t)) {
				if (t < shortest) {
					index = i;
					shortest = t;
				}
			}
		}
		//std::cout << "Shortest " << shortest << "\n";
		//std::cout << "dis " << shortest - glm::length(vel) << "\n";
		//std::cout << c.x << " " << c.y << " " << c.z << "\n";
		std::cout << "B " << raysAABB[index].x << " " << raysAABB[index].y << " " << raysAABB[index].z << "\n";
		std::cout << " " << shortest - glm::length(vel) << "\n";
		if (shortest - glm::length(vel) < shortDistance) {
			if (index != -1) {
				bColl = true;
				float VdN = glm::dot(vel, triN);
				triN = glm::normalize(triN);
				glm::vec3 velt = glm::normalize(vel);
				glm::vec3 check = glm::normalize(glm::cross(velt, triN));
				glm::vec3 test = glm::normalize(glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), glm::cross(velt, triN)));
				if (abs(velt.x) == abs(triN.x) && abs(velt.y) == abs(triN.y) && abs(velt.z) == abs(triN.z)) {
					test = glm::vec3(0.0f, 0.0f, 0.0f);
				}
				if (check.x == -1 && abs(check.y) == 0 && abs(check.z) == 0) {
					test = glm::vec3(0.0f, 0.0f, 0.0f);
					std::cout << "o\n";
				}
				std::cout << " a " << check.x << " " << check.y << " " << check.z << "\n";
				*cent += (vel * (shortest)) + (test * 0.001f);
			}
			//if (index >= 0) {
				//std::cout << "COLL\n";
				//std::cout << triN.y << "\n";
				//bColl = true;
				//glm::vec3 tempTras = (glm::normalize(triN) * shortest);
				//std::cout << tempTras.x << " " << tempTras.y << " " << tempTras.z << "\n";
				//*cent -= tempTras;
			//}
		}
		//*cent += (vel * (shortest));
		//std::cin >> shortest;
	}

	if (bColl == false) {
		*cent += vel;
	}
}

bool Collision::rayTrif(glm::vec3 orig, glm::vec3 dir, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float *t) {

	glm::vec3 diff = orig - v0;
	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;
	glm::vec3 norm = glm::cross(edge1, edge2);

	float DdN = glm::dot(dir, norm);
	float sign = 1.0f;

	bool para = false;

	if (DdN > 0.0f) {
		sign = 1.0f;
	}
	else if (DdN < 0.0f) {
		sign = -1.0f;
		DdN = -DdN;
	}
	else {
		return false;
	}

	float DdDiE2 = (sign)*glm::dot(dir, glm::cross(diff, edge2));
	if (DdDiE2 >= 0.0f) {

		float DdE1Di = (sign)*glm::dot(dir, glm::cross(edge1, diff));
		if (DdE1Di >= 0.0f) {

			if (DdDiE2 + DdE1Di <= DdN) {

				float QdN = (-sign)*glm::dot(diff, norm);
				if (QdN >= 0.0f) {
					*t = QdN * (1.0f / DdN);
					return true;
				}
			}
		}
	}
	return false;
}

bool Collision::satCollisionOOBTri(glm::vec3 cent, glm::vec3 exten, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {

	v0 -= cent;
	v1 -= cent;
	v2 -= cent;

	glm::vec3 f0 = v1 - v0;
	glm::vec3 f1 = v2 - v1;
	glm::vec3 f2 = v0 - v2;

	//AABB, need to convert to OOB
	glm::vec3 u0 = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 u1 = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 u2 = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::vec3 aAxis[9];

	aAxis[0] = glm::cross(u0, f0);
	aAxis[1] = glm::cross(u0, f1);
	aAxis[2] = glm::cross(u0, f2);

	aAxis[3] = glm::cross(u1, f0);
	aAxis[4] = glm::cross(u1, f1);
	aAxis[5] = glm::cross(u1, f2);

	aAxis[6] = glm::cross(u2, f0);
	aAxis[7] = glm::cross(u2, f1);
	aAxis[8] = glm::cross(u2, f2);
	
	for (int i = 0; i < 9; i++) {
		if (satAxisProjection(aAxis[i], exten, v0, v1, v2, u0, u1, u2) == false) {

			return false;
		}
	}

	if (satAxisProjection(u0, exten, v0, v1, v2, u0, u1, u2) == false) {

		return false;
	}

	if (satAxisProjection(u1, exten, v0, v1, v2, u0, u1, u2) == false) {

		return false;
	}

	if (satAxisProjection(u2, exten, v0, v1, v2, u0, u1, u2) == false) {

		return false;
	}

	glm::vec3 triNormal = glm::cross(f0, f1);

	if (satAxisProjection(triNormal, exten, v0, v1, v2, u0, u1, u2) == false) {

		return false;
	}

	return true;
}

bool Collision::satAxisProjection(glm::vec3 axis, glm::vec3 exten, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 u0, glm::vec3 u1, glm::vec3 u2) {

	float p0 = glm::dot(v0, axis);
	float p1 = glm::dot(v1, axis);
	float p2 = glm::dot(v2, axis);

	float r = exten.x * abs(glm::dot(u0, axis)) + exten.y * abs(glm::dot(u1, axis)) + exten.z * abs(glm::dot(u2, axis));

	if (std::max(-maxThree(p0, p1, p2), minThree(p0, p1, p2)) > r) {

		return false;
	}
}

float Collision::maxThree(float x, float y, float z) {

	return std::max(std::max(x, y), z);
}

float Collision::minThree(float x, float y, float z) {

	return std::min(std::min(x, y), z);
}

bool Collision::satCollisionOOBTriStatic(glm::vec3 *cent, glm::vec3 exten, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {

	v0 -= *cent;
	v0 -= *cent;
	v0 -= *cent;

	glm::vec3 f0 = v1 - v0;
	glm::vec3 f1 = v2 - v1;
	glm::vec3 f2 = v0 - v2;

	//AABB, need to convert to OOB
	glm::vec3 u0 = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 u1 = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 u2 = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::vec3 aAxis[9];

	aAxis[0] = glm::cross(u0, f0);
	aAxis[1] = glm::cross(u0, f1);
	aAxis[2] = glm::cross(u0, f2);

	aAxis[3] = glm::cross(u1, f0);
	aAxis[4] = glm::cross(u1, f1);
	aAxis[5] = glm::cross(u1, f2);

	aAxis[6] = glm::cross(u2, f0);
	aAxis[7] = glm::cross(u2, f1);
	aAxis[8] = glm::cross(u2, f2);

	float t = 9999.9f;
	float overlap = 9999.9f;
	glm::vec3 cAxis = glm::vec3(0.0f);

	for (int i = 0; i < 9; i++) {
		if (aAxis[i] != glm::vec3(0.0f, 0.0f, 0.0f)) {
			if (satAxisProjectionStatic(aAxis[i], exten, v0, v1, v2, u0, u1, u2, &t) == false) {

				return false;
			}
			else {
				if (abs(t) < abs(overlap)) {
					overlap = t;
					cAxis = aAxis[i];
					//std::cout << i << " i " << aAxis[i].x << " " << aAxis[i].y << " " << aAxis[i].z << "\n";
				}
			}
		}
	}

	if (satAxisProjectionStatic(u0, exten, v0, v1, v2, u0, u1, u2, &t) == false) {

		return false;
	}
	else {
		if (abs(t) < abs(overlap)) {
			overlap = t;
			cAxis = u0;
			std::cout << "A" << "\n";
		}
	}

	if (satAxisProjectionStatic(u1, exten, v0, v1, v2, u0, u1, u2, &t) == false) {

		return false;
	}
	else {
		if (abs(t) < abs(overlap)) {
			overlap = t;
			cAxis = u1;
			std::cout << "B" << "\n";
		}
	}

	if (satAxisProjectionStatic(u2, exten, v0, v1, v2, u0, u1, u2, &t) == false) {

		return false;
	}
	else {
		if (abs(t) < abs(overlap)) {
			overlap = t;
			cAxis = u2;
			std::cout << "C" << "\n";
		}
	}

	glm::vec3 triNormal = glm::cross(f0, f1);

	if (satAxisProjectionStatic(triNormal, exten, v0, v1, v2, u0, u1, u2, &t) == false) {

		return false;
	}
	else {
		if (abs(t) < abs(overlap)) {
			overlap = t;
			cAxis = glm::vec3(0.0f, 1.0f, 0.0f);
			std::cout << "D" << "\n";
		}
	}
	cAxis = glm::normalize(cAxis);

	glm::vec3 finalV = ((cAxis * overlap) + (cAxis * exten));
	*cent += ((cAxis * overlap) + (cAxis * exten));
	
	std::cout << overlap << "\n";
	std::cout << cAxis.x << " " << cAxis.y << " " << cAxis.z << "\n";

	return true;
}

bool Collision::satAxisProjectionStatic(glm::vec3 axis, glm::vec3 exten, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 u0, glm::vec3 u1, glm::vec3 u2, float *t) {

	float p0 = glm::dot(v0, axis);
	float p1 = glm::dot(v1, axis);
	float p2 = glm::dot(v2, axis);

	float r = exten.x * abs(glm::dot(u0, axis)) + exten.y * abs(glm::dot(u1, axis)) + exten.z * abs(glm::dot(u2, axis));
	//std::cout << r << "\n";
	if (std::max(-maxThree(p0, p1, p2), minThree(p0, p1, p2)) > r) {

		return false;
	}
	if ((abs(p1) > abs(p0)) && abs(p2) > abs(p0)) {
		*t = p0;
	}else if ((abs(p0) > abs(p1)) && abs(p2) > abs(p1)) {
		*t = p1;
	}
	else {
		*t = p2;
	}
	if (*t == 0.0f) {
		*t = 9999.9f;
	}
	return true;
}

bool Collision::rayAABB(glm::vec3 min, glm::vec3 max, glm::vec3 orig, glm::vec3 dir) {

	glm::vec3 bounds[2];
	bounds[0] = min;
	bounds[1] = max;

	glm::vec3 invdir = 1.0f / dir;
	int sign[3];
	sign[0] = (invdir.x < 0);
	sign[1] = (invdir.y < 0);
	sign[2] = (invdir.z < 0);

	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds[sign[0]].x - orig.x) * invdir.x;
	tmax = (bounds[1 - sign[0]].x - orig.x) * invdir.x;
	tymin = (bounds[sign[1]].y - orig.y) * invdir.y;
	tymax = (bounds[1 - sign[1]].y - orig.y) * invdir.y;

	if ((tmin > tymax) || (tymin > tmax)) {

		return false;
	}
	if (tymin > tmin) {
		tmin = tymin;
	}
	if (tymax < tmax) {
		tmax = tymax;
	}

	tzmin = (bounds[sign[2]].z - orig.z) * invdir.z;
	tzmax = (bounds[1 - sign[2]].z - orig.z) * invdir.z;

	if ((tmin > tzmax) || (tzmin > tmax)) {
		return false;
	}
	if (tzmin > tmin) {
		tmin = tzmin;
	}
	if (tzmax < tmax) {
		tmax = tzmax;
	}

	if (tmin < 0.0f && tmax < 0.0f) {
		return false;
	}

	return true;
}

float Collision::rayAABBf(glm::vec3 min, glm::vec3 max, glm::vec3 orig, glm::vec3 dir) {

	glm::vec3 bounds[2];
	bounds[0] = min;
	bounds[1] = max;

	glm::vec3 invdir = 1.0f / dir;
	int sign[3];
	sign[0] = (invdir.x < 0);
	sign[1] = (invdir.y < 0);
	sign[2] = (invdir.z < 0);

	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds[sign[0]].x - orig.x) * invdir.x;
	tmax = (bounds[1 - sign[0]].x - orig.x) * invdir.x;
	tymin = (bounds[sign[1]].y - orig.y) * invdir.y;
	tymax = (bounds[1 - sign[1]].y - orig.y) * invdir.y;

	if ((tmin > tymax) || (tymin > tmax)) {

		return -9.0f;
	}
	if (tymin > tmin) {
		tmin = tymin;
	}
	if (tymax < tmax) {
		tmax = tymax;
	}

	tzmin = (bounds[sign[2]].z - orig.z) * invdir.z;
	tzmax = (bounds[1 - sign[2]].z - orig.z) * invdir.z;

	if ((tmin > tzmax) || (tzmin > tmax)) {
		return -9.0f;
	}
	if (tzmin > tmin) {
		tmin = tzmin;
	}
	if (tzmax < tmax) {
		tmax = tzmax;
	}

	if (tmin < 0.0f && tmax < 0.0f) {
		return -9.0f;
	}

	if (tmin < tmax) {
		return tmin;
	}

	return tmax;
}

float Collision::rayPlaneIntersection(glm::vec3 dir, glm::vec3 orig, glm::vec3 center, glm::vec3 normal) {

	float denom = glm::dot(normal, dir);
	if (abs(denom) > 0.0001f) {
		float t = glm::dot((center - orig), normal) / denom;
		if (t >= 0) return t;
	}
	return -9999.9f;
}