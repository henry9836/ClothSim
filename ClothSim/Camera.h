#pragma once
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <freeglut.h>
#include <iostream>
#include "ScreenInfo.h"

class Camera
{
public:
	enum MODE {
		ORBIT,
		FOLLOW,
		FOLLOW_ORBIT,
		FOLLOW_STATIC,
		ORTH,
		PRESPECTIVE,
		CONTROL,
		PRESET1
	};
	void initializeCamera();
	void Tick(ScreenInfo m_Screen, float deltaTime);
	void SwitchMode(MODE _mode, glm::vec3 _target, glm::vec3 _camPos, glm::vec3 _lookDirFromFollow, GLfloat _radius, GLfloat _height);

	glm::mat4 getMVP(glm::vec3 postion, glm::vec3 scale, glm::mat4 rotationZ);

	glm::vec4 camStartPos = glm::vec4(-5, 3, 0, 1);
	glm::vec3 camPos;
	glm::vec3 camLookDir;
	glm::vec3 camUpDir;

	glm::mat4 rotMat;
	glm::vec4 newPos;

	glm::mat4 view;
	glm::vec3 camTar;
	glm::vec3 camFollowTar;
	glm::mat4 proj;

	float halfw = 0.0f;
	float halfh = 0.0f;
	float maxRenderDistance = 10000.0f;
	float minRenderDistance = 0.01f;
	float FOV = 90.0f;
	float timeElapsed = 0.0f;

	GLfloat radius = 3.0f;
	GLfloat height = 3.0f;
	glm::vec3 lookDirFromFollow = glm::vec3(0.0f, 0.0f, 0.0f);

	bool staticCam = true;
	bool orbitCam = false;
	bool followCam = true;
	bool orthoMode = false;
	bool controllable = false;

	int activeMode = -1;

};
