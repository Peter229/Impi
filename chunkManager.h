#pragma once

#include <unordered_map>
#include <glm/glm.hpp>
#include "chunk.h"
#include <memory>

class chunkManager {

public:

	chunkManager();
	~chunkManager();

	void createChunk(int x, int z, int seed);

	void render(Shader *shader);

	void cleanUp();

	bool checkChunk(int x, int z);

	void giveNeighbours();

	void removeChunks(int x, int z);

private:
	std::unordered_map<int, chunk*> chunks;
};