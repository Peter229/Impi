#include "game.h"

Game::Game() {

}
 
Game::~Game() {

}

void Game::start() {

	if (WindowGL::start() == -1) {
		std::cout << "Failed to create window\n";
	}

	deltaTime = 0.0f;
	lastFrame = 0.0f;

	deltaTimeT = 0;
	deltaTime2 = 0;
	lastTimeA = glfwGetTime();
	lastTimeB = glfwGetTime();
	nbFrames = 0;

	firstMouse = false;

	this->windowWidth = SCR_WIDTH;
	this->windowHeight = SCR_HEIGHT;

	endMouse = startMouse = glm::vec3(0.0f, 0.0f, 0.0f);

	//Load Shaders
	terrainShader = new Shader("Shaders/Vertex/vertexShader.txt", "Shaders/Fragment/fragmentShader.txt");
	hudShader = new Shader("Shaders/Vertex/hudVertexShader.txt", "Shaders/Fragment/hudFragmentShader.txt");

	//Load Textures
	Resource_Manager::loadTexture("Textures/font.png", "font");

	//Create Hud
	huddy = new hud(glm::mat4(1.0f), windowWidth, windowHeight);

	huddy->drawCrosshair();
	huddy->drawString("fps", "FPS ", -38.0f, -8.5f);

	//Create Camera
	camera = new Camera(glm::vec3(1000.0f, 30.0f, 1000.0f));
	//camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));

	//Create Frustum
	frust = { camera->Front,
		camera->Front * glm::vec3(-1.0f),
		camera->Up * glm::vec3(-1.0f),
		camera->Up,
		camera->Right * glm::vec3(-1.0f),
		camera->Right,
		camera->Position - (camera->Front * glm::vec3(32)) - (camera->Up * glm::vec3(100)) - (camera->Right * glm::vec3(150)),
		camera->Position + (camera->Front * glm::vec3(400)) + (camera->Up * glm::vec3(100)) + (camera->Right * glm::vec3(150)) };

	cManager = new chunkManager();
}

void Game::loop() {

	while (!glfwWindowShouldClose(WindowGL::getWindow())) {

		double currentTimeA = glfwGetTime();
		nbFrames++;
		if (currentTimeA - lastTimeA >= 1.0) {
			printf("%f ms/frame\n%d Frames Per Second\n", 1000.0 / double(nbFrames), nbFrames);
			huddy->removeHudPart("fps");
			huddy->drawString("fps", "FPS " + std::to_string(nbFrames) , -38.0f, -8.5f);
			nbFrames = 0;
			lastTimeA += 1.0;
		}

		//Delta Time To Get How Fast The Game Should Run
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Get Inputs
		WindowGL::update();

		update(WindowGL::getKeys(), WindowGL::getMousePos(), deltaTime);

		if (currentTimeA - lastTimeB >= 0.0166) { //Call physics 60 times a second

			lastTimeB += 0.0166;

			//tick();
		}

		render();

		//Go To Next Frame
		glfwSwapBuffers(WindowGL::getWindow());
	}
}

void Game::update(GLboolean* Keys, double* mousePos, float deltaTime) {

	//Mouse Movement
	double xpos = mousePos[0];
	double ypos = mousePos[1];
	
	if (firstMouse) {

		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera->ProcessMouseMovement(xoffset, yoffset);
	
	if (Keys[GLFW_KEY_W] == GLFW_PRESS) {
		camera->ProcessKeyboard(FORWARD, deltaTime);
	}
	if (Keys[GLFW_KEY_S] == GLFW_PRESS) {
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (Keys[GLFW_KEY_A] == GLFW_PRESS) {
		camera->ProcessKeyboard(LEFT, deltaTime);
	}
	if (Keys[GLFW_KEY_D] == GLFW_PRESS) {
		camera->ProcessKeyboard(RIGHT, deltaTime);
	}

	int cX = camera->Position.x / 32;
	int cZ = camera->Position.z / 32;

	int loadDistance = 5;

	bool chunkLoaded = false;

	for (int i = 0; i < loadDistance; i++) {

		int minX = std::max(cX - i, 0);
		int minZ = std::max(cZ - i, 0);
		int maxX = cX + i;
		int maxZ = cZ + i;

		for (int x = minX; x < maxX; ++x) {

			for (int z = minZ; z < maxZ; ++z) {

				if (!cManager->checkChunk(x * 32, z * 32) && !chunkLoaded) {
					cManager->createChunk(x * 32, z * 32, 0);
					chunkLoaded = true;
				}
			}

			if (chunkLoaded) {
				break;
			}
		}
	}
		
	cManager->removeChunks((int)camera->Position.x, (int)camera->Position.z);
}

void Game::logic() {

}

void Game::tick() {

}

void Game::render() {

	frust = { camera->Front,
		camera->Front * glm::vec3(-1.0f),
		camera->Up * glm::vec3(-1.0f),
		camera->Up,
		camera->Right * glm::vec3(-1.0f),
		camera->Right,
		camera->Position - (camera->Front * glm::vec3(32)) - (camera->Up * glm::vec3(100)) - (camera->Right * glm::vec3(200)),
		camera->Position + (camera->Front * glm::vec3(400)) + (camera->Up * glm::vec3(100)) + (camera->Right * glm::vec3(200)) };

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.9f, 1.0f);

	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 400.0f);
	glm::mat4 view = camera->GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	//Render With Basic Shader
	terrainShader->use();
	//Set Projection Matrix
	terrainShader->setMat4("projection", projection);
	//Set View Matrix
	terrainShader->setMat4("view", view);

	cManager->render(terrainShader);

	//Render Hud
	hudShader->use();
	if (camera->Position.x < 0 || camera->Position.z < 0) {
		huddy->drawString("pos", "Neg", -38.0f, -10.5f);
	}
	else {
		huddy->drawString("pos", "X " + std::to_string((int)camera->Position.x) + " Y " + std::to_string((int)camera->Position.y) + " Z " + std::to_string((int)camera->Position.z), -38.0f, -10.5f);
	}
	huddy->render(hudShader);
	huddy->removeHudPart("pos");
}

void Game::cleanUp() {

	Resource_Manager::clearData();

	cManager->cleanUp();

	huddy->cleanUp();

	WindowGL::end();
}