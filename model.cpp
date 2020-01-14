#include "model.h"

void DeleteVertexBuffer(GLuint& vboID) {

	if (vboID != 0) {

		glDeleteBuffers(1, &vboID);
		vboID = 0;
	}
}

inline void CreateVertexBuffer(GLuint& vboID) {

	DeleteVertexBuffer(vboID);
	glGenBuffers(1, &vboID);
}

Model::Model() {

}

Model::Model(std::string path) {

	std::ifstream file(path, std::ios::in | std::ios::binary);

	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});

	file.close();

	uint32_t magic;
	uint32_t version;
	uint32_t length;

	unsigned int offset = 0;

	memcpy(&magic, &buffer[offset + 0], 4);
	memcpy(&version, &buffer[offset + 4], 4);
	memcpy(&length, &buffer[offset + 8], 4);

	uint32_t chunkLength;
	uint32_t chunkType;
	std::vector<unsigned char> chunkData;

	memcpy(&chunkLength, &buffer[offset + 12], 4);
	memcpy(&chunkType, &buffer[offset + 16], 4);
	for (int i = 0; i < chunkLength; i++) {
		chunkData.push_back(buffer[offset + 20 + i]);
	}

	uint32_t chunk2Length;
	uint32_t chunk2Type;
	std::vector<unsigned char> chunk2Data;

	memcpy(&chunk2Length, &buffer[offset + 20 + chunkLength], 4);
	memcpy(&chunk2Type, &buffer[offset + 24 + chunkLength], 4);
	for (int i = 0; i < chunk2Length; i++) {
		chunk2Data.push_back(buffer[offset + 28 + i + chunkLength]);
	}

	std::string json;

	for (int i = 0; i < chunkData.size(); i++) {

		json += chunkData[i];
	}

	unsigned int posBufferOffset;
	unsigned int posBufferBytes;
	unsigned int indBufferOffset;
	unsigned int indBufferBytes;
	unsigned int uvBufferOffset;
	unsigned int uvBufferBytes;
	unsigned int norBufferOffset;
	unsigned int norBufferBytes;
	unsigned int x;
	unsigned int y;

	unsigned int found = json.find("\"buffer\"");
	if (found != std::string::npos) {

		found = json.find("byteLength");
		x = json.find_first_of(":", found) + 1;
		y = json.find_first_of(",", found);

		if (found != std::string::npos) {
			posBufferBytes = std::stoi(json.substr(x, y - x));

		}

		found = json.find("byteOffset");
		x = json.find_first_of(":", found) + 1;
		y = json.find_first_of("}", found);

		if (found != std::string::npos) {

			posBufferOffset = std::stoi(json.substr(x, y - x));
		}
	}

	found = json.find("\"buffer\"", found);
	if (found != std::string::npos) {


		found = json.find("byteLength", found);
		x = json.find_first_of(":", found) + 1;
		y = json.find_first_of(",", found);

		if (found != std::string::npos) {

			norBufferBytes = std::stoi(json.substr(x, y - x));
		}

		found = json.find("byteOffset", found);
		x = json.find_first_of(":", found) + 1;
		y = json.find_first_of("}", found);

		if (found != std::string::npos) {

			norBufferOffset = std::stoi(json.substr(x, y - x));
		}
	}

	found = json.find("\"buffer\"", found);
	if (found != std::string::npos) {

		found = json.find("byteLength", found);
		x = json.find_first_of(":", found) + 1;
		y = json.find_first_of(",", found);

		if (found != std::string::npos) {

			uvBufferBytes = std::stoi(json.substr(x, y - x));
		}

		found = json.find("byteOffset", found);
		x = json.find_first_of(":", found) + 1;
		y = json.find_first_of("}", found);

		if (found != std::string::npos) {

			uvBufferOffset = std::stoi(json.substr(x, y - x));
		}
	}

	found = json.find("\"buffer\"", found);
	if (found != std::string::npos) {

		found = json.find("byteLength", found);
		x = json.find_first_of(":", found) + 1;
		y = json.find_first_of(",", found);

		if (found != std::string::npos) {

			indBufferBytes = std::stoi(json.substr(x, y - x));
		}

		found = json.find("byteOffset", found);
		x = json.find_first_of(":", found) + 1;
		y = json.find_first_of("}", found);

		if (found != std::string::npos) {

			indBufferOffset = std::stoi(json.substr(x, y - x));
		}
	}

	float vert;
	uint16_t indice;

	for (int i = posBufferOffset; i < posBufferOffset + posBufferBytes; i += 4) {
		memcpy(&vert, &chunk2Data[i], 4);
		PosBuffer.push_back(vert);
	}

	for (int i = norBufferOffset; i < norBufferOffset + norBufferBytes; i += 4) {
		memcpy(&vert, &chunk2Data[i], 4);
		NorBuffer.push_back(vert);
	}

	for (int i = uvBufferOffset; i < uvBufferOffset + uvBufferBytes; i += 4) {
		memcpy(&vert, &chunk2Data[i], 4);
		UvBuffer.push_back(vert);
	}

	for (int i = indBufferOffset; i < indBufferOffset + indBufferBytes; i += 2) {
		memcpy(&indice, &chunk2Data[i], 2);
		IndBuffer.push_back(indice);
	}

	genVAO();

	chunk2Data.clear();
	chunkData.clear();
	buffer.clear();
}

Model::~Model() {

}

std::vector<float> Model::getPosBuffer() {

	return PosBuffer;
}

void Model::render(Shader *shader) {

	//glm::mat4 model = glm::mat4(1.0f);

	glBindVertexArray(GLVAO);
	//shader->setMat4("model", model);
	glDrawElements(GL_TRIANGLES, IndBuffer.size(), GL_UNSIGNED_INT, 0);
}

void Model::cleanUp() {

	DeleteVertexBuffer(GLUvBuffer);
	DeleteVertexBuffer(GLNorBuffer);
	DeleteVertexBuffer(GLPosBuffer);
	DeleteVertexBuffer(GLIndBuffer);
	glDeleteVertexArrays(1, &GLVAO);
}

void Model::genVAO() {

	glGenVertexArrays(1, &GLVAO);
	glGenBuffers(1, &GLPosBuffer);
	glGenBuffers(1, &GLIndBuffer);
	glGenBuffers(1, &GLUvBuffer);
	glGenBuffers(1, &GLNorBuffer);
	//CreateVertexBuffer(GLPosBuffer);
	//CreateVertexBuffer(GLIndBuffer);

	glBindVertexArray(GLVAO);

	glBindBuffer(GL_ARRAY_BUFFER, GLPosBuffer);
	glBufferData(GL_ARRAY_BUFFER, PosBuffer.size() * sizeof(float), &PosBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, GLNorBuffer);
	glBufferData(GL_ARRAY_BUFFER, NorBuffer.size() * sizeof(float), &NorBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, GLUvBuffer);
	glBufferData(GL_ARRAY_BUFFER, UvBuffer.size() * sizeof(float), &UvBuffer[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLIndBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndBuffer.size() * sizeof(GLuint), &IndBuffer[0], GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}