#include "Camera.h"
#include "ConsoleController.h"
#include "glm.hpp"
#include <iostream>
#include <freeglut.h>

void Camera::initializeCamera()
{
	Console_OutputLog(L"Initialising Camera...", LOGINFO);
	camPos = glm::vec3(0.0f, 0.0f, 0.0f);
	camLookDir = glm::vec3(0.0f, 0.0f, -1.0f);
	camUpDir = glm::vec3(0.0f, 1.0f, 0.0f);
	Console_OutputLog(L"Initialised Camera", LOGINFO);
}

void Camera::Tick(ScreenInfo m_Screen, float deltaTime)
{
	if (orbitCam) {
		timeElapsed += deltaTime;
		camPos.x = sin(timeElapsed) * radius;
		camPos.z = cos(timeElapsed) * radius;
		//camTar = glm::vec3(0.0f, 0.0f, 0.0f);
		if (followCam) {
			camTar = glm::vec3(camFollowTar + lookDirFromFollow);
			if (!staticCam) {
				camPos.x = (sin(timeElapsed) * radius) + camFollowTar.x;
				camPos.y = camFollowTar.y + height;
				camPos.z = (cos(timeElapsed) * radius) + camFollowTar.z;
			}
		}
		view = glm::lookAt(camPos, camTar, camUpDir);
		if (orthoMode) {
			halfw = (float)m_Screen.SCR_WIDTH * 0.5f;
			halfh = (float)m_Screen.SCR_HEIGHT * 0.5f;
			proj = glm::ortho(-halfw, halfw, -halfh, halfh, minRenderDistance, maxRenderDistance);
		}
		else {
			proj = glm::perspective(FOV / 2, (float)m_Screen.SCR_WIDTH / (float)m_Screen.SCR_HEIGHT, minRenderDistance, maxRenderDistance);
		}
	}
	else {
		if (!controllable) {
			camPos = glm::vec3(camStartPos);
			camTar = glm::vec3(0.0f, 0.0f, 0.0f);
			if (followCam) {
				camTar = glm::vec3(camFollowTar + lookDirFromFollow);
				if (!staticCam) {
					camPos.x = camFollowTar.x;
					camPos.y = camFollowTar.y + height;
					camPos.z = camFollowTar.z;
				}
			}
		}
		view = glm::lookAt(camPos, camTar, camUpDir);
		if (orthoMode) {
			halfw = (float)m_Screen.SCR_WIDTH * 0.5f;
			halfh = (float)m_Screen.SCR_HEIGHT * 0.5f;
			proj = glm::ortho(-halfw, halfw, -halfh, halfh, minRenderDistance, maxRenderDistance);
		}
		else {
			proj = glm::perspective(FOV / 2, (float)m_Screen.SCR_WIDTH / (float)m_Screen.SCR_HEIGHT, minRenderDistance, maxRenderDistance);
		}
	}

}

void Camera::SwitchMode(MODE _mode, glm::vec3 _target, glm::vec3 _camPos, glm::vec3 _lookDirFromFollow, GLfloat _radius, GLfloat _height)
{

	switch (_mode)
	{
	case Camera::ORBIT:
	{
		if (_mode != activeMode) {
			Console_OutputLog(L"Switching Camera Mode To Orbit", LOGINFO);
		}
		this->staticCam = true;
		this->orbitCam = true;
		this->followCam = false;
		this->controllable = false;
		this->radius = _radius;
		this->height = _height;
		this->camPos = _camPos;
		this->camTar = _target;
		this->camFollowTar = _target;
		this->lookDirFromFollow = _lookDirFromFollow;
		this->activeMode = _mode;
		break;
	}
	case Camera::FOLLOW:
	{
		if (_mode != activeMode) {
			Console_OutputLog(L"Switching Camera Mode To Follow", LOGINFO);
		}
		this->staticCam = false;
		this->orbitCam = false;
		this->controllable = false;
		this->followCam = true;
		this->camPos = _camPos;
		this->camTar = _target;
		this->lookDirFromFollow = _lookDirFromFollow;
		this->camFollowTar = _target;
		this->radius = _radius;
		this->height = _height;
		this->activeMode = _mode;
		break;
	}
	case Camera::FOLLOW_ORBIT:
	{
		if (_mode != activeMode) {
			Console_OutputLog(L"Switching Camera Mode To Follow", LOGINFO);
		}
		this->staticCam = false;
		this->orbitCam = true;
		this->followCam = true;
		this->camPos = _camPos;
		this->camTar = _target;
		this->lookDirFromFollow = _lookDirFromFollow;
		this->camFollowTar = _target;
		this->radius = _radius;
		this->height = _height;
		this->activeMode = _mode;
		this->controllable = false;
		break;
	}
	case Camera::FOLLOW_STATIC:
	{
		if (_mode != activeMode) {
			Console_OutputLog(L"Switching Camera Mode To Follow", LOGINFO);
		}
		this->staticCam = true;
		this->orbitCam = false;
		this->controllable = false;
		this->followCam = true;
		this->camPos = _camPos;
		this->camTar = _target;
		this->camFollowTar = _target;
		this->lookDirFromFollow = _lookDirFromFollow;
		this->radius = _radius;
		this->height = _height;
		this->activeMode = _mode;
		break;
	}

	case Camera::CONTROL: {
		if (_mode != activeMode) {
			Console_OutputLog(L"Switching Camera Mode To Control", LOGINFO);
		}
		this->staticCam = false;
		this->orbitCam = false;
		this->controllable = true;
		this->followCam = false;
		this->camPos = _camPos;
		this->camTar = _target;
		this->camFollowTar = _target;
		this->lookDirFromFollow = _lookDirFromFollow;
		this->radius = _radius;
		this->height = _height;
		this->activeMode = _mode;
		break;
	}

	case Camera::ORTH:
	{
		if (_mode != activeMode) {
			Console_OutputLog(L"Switching Camera Mode To Orth", LOGINFO);
		}
		this->orthoMode = true;
		this->activeMode = _mode;
		break;
	}

	case Camera::PRESPECTIVE:
	{
		if (_mode != activeMode) {
			Console_OutputLog(L"Switching Camera Mode To Perspective", LOGINFO);
		}
		this->orthoMode = false;
		this->activeMode = _mode;
		break;
	}
	case Camera::PRESET1:
	{
		if (_mode != activeMode) {
			Console_OutputLog(L"Switching Camera Mode To Preset1", LOGINFO);
		}
		this->activeMode = _mode;
		break;
	}
	default: {
		Console_OutputLog(to_wstring("UNRECONGISED MODE:" + std::to_string(_mode)), LOGWARN);
		break;
		}
	}
}

glm::mat4 Camera::getMVP(glm::vec3 postion, glm::vec3 scale, glm::mat4 rotationZ)
{
	glm::vec3 backObjPosition = postion;
	glm::mat4 backTranslationMatrix = glm::translate(glm::mat4(), backObjPosition);
	glm::vec3 objscaleBack = scale;
	glm::mat4 scaleMatrixBack = glm::scale(glm::mat4(), objscaleBack);
	glm::mat4 backModel = backTranslationMatrix * rotationZ * scaleMatrixBack;
	glm::mat4 backProj_calc = proj * view * backModel;
	return (backProj_calc);
}



