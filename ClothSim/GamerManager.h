#pragma once

#include <vector>
#include <fstream>
#include "3D.h"
#include "AI.h"

class GameManager {
public:
	GameManager();

	void CheckGeneralInput(GameManager& m_game, Camera& camera, glm::vec3 orbittar, Model* player);



	enum Screens {
		MAIN,
		GAME,
		GAMEOVER
	};
	int currentScreen = MAIN;

	std::vector<Enemy*>* enemyList;
	std::vector<AIObject*>* aiList;

	glm::vec3 playerPos = glm::vec3(0,0,0);

	Camera* mCam;

	bool gameover = false;
	bool stencil = false;
	bool scissor = false;
	bool depth = false;
	bool leave = false;
	bool caminManual = false;
	bool wireFrame = false;
	
};

