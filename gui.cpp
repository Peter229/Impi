#include "gui.h"

GUI::GUI() {

	optionHeight = 0.6f;
	optionPadding = 0.1f;
	scale = 0.4;
}

GUI::~GUI() {

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	VAO = 0;
	VBO = 0;
}

void GUI::addWindow(std::string window, glm::vec2 pos) {

	std::vector<Option*> options;

	windows[window] = new Window{ pos, 1, options, false };
}

void GUI::addOption(std::string window, std::string txt, int type) {

	Option *option = new Option{ txt, type };

	windows[window]->options.push_back(option);

	genVerts();
}

void GUI::cleanUp() {

	verts.clear();
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	VAO = 0;
	VBO = 0;
}

void GUI::render(Shader *shader) {

	glBindVertexArray(VAO);
	glm::mat4 model = glm::mat4(1.0f);
	shader->setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, verts.size());
}

void GUI::update(glm::vec2 mousePos, bool lPressed) {

	//std::cout << mousePos.x << " " << mousePos.y << "\n";

	for (auto i : windows) {

		//std::cout << i.second->pos.y << "\n";

		if (mousePos.x > i.second->pos.x && mousePos.y > (i.second->pos.y * -1) && mousePos.x < i.second->pos.x + i.second->width * scale && mousePos.y < (i.second->pos.y - ((i.second->options.size() * optionHeight) * scale)) * -1) {

			if (lPressed) {

				i.second->pos.x = mousePos.x - 0.1;
				i.second->pos.y = -mousePos.y + 0.1;
				genVerts();

			}
		}
		//std::cout << i.second.active << "bruh\n";
	}

}

void GUI::genVerts() {

	cleanUp();

	glm::vec3 col = { 0.2f, 0.2f, 0.2f };
	glm::vec3 col2 = { 0.5f, 0.5f, 0.5f };

	for (auto i : windows) {

		float amountOfOptions = i.second->options.size();

		amountOfOptions *= optionHeight;

		Vert2DC verta;
		Vert2DC vertb;
		Vert2DC vertc;
		Vert2DC vertd;

		for (int j = 1; j <= i.second->options.size(); j++) {

			verta = { glm::vec2(i.second->pos.x + (optionPadding * scale), i.second->pos.y - ((j - 1) * optionHeight * scale) - (optionPadding * scale)), col2 };
			vertb = { glm::vec2(i.second->pos.x + (i.second->width * scale) - (optionPadding * scale), i.second->pos.y - ((j - 1) * optionHeight * scale) - (optionPadding * scale)), col2 };
			vertc = { glm::vec2(i.second->pos.x + (i.second->width * scale) - (optionPadding * scale), i.second->pos.y - (j * optionHeight * scale) + (optionPadding * scale)), col2 };
			vertd = { glm::vec2(i.second->pos.x + (optionPadding * scale), i.second->pos.y - (j * optionHeight * scale) + (optionPadding * scale)), col2 };

			verts.push_back(verta);
			verts.push_back(vertc);
			verts.push_back(vertb);

			verts.push_back(verta);
			verts.push_back(vertd);
			verts.push_back(vertc);
		}

		verta = { i.second->pos, col };
		vertb = { glm::vec2(i.second->pos.x + i.second->width * scale,  i.second->pos.y), col };
		vertc = { glm::vec2(i.second->pos.x + i.second->width * scale,  i.second->pos.y - amountOfOptions * scale), col };
		vertd = { glm::vec2(i.second->pos.x,  i.second->pos.y - amountOfOptions * scale), col };

		verts.push_back(verta);
		verts.push_back(vertc);
		verts.push_back(vertb);

		verts.push_back(verta);
		verts.push_back(vertd);
		verts.push_back(vertc);
	}

	genVAO();
}

void GUI::genVAO() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vert2DC) * verts.size(), &verts[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}