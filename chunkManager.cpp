#include "chunkManager.h"

chunkManager::chunkManager() {

}

chunkManager::~chunkManager() {

}

void chunkManager::createChunk(int x, int z, int seed) {

	chunks[32 * x + z] = new chunk(x, z, seed);

	giveNeighbours();
}

bool chunkManager::checkChunk(int x, int z) {

	if (chunks.count(32 * x + z) == 0) {
		return false;
	}

	return true;
}

void chunkManager::removeChunks(int x, int z) {

	std::unordered_map<int, chunk*>::iterator it;

	for (it = chunks.begin(); it != chunks.end(); ++it) {
		glm::vec2 temp = it->second->getChunkPos();
		if ((abs(temp.x - x) > 200) || (abs(temp.y - z) > 200)) {
			it->second->cleanUp();
			delete it->second;
			it = chunks.erase(it);
		}
	}
}

void chunkManager::render(Shader *shader) {

	std::unordered_map<int, chunk*>::iterator it;

	for (it = chunks.begin(); it != chunks.end(); ++it) {
		if (it->second->getShouldRender()) {
			it->second->render(shader);
		}
	}
}

void chunkManager::giveNeighbours() {

	std::unordered_map<int, chunk*>::iterator it;

	for (it = chunks.begin(); it != chunks.end(); ++it) {
		if (!it->second->getHasNeigbours()) {
			if (chunks.count(32 * (it->second->getChunkPos().x - 32) + it->second->getChunkPos().y) != 0) {
				if (chunks.count(32 * (it->second->getChunkPos().x + 32) + it->second->getChunkPos().y) != 0) {
					if (chunks.count(32 * it->second->getChunkPos().x + (it->second->getChunkPos().y - 32)) != 0) {
						if (chunks.count(32 * it->second->getChunkPos().x + (it->second->getChunkPos().y + 32)) != 0) {
							chunk* tempChunks[4];
							tempChunks[0] = chunks.at(32 * (it->second->getChunkPos().x - 32) + it->second->getChunkPos().y);
							tempChunks[1] = chunks.at(32 * (it->second->getChunkPos().x + 32) + it->second->getChunkPos().y);
							tempChunks[2] = chunks.at(32 * it->second->getChunkPos().x + (it->second->getChunkPos().y - 32));
							tempChunks[3] = chunks.at(32 * it->second->getChunkPos().x + (it->second->getChunkPos().y + 32));
							it->second->setNeighbourChunks(tempChunks);
						}
					}
				}
			}
		}
	}
}

void chunkManager::cleanUp() {

	std::unordered_map<int, chunk*>::iterator it;

	for (it = chunks.begin(); it != chunks.end(); ++it) {
		it->second->cleanUp();
	}
}