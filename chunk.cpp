#include "chunk.h"

chunk::chunk() {
	gx = 0;
	gz = 0;
	seed = 0;
	model = glm::mat4(1.0f);
	shouldRender = false;
	hasNeighbours = false;
}

chunk::chunk(int gx, int gz, int seed) {

	this->gx = gx;
	this->gz = gz;
	this->seed = seed;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(gx, 0, gz));
	shouldRender = false;
	hasNeighbours = false;
	genTerrain();
}

chunk::~chunk() {

	if (shouldRender) {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);

		tris.clear();
	}
	//delete[] cubes;
	//delete[] cubes;
}


void chunk::render(Shader *shader) {

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(VAO);
	shader->setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, tris.size() * 3);

}

void chunk::cleanUp() {

	if (shouldRender) {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);

		tris.clear();
	}
}

void chunk::setNeighbourChunks(chunk* neChunks[4]) {

	nChunks[0] = neChunks[0];
	nChunks[1] = neChunks[1];
	nChunks[2] = neChunks[2];
	nChunks[3] = neChunks[3];
	hasNeighbours = true;
	buildMeshMarchingCubes();
}

bool chunk::getHasNeigbours() {

	return hasNeighbours;
}

void chunk::genTerrain() {

	PerlinNoise sn(seed);

	PerlinNoise bn(seed+32*15);

	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			for (int z = 0; z < size; z++) {

				float b = 100 * ((0.11 * bn.noise(0.0031 * (x + gx), 0.0031 * y, 0.0031 * (z + gz))));
				//std::cout << b << "\n";
				/*float g = 1.0f, h = 1.0f;
				if (b > 5.8) {
					g = 100 * ((0.11 * sn.noise(0.031 * (x + gx), 0.031 * (z + gz), 0.55)));
					g = pow(g, 1.2);
					h = 100 * ((0.11 * sn.noise(0.031 * (x + gx), 0.031 * y, 0.031 * (z + gz))));
				}
				else {
					g = 100 * ((0.11 * sn.noise(0.021 * (x + gx), 0.021 * (z + gz), 0.55)));
					g = pow(g, 1.2);
					//h = 100 * ((0.11 * sn.noise(0.00031 * (x + gx), 0.00031 * y, 0.00031 * (z + gz))));
				}
				//float g = 100 * ((0.11 * sn.noise(0.031 * (x + gx), 0.031 * (z + gz), 0.55)));
				//g = pow(g, 1.2);
				//float h = 100 * ((0.11 * sn.noise(0.031 * (x + gx), 0.031 * y, 0.031 * (z + gz))));
				//std::cout << h << "\n";
				//std::cout << h << "\n";

				*/
				float h = 1.0f;
				if (b > 9) {
					h = -y + 50 * ((0.91 * sn.noise(0.031 * (x + gx), 0.11 * y, 0.031 * (z + gz))));
					h = pow(h, 1.3);
				}
				else {
					h = -y + 50 * ((1.5 * sn.noise(0.031 * (x + gx), 0.11 * y, 0.031 * (z + gz))));
					//h = pow(h, 1.35);
				}
				//h = (-y +  ((sn.noise(0.031 * (x + gx), 0.11 * y, 0.031 * (z + gz))))) * 100;
				h = -y + (sn.noise((x + gx) * 0.011, y * 0.03, (z + gz) * 0.011) * 50);
				//std::cout << h << "\n";
				//h = 5 - glm::length(glm::vec3(x, y, z)) + 50 * ((1.5 * sn.noise(0.031 * (x + gx), 0.11 * (y+5), 0.031 * (z + gz))));
				//if (y < h /*|| y < g*/) {
				//	cubes[x + size * (y + size * z)] = 1;
				//}
				//else {
				//	cubes[x + size * (y + size * z)] = 0;
				//}
				cubes[x + size * (y + size * z)] = h;
			}
		}
	}

	//buildMesh();
}

void chunk::buildMeshMarchingCubes() {

	for (int x = 0; x < size - 1; x++) {
		for (int y = 0; y < size - 1; y++) {
			for (int z = 0; z < size - 1; z++) {

				int cubeIndex = 0;

				glm::vec3 cubeCorners[] = {
					glm::vec3(x, y, z),
					glm::vec3(x + 1, y, z),
					glm::vec3(x + 1, y, z + 1),
					glm::vec3(x, y, z + 1),
					glm::vec3(x, y + 1, z),
					glm::vec3(x + 1, y + 1, z),
					glm::vec3(x + 1, y + 1, z + 1),
					glm::vec3(x, y + 1, z + 1)
				};

				float cubeCornerValues [] = {
					cubes[x + size * (y + size * z)],
					cubes[(x + 1) + size * (y + size * z)],
					cubes[(x + 1) + size * (y + size * (z + 1))],
					cubes[x + size * (y + size * (z + 1))],
					cubes[x + size * ((y + 1) + size * z)],
					cubes[(x + 1) + size * ((y + 1) + size * z)],
					cubes[(x + 1) + size * ((y + 1) + size * (z + 1))],
					cubes[x + size * ((y + 1) + size * (z + 1))]
				};

				float surfaceHeight = 6.5f;

				if (cubes[x + size * (y + size * z)] > surfaceHeight) {
					cubeIndex |= 1;
				}

				if (cubes[(x + 1) + size * (y + size * z)] > surfaceHeight) {
					cubeIndex |= 2;
				}

				if (cubes[(x + 1) + size * (y + size * (z + 1))] > surfaceHeight) {
					cubeIndex |= 4;
				}

				if (cubes[x + size * (y + size * (z + 1))] > surfaceHeight) {
					cubeIndex |= 8;
				}

				if (cubes[x + size * ((y + 1) + size * z)] > surfaceHeight) {
					cubeIndex |= 16;
				}

				if (cubes[(x + 1) + size * ((y + 1) + size * z)] > surfaceHeight) {
					cubeIndex |= 32;
				}

				if (cubes[(x + 1) + size * ((y + 1) + size * (z + 1))] > surfaceHeight) {
					cubeIndex |= 64;
				}

				if (cubes[x + size * ((y + 1) + size * (z + 1))] > surfaceHeight) {
					cubeIndex |= 128;
				}

				//int* triangulation = triTable[cubeIndex];

				//glm::vec3 norm = glm::vec3(0.0f, 1.0f, 0.0f);

				for (int i = 0; triTable[cubeIndex][i] != -1; i += 3) {

					int a0 = cornerIndexAFromEdge[triTable[cubeIndex][i]];
					int b0 = cornerIndexBFromEdge[triTable[cubeIndex][i]];

					int a1 = cornerIndexAFromEdge[triTable[cubeIndex][i + 1]];
					int b1 = cornerIndexBFromEdge[triTable[cubeIndex][i + 1]];

					int a2 = cornerIndexAFromEdge[triTable[cubeIndex][i + 2]];
					int b2 = cornerIndexBFromEdge[triTable[cubeIndex][i + 2]];

					//glm::vec3 norm = glm::normalize(glm::cross(((cubeCorners[a2] + cubeCorners[b2]) / glm::vec3(2.0f)) - ((cubeCorners[a0] + cubeCorners[b0]) / glm::vec3(2.0f)), ((cubeCorners[a1] + cubeCorners[b1]) / glm::vec3(2.0f)) - ((cubeCorners[a0] + cubeCorners[b0]) / glm::vec3(2.0f))));

					//glm::vec3 norm = glm::vec3(0.0f, -1.0f, 0.0f);

					float t = (surfaceHeight - cubeCornerValues[a0]) / (cubeCornerValues[b0] - cubeCornerValues[a0]);
					glm::vec3 v1 = cubeCorners[a0] + t * (cubeCorners[b0] - cubeCorners[a0]);
					t = (surfaceHeight - cubeCornerValues[a1]) / (cubeCornerValues[b1] - cubeCornerValues[a1]);
					glm::vec3 v2 = cubeCorners[a1] + t * (cubeCorners[b1] - cubeCorners[a1]);
					t = (surfaceHeight - cubeCornerValues[a2]) / (cubeCornerValues[b2] - cubeCornerValues[a2]);
					glm::vec3 v3 = cubeCorners[a2] + t * (cubeCorners[b2] - cubeCorners[a2]);

					glm::vec3 norm = glm::normalize(glm::cross((v3 - v1), (v2 - v1)));

					//glm::vec3 v1 = (cubeCorners[a0] + cubeCorners[b0]) / glm::vec3(2.0f);
					//glm::vec3 v2 = (cubeCorners[a1] + cubeCorners[b1]) / glm::vec3(2.0f);
					//glm::vec3 v3 = (cubeCorners[a2] + cubeCorners[b2]) / glm::vec3(2.0f);

					tris.push_back({ v1, glm::vec3(0.8, 0.1, 0.1), norm,
						v2, glm::vec3(0.8, 0.1, 0.1), norm,
						v3, glm::vec3(0.8, 0.1, 0.1), norm });
				}
			}
		}
	}

	/*int cubeIndex = 0;

	int x = 0, y = 0, z = 0;

	glm::vec3 cubeCorners[] = {
		glm::vec3(x, y, z),
		glm::vec3(x + 1, y, z),
		glm::vec3(x + 1, y, z + 1),
		glm::vec3(x, y, z + 1),
		glm::vec3(x, y + 1, z),
		glm::vec3(x + 1, y + 1, z),
		glm::vec3(x + 1, y + 1, z + 1),
		glm::vec3(x, y + 1, z + 1)
	};

	cubes[x + size * ((y + 1) + size * z)] = 1;
	cubes[x + size * ((y + 1) + size * (z + 1))] = 1;
	cubes[(x + 1) + size * ((y + 1) + size * z)] = 1;
	cubes[(x + 1) + size * ((y + 1) + size * (z + 1))] = 1;

	cubes[x + size * (y + size * z)] = 0;
	cubes[x + size * (y + size * (z + 1))] = 0;
	cubes[(x + 1) + size * (y + size * z)] = 0;
	cubes[(x + 1) + size * (y + size * (z + 1))] = 0;

	if (cubes[x + size * (y + size * z)] == 0) {
		cubeIndex |= 1;
	}

	if (cubes[(x + 1) + size * (y + size * z)] == 0) {
		cubeIndex |= 2;
	}

	if (cubes[(x + 1) + size * (y + size * (z + 1))] == 0) {
		cubeIndex |= 4;
	}

	if (cubes[x + size * (y + size * (z + 1))] == 0) {
		cubeIndex |= 8;
	}

	if (cubes[x + size * ((y + 1) + size * z)] == 0) {
		cubeIndex |= 16;
	}

	if (cubes[(x + 1) + size * ((y + 1) + size * z)] == 0) {
		cubeIndex |= 32;
	}

	if (cubes[(x + 1) + size * ((y + 1) + size * (z + 1))] == 0) {
		cubeIndex |= 64;
	}

	if (cubes[x + size * ((y + 1) + size * (z + 1))] == 0) {
		cubeIndex |= 128;
	}

	std::cout << "crea\n";

	//int* triangulation = triTable[cubeIndex];

	//glm::vec3 norm = glm::vec3(0.0f, 1.0f, 0.0f);

	for (int i = 0; triTable[cubeIndex][i] != -1; i += 3) {

		int a0 = cornerIndexAFromEdge[triTable[cubeIndex][i]];
		int b0 = cornerIndexAFromEdge[triTable[cubeIndex][i]];

		int a1 = cornerIndexAFromEdge[triTable[cubeIndex][i + 1]];
		int b1 = cornerIndexAFromEdge[triTable[cubeIndex][i + 1]];

		int a2 = cornerIndexAFromEdge[triTable[cubeIndex][i + 2]];
		int b2 = cornerIndexAFromEdge[triTable[cubeIndex][i + 2]];

		glm::vec3 norm = glm::normalize(glm::cross(((cubeCorners[a2] + cubeCorners[b2]) / glm::vec3(2.0f)) - ((cubeCorners[a0] + cubeCorners[b0]) / glm::vec3(2.0f)), ((cubeCorners[a1] + cubeCorners[b1]) / glm::vec3(2.0f)) - ((cubeCorners[a0] + cubeCorners[b0]) / glm::vec3(2.0f))));

		//glm::vec3 norm = glm::vec3(0.0f, -1.0f, 0.0f);

		std::cout << ((cubeCorners[a0] + cubeCorners[b0]) / glm::vec3(2.0f)).x << " " << ((cubeCorners[a0] + cubeCorners[b0]) / glm::vec3(2.0f)).y << " " << ((cubeCorners[a0] + cubeCorners[b0]) / glm::vec3(2.0f)).z << "\n";
		std::cout << ((cubeCorners[a1] + cubeCorners[b1]) / glm::vec3(2.0f)).x << " " << ((cubeCorners[a1] + cubeCorners[b1]) / glm::vec3(2.0f)).y << " " << ((cubeCorners[a1] + cubeCorners[b1]) / glm::vec3(2.0f)).z << "\n";
		std::cout << ((cubeCorners[a2] + cubeCorners[b2]) / glm::vec3(2.0f)).x << " " << ((cubeCorners[a2] + cubeCorners[b2]) / glm::vec3(2.0f)).y << " " << ((cubeCorners[a2] + cubeCorners[b2]) / glm::vec3(2.0f)).z << "\n";

		tris.push_back({ (cubeCorners[a0] + cubeCorners[b0]) / glm::vec3(2.0f), glm::vec3(0.8, 0.1, 0.1), norm,
			(cubeCorners[a1] + cubeCorners[b1]) / glm::vec3(2.0f), glm::vec3(0.8, 0.1, 0.1), norm,
			(cubeCorners[a2] + cubeCorners[b2]) / glm::vec3(2.0f), glm::vec3(0.8, 0.1, 0.1), norm });
	}
	*/
	if (tris.size() == 0) {
		shouldRender = false;
	}
	else {
		shouldRender = true;
		genVAO();
	}
}

void chunk::buildMesh() {

	/*PerlinNoise dn(seed);

	glm::vec3 cubeColourGreen = { 0.0, 1.0, 0.0 };
	glm::vec3 cubeColourWhite = { 1.0, 1.0, 1.0 };
	glm::vec3 cubeColourGrey = { 0.5, 0.5, 0.5 };
	glm::vec3 cubeColourDarken = { 0.03, 0.03, 0.03 };
	glm::vec3 cubeColour = { 0.1, 1.0, 0.1 };
	//Left right back front

	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			for (int z = 0; z < size; z++) {
				if (cubes[x + size * (y + size * z)] != 0) {
					if (y < size - 1) {
						float t = 5 * dn.noise(0.11 * (gx + x), 0.5, 0.11 * (gz + z));
						if (cubes[x + size * ((y + 1) + size * z)] == 0) {
							if (y > 25) {
								cubeColour = cubeColourWhite - (cubeColourDarken * glm::vec3(t));
							}
							else {
								cubeColour = cubeColourGreen - (cubeColourDarken * glm::vec3(t));
							}
							cubeColour -= glm::vec3(32 - y) * cubeColourDarken;
							tris.push_back({ glm::vec3(x, y + 1.0f, z), cubeColour, glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), cubeColour, glm::vec3(x + 1.0f, y + 1.0f, z), cubeColour });
							tris.push_back({ glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), cubeColour, glm::vec3(x, y + 1.0f, z), cubeColour, glm::vec3(x, y + 1.0f, z + 1.0f), cubeColour });
						}
						else {
							cubeColour = cubeColourGrey;
							cubeColour -= glm::vec3(32 - y) * (cubeColourDarken * glm::vec3(0.20) * glm::vec3(t));
						}
					}
					if (y > 0) {
						if (cubes[x + size * ((y - 1) + size * z)] == 0) {

							tris.push_back({ glm::vec3(x, y, z), cubeColour, glm::vec3(x + 1.0f, y, z), cubeColour, glm::vec3(x + 1.0f, y, z + 1.0f), cubeColour });
							tris.push_back({ glm::vec3(x + 1.0f, y, z + 1.0f), cubeColour, glm::vec3(x, y, z + 1.0f), cubeColour, glm::vec3(x, y, z), cubeColour });
						}
					}

					if (z > 0) {
						if (cubes[x + size * (y + size * (z - 1))] == 0) {

							tris.push_back({ glm::vec3(x, y, z), cubeColour, glm::vec3(x + 1.0f, y + 1.0f, z), cubeColour, glm::vec3(x + 1.0f, y, z), cubeColour });
							tris.push_back({ glm::vec3(x + 1.0f, y + 1.0f, z), cubeColour, glm::vec3(x, y, z), cubeColour, glm::vec3(x, y + 1.0f, z), cubeColour });
						}
					}
					else {
						if (nChunks[2]->cubes[x + size * (y + size * (31))] == 0) {
							tris.push_back({ glm::vec3(x, y, z), cubeColour, glm::vec3(x + 1.0f, y + 1.0f, z), cubeColour, glm::vec3(x + 1.0f, y, z), cubeColour });
							tris.push_back({ glm::vec3(x + 1.0f, y + 1.0f, z), cubeColour, glm::vec3(x, y, z), cubeColour, glm::vec3(x, y + 1.0f, z), cubeColour });
						}
					}

					if (z < size - 1) {
						if (cubes[x + size * (y + size * (z + 1))] == 0) {

							tris.push_back({ glm::vec3(x, y, z + 1.0f), cubeColour, glm::vec3(x + 1.0f, y, z + 1.0f), cubeColour, glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), cubeColour });
							tris.push_back({ glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), cubeColour, glm::vec3(x, y + 1.0f, z + 1.0f), cubeColour, glm::vec3(x, y, z + 1.0f), cubeColour });
						}
					}
					else {
						if (nChunks[3]->cubes[x + size * (y + size * (0))] == 0) {
							tris.push_back({ glm::vec3(x, y, z + 1.0f), cubeColour, glm::vec3(x + 1.0f, y, z + 1.0f), cubeColour, glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), cubeColour });
							tris.push_back({ glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), cubeColour, glm::vec3(x, y + 1.0f, z + 1.0f), cubeColour, glm::vec3(x, y, z + 1.0f), cubeColour });
						}
					}

					if (x > 0) {
						if (cubes[(x - 1) + size * (y + size * z)] == 0) {

							tris.push_back({ glm::vec3(x, y + 1.0f, z + 1.0f), cubeColour, glm::vec3(x, y + 1.0f, z), cubeColour, glm::vec3(x, y, z), cubeColour });
							tris.push_back({ glm::vec3(x, y, z), cubeColour, glm::vec3(x, y, z + 1.0f), cubeColour, glm::vec3(x, y + 1.0f, z + 1.0f), cubeColour });
						}
					}
					else {
						if (nChunks[0]->cubes[(31) + size * (y + size * z)] == 0) {
							tris.push_back({ glm::vec3(x, y + 1.0f, z + 1.0f), cubeColour, glm::vec3(x, y + 1.0f, z), cubeColour, glm::vec3(x, y, z), cubeColour });
							tris.push_back({ glm::vec3(x, y, z), cubeColour, glm::vec3(x, y, z + 1.0f), cubeColour, glm::vec3(x, y + 1.0f, z + 1.0f), cubeColour });
						}
					}

					if (x < size - 1) {
						if (cubes[(x + 1) + size * (y + size * z)] == 0) {

							tris.push_back({ glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), cubeColour, glm::vec3(x + 1.0f, y, z), cubeColour, glm::vec3(x + 1.0f, y + 1.0f, z), cubeColour });
							tris.push_back({ glm::vec3(x + 1.0f, y, z), cubeColour, glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), cubeColour, glm::vec3(x + 1.0f, y, z + 1.0f), cubeColour });
						}
					}
					else {
						if (nChunks[1]->cubes[(0) + size * (y + size * z)] == 0) {
							tris.push_back({ glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), cubeColour, glm::vec3(x + 1.0f, y, z), cubeColour, glm::vec3(x + 1.0f, y + 1.0f, z), cubeColour });
							tris.push_back({ glm::vec3(x + 1.0f, y, z), cubeColour, glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f), cubeColour, glm::vec3(x + 1.0f, y, z + 1.0f), cubeColour });
						}
					}
				}
			}
		}
	}

	if (tris.size() == 0) {
		shouldRender = false;
	}
	else {
		shouldRender = true;
		genVAO();
	}*/
}

bool chunk::getShouldRender() {

	return shouldRender;
}

void chunk::genVAO() {

	/*glm::vec3 cubeColour = { 0.1, 1.0, 0.1 };
	glm::vec3 cubeColour2 = { 1.0, 0.1, 0.1 };
	glm::vec3 cubeColour3 = { 0.1, 0.1, 1.0 };

	tris.push_back({ glm::vec3(0.0f, 0.0f, 0.0f), cubeColour, glm::vec3(1.0f, 1.0f, 0.0f), cubeColour, glm::vec3(1.0f, 0.0f, 0.0f), cubeColour });
	tris.push_back({ glm::vec3(1.0f, 1.0f, 0.0f), cubeColour, glm::vec3(0.0f, 0.0f, 0.0f), cubeColour, glm::vec3(0.0f, 1.0f, 0.0f), cubeColour });

	tris.push_back({ glm::vec3(0.0f, 0.0f, 1.0f), cubeColour, glm::vec3(1.0f, 0.0f, 1.0f), cubeColour, glm::vec3(1.0f, 1.0f, 1.0f), cubeColour });
	tris.push_back({ glm::vec3(1.0f, 1.0f, 1.0f), cubeColour, glm::vec3(0.0f, 1.0f, 1.0f), cubeColour, glm::vec3(0.0f, 0.0f, 1.0f), cubeColour });

	tris.push_back({ glm::vec3(0.0f, 1.0f, 1.0f), cubeColour2, glm::vec3(0.0f, 1.0f, 0.0f), cubeColour2, glm::vec3(0.0f, 0.0f, 0.0f), cubeColour2 });
	tris.push_back({ glm::vec3(0.0f, 0.0f, 0.0f), cubeColour2, glm::vec3(0.0f, 0.0f, 1.0f), cubeColour2, glm::vec3(0.0f, 1.0f, 1.0f), cubeColour2 });

	tris.push_back({ glm::vec3(1.0f, 1.0f, 1.0f), cubeColour, glm::vec3(1.0f, 0.0f, 0.0f), cubeColour, glm::vec3(1.0f, 1.0f, 0.0f), cubeColour });
	tris.push_back({ glm::vec3(1.0f, 0.0f, 0.0f), cubeColour, glm::vec3(1.0f, 1.0f, 1.0f), cubeColour, glm::vec3(1.0f, 0.0f, 1.0f), cubeColour });

	tris.push_back({ glm::vec3(0.0f, 0.0f, 0.0f), cubeColour3, glm::vec3(1.0f, 0.0f, 0.0f), cubeColour3, glm::vec3(1.0f, 0.0f, 1.0f), cubeColour3 });
	tris.push_back({ glm::vec3(1.0f, 0.0f, 1.0f), cubeColour3, glm::vec3(0.0f, 0.0f, 1.0f), cubeColour3, glm::vec3(0.0f, 0.0f, 0.0f), cubeColour3 });

	tris.push_back({ glm::vec3(0.0f, 1.0f, 0.0f), cubeColour, glm::vec3(1.0f, 1.0f, 1.0f), cubeColour, glm::vec3(1.0f, 1.0f, 0.0f), cubeColour });
	tris.push_back({ glm::vec3(1.0f, 1.0f, 1.0f), cubeColour, glm::vec3(0.0f, 1.0f, 0.0f), cubeColour, glm::vec3(0.0f, 1.0f, 1.0f), cubeColour });*/

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriN) * tris.size(), &tris[0], GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//Colour attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

glm::vec2 chunk::getChunkPos() {

	return glm::vec2(gx, gz);
}