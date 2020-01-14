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

	firstMouse = true;

	this->windowWidth = SCR_WIDTH;
	this->windowHeight = SCR_HEIGHT;

	endMouse = startMouse = glm::vec3(0.0f, 0.0f, 0.0f);

	//Var init
	lClick = lShift = false;
	sIntersectionPoint = eIntersectionPoint = glm::vec3(0.0f, 0.0f, 0.0f);

	//Load Shaders
	terrainShader = new Shader("Shaders/Vertex/vertexShader.txt", "Shaders/Fragment/fragmentShader.txt");
	hudShader = new Shader("Shaders/Vertex/hudVertexShader.txt", "Shaders/Fragment/hudFragmentShader.txt");
	brushShader = new Shader("Shaders/Vertex/vBrushShader.txt", "Shaders/Fragment/fBrushShader.txt");
	guiShader = new Shader("Shaders/Vertex/vGUIShader.txt", "Shaders/Fragment/fGUIShader.txt");
	glTFShader = new Shader("Shaders/Vertex/vGLTFShader.txt", "Shaders/Fragment/fGLTFShader.txt");

	//Load Textures
	Resource_Manager::loadTexture("Textures/font.png", "font");
	Resource_Manager::loadTexture("Textures/ground.png", "ground");
	Resource_Manager::loadTexture("Textures/brushTex.png", "brushTex");
	Resource_Manager::loadTexture("Textures/grass.png", "select");

	//Create Hud
	huddy = new hud(glm::mat4(1.0f), windowWidth, windowHeight);

	huddy->drawCrosshair();
	huddy->drawString("fps", "FPS ", -38.0f, -8.5f);

	//Create Camera
	//camera = new Camera(glm::vec3(1000.0f, 30.0f, 1000.0f));
	camera = new Camera(glm::vec3(0.0f, 1.5f, 0.0f));

	//Create Frustum
	frust = { camera->Front,
		camera->Front * glm::vec3(-1.0f),
		camera->Up * glm::vec3(-1.0f),
		camera->Up,
		camera->Right * glm::vec3(-1.0f),
		camera->Right,
		camera->Position - (camera->Front * glm::vec3(32)) - (camera->Up * glm::vec3(100)) - (camera->Right * glm::vec3(150)),
		camera->Position + (camera->Front * glm::vec3(400)) + (camera->Up * glm::vec3(100)) + (camera->Right * glm::vec3(150)) };

	brushManager = new BrushManager();

	brushManager->addBrush(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 1.0f, 10.0f));
	brushManager->addBrush(glm::vec3(0.0f, 1.0f, 10.0f), glm::vec3(10.0f, 10.0f, 11.0f));

	gui = new GUI();

	//gui->addWindow("test", glm::vec2(-0.8f, 0.8f));
	//gui->addOption("test", "Wow", 0);
	//gui->addOption("test", "cock", 0);

	Resource_Manager::loadModel("cube.glb", "testModel");

	player = new Player(glm::vec3(3.0f, 3.0f, 3.0f));

	//cManager = new chunkManager();
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

			tick();
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

	if (glfwGetInputMode(WindowGL::getWindow(), GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {

		gui->update(glm::vec2(((xpos - (windowWidth / 2)) / windowWidth) * 2, ((ypos - (windowHeight / 2)) / windowHeight) * 2), Keys[GLFW_MOUSE_BUTTON_LEFT] == GLFW_PRESS);
		firstMouse = true;
	}
	else {

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
	}
	
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

	float speed = 2.0f;
	if (Keys[GLFW_KEY_UP] == GLFW_PRESS) {
		player->addVel(glm::vec3(0.0f, 0.0f, speed * deltaTime));
	}
	if (Keys[GLFW_KEY_DOWN] == GLFW_PRESS) {
		player->addVel(glm::vec3(0.0f, 0.0f, -speed * deltaTime));
	}
	if (Keys[GLFW_KEY_LEFT] == GLFW_PRESS) {
		player->addVel(glm::vec3(speed * deltaTime, 0.0f, 0.0f));
	}
	if (Keys[GLFW_KEY_RIGHT] == GLFW_PRESS) {
		player->addVel(glm::vec3(-speed * deltaTime, 0.0f, 0.0f));
	}
	if (Keys[GLFW_KEY_E] == GLFW_PRESS) {
		player->addVel(glm::vec3(0.0f, 5.0f * deltaTime, 0.0f));
	}
	if (Keys[GLFW_KEY_Q] == GLFW_PRESS) {
		player->addVel(glm::vec3(0.0f, -speed * deltaTime, 0.0f));
	}

	//LEVEL CREATOR
	if (Keys[GLFW_KEY_LEFT_SHIFT] == GLFW_RELEASE) {

		lShift = false;
		//lClick = false;
	}

	if (Keys[GLFW_MOUSE_BUTTON_LEFT] == GLFW_PRESS && !lClick && Keys[GLFW_KEY_LEFT_SHIFT] == GLFW_PRESS) {

		sIntersectionPoint = camera->Position + camera->Front * Collision::rayPlaneIntersection(camera->Front, camera->Position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lClick = true;
		lShift = true;
	}
	else if (Keys[GLFW_MOUSE_BUTTON_LEFT] == GLFW_PRESS && !lClick) {

		brushManager->selectBrush(camera->Position, camera->Front);
		lClick = true;
	}
	if (Keys[GLFW_MOUSE_BUTTON_LEFT] == GLFW_RELEASE && lClick && lShift == true) {

		eIntersectionPoint = camera->Position + camera->Front * Collision::rayPlaneIntersection(camera->Front, camera->Position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		if (round(sIntersectionPoint.x) != round(eIntersectionPoint.x) && round(sIntersectionPoint.z) != round(eIntersectionPoint.z)) {

			brushManager->addBrush(glm::vec3(std::min(round(sIntersectionPoint.x), round(eIntersectionPoint.x)), std::min(round(sIntersectionPoint.y), round(eIntersectionPoint.y)), std::min(round(sIntersectionPoint.z), round(eIntersectionPoint.z))), glm::vec3(std::max(round(sIntersectionPoint.x), round(eIntersectionPoint.x)), std::max(round(sIntersectionPoint.y) + 1.0f, round(eIntersectionPoint.y) + 1.0f), std::max(round(sIntersectionPoint.z), round(eIntersectionPoint.z))));
		}
		lShift = false;
		lClick = false;
	}
	else if (Keys[GLFW_MOUSE_BUTTON_LEFT] == GLFW_RELEASE && lClick) {

		lClick = false;
	}
	if (Keys[GLFW_KEY_DELETE] == GLFW_PRESS) {
		brushManager->deleteBrush();
	}
	if (Keys[GLFW_KEY_1] == GLFW_PRESS) {
		brushManager->changeBrushTexture(0);
	}
	if (Keys[GLFW_KEY_2] == GLFW_PRESS) {
		brushManager->changeBrushTexture(1);
	}
	if (Keys[GLFW_KEY_3] == GLFW_PRESS) {
		brushManager->changeBrushTexture(2);
	}
	if (Keys[GLFW_KEY_4] == GLFW_PRESS) {
		brushManager->changeBrushTexture(3);
	}


	//cubeVerts2.clear();
	//for (int i = 0; i < Resource_Manager::getModel("testModel")->getPosBuffer().size(); i+=3) {

		//cubeVerts2.push_back(glm::vec3(Resource_Manager::getModel("testModel")->getPosBuffer()[i], Resource_Manager::getModel("testModel")->getPosBuffer()[i + 1], Resource_Manager::getModel("testModel")->getPosBuffer()[i + 2]) + player->getPos());
	//}

	int cX = camera->Position.x / 32;
	int cZ = camera->Position.z / 32;

	/*int loadDistance = 5;

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
		
	cManager->removeChunks((int)camera->Position.x, (int)camera->Position.z);*/
}

void Game::logic() {

}

void Game::tick() {

	std::vector<glm::vec3> triVerts;
	triVerts.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	triVerts.push_back(glm::vec3(0.0f, 1.0f, 10.0f));
	triVerts.push_back(glm::vec3(10.0f, 1.0f, 10.0f));

	//if (Collision::gjkCollision(cubeVerts2, triVerts, player->getPos() - glm::vec3(5.0f, 1.0f, 5.0f))) {

		//std::cout << "COLL\n";
	//}

	float t = 0.0f;

	glm::vec3 move = glm::vec3(player->getPos());

	Collision::movePlayerAgainstWorld(&move, glm::vec3(0.5f, 0.5f, 0.5f), &(player->getVel()), brushManager->getColHull());

	//Collision::toiAABBTri(&move, glm::vec3(1.0f, 1.0f, 1.0f), player->getVel(), triVerts[0], triVerts[1], triVerts[2]);

	//Collision::pointExpandTri(&move, glm::vec3(0.5f, 0.5f, 0.5f), player->getVel(), triVerts[0], triVerts[1], triVerts[2]);


	//player->updatePos(move);

	//Gravity Pass
	//Collision::toiAABBTri(&move, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, -0.05f, 0.0f), triVerts[0], triVerts[1], triVerts[2]);

	player->updatePos(move);

	//if (Collision::satCollisionOOBTri(move, glm::vec3(0.5f, 0.5f, 0.5f), triVerts[0], triVerts[1], triVerts[2])) {

		//std::cout << "COLL\n";
	//}

	//std::cout << move.x << " " << move.y << " " << move.z << "\n";

	//if (Collision::satCollisionOOBTriStatic(&move, glm::vec3(0.5f, 0.5f, 0.5f), triVerts[0], triVerts[1], triVerts[2])) {

		//std::cout << "Y " << move.y << "\n";
		//player->updatePos(move);
		//std::cout << move.x << " " << move.y << " " << move.z << "\n";
		//std::cout << "COLL\n";
	//}
	//Collision::satCollisionOOBTriStatic(&move, glm::vec3(0.5f, 0.5f, 0.5f), triVerts[0], triVerts[1], triVerts[2]);
	//player->updatePos(move);
	//player->update();
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

	//cManager->render(terrainShader);

	brushShader->use();
	brushShader->setMat4("projection", projection);
	brushShader->setMat4("view", view);
	//brushShader->setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, Resource_Manager::getTexture("brushTex"));
	brushManager->render(brushShader);

	model = glm::mat4(1.0f);

	//guiShader->use();
	//gui->render(guiShader);

	glTFShader->use();
	glTFShader->setMat4("projection", projection);
	glTFShader->setMat4("view", view);
	model = glm::translate(model, player->getPos());
	model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.5f));
	glTFShader->setMat4("model", model);
	glBindTexture(GL_TEXTURE_2D, Resource_Manager::getTexture("ground"));
	Resource_Manager::getModel("testModel")->render(glTFShader);

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

	gui->cleanUp();

	Resource_Manager::clearData();

	brushManager->cleanUp();

	//cManager->cleanUp();

	huddy->cleanUp();

	WindowGL::end();
}