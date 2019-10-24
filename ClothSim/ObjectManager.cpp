#include "ObjectManager.h"

Input m_input;


ObjectManager::ObjectManager()
{

}

ObjectManager::~ObjectManager()
{

}

void ObjectManager::Reset()
{
	objPos = glm::vec3(0.0f, 0.0f, 0.0f);
}

MovementPacket ObjectManager::Move(OBJECTTYPE objType, float speed, AudioSystem& audio, GLfloat deltaTime, glm::vec4 maxWorldSize, glm::vec3 currentPos, float currentRotation, glm::vec3 Target) {
	
	MovementPacket currentState;

	

	currentState.newPosition = currentPos;
	currentState.newRotation = currentRotation;
	currentState.fire = false;

	if (objType == PLAYER) {
		//Console_OutputLog(L"Current Pos: X[" + std::to_string(currentPos.x) + "] Y[" + std::to_string(currentPos.y) + "]" + "] Z[" + std::to_string(currentPos.z) + "]", LOGINFO);
		//Movement
		//if (m_input.CheckKeyDown('w'))
		//{
		//	currentState.newRotation = 270.0;
		//	if (currentState.newPosition.x < maxWorldSize.z) {
		//		currentState.newPosition.x += speed * deltaTime;
		//	}
		//}
		//if (m_input.CheckKeyDown('s'))
		//{
		//	currentState.newRotation = 90.0f;
		//	if (currentState.newPosition.x > maxWorldSize.y) {
		//		currentState.newPosition.x -= speed * deltaTime;
		//	}
		//}
		//if (m_input.CheckKeyDown('a'))
		//{
		//	currentState.newRotation = 0.0f;
		//	if (currentState.newPosition.z > maxWorldSize.x) {
		//		currentState.newPosition.z -= speed * deltaTime;
		//	}
		//}
		//if (m_input.CheckKeyDown('d'))
		//{
		//	currentState.newRotation = 180.0f;
		//	if (currentState.newPosition.z < maxWorldSize.w) {
		//		currentState.newPosition.z += speed * deltaTime;
		//	}
		//}
		//if (m_input.CheckKeyDown('w') && m_input.CheckKeyDown('a')) {
		//	currentState.newRotation = 315.0f;
		//}
		//else if (m_input.CheckKeyDown('w') && m_input.CheckKeyDown('d')) {
		//	currentState.newRotation = 225.0f;
		//}
		//if (m_input.CheckKeyDown('s') && m_input.CheckKeyDown('d')) {
		//	currentState.newRotation = 135.0f;
		//}
		//else if (m_input.CheckKeyDown('s') && m_input.CheckKeyDown('a')) {
		//	currentState.newRotation = 45.0f;
		//}

		////Basic rotation
		///*if (m_input.CheckKeyDown('q'))
		//{
		//	currentState.newRotation += (speed * 30) * deltaTime;
		//}
		//if (m_input.CheckKeyDown('e'))
		//{
		//	currentState.newRotation -= (speed * 30) * deltaTime;
		//}*/
		//
		////Shooting
		//currentreload += deltaTime;

		//if (m_input.CheckKeyDown('f') && currentreload > reloadTime) {
		//	currentreload = 0.0f;
		//	audio.Play(audio.SHOOT);
		//	currentState.fire = true;
		//}
	}

	else if (objType == BULLET) {

		currentRotation += 180.0f;

		if (currentRotation == 270.0) // w
		{
			currentState.newPosition.x += speed * deltaTime;
		}
		if (currentRotation == 90.0f) //s
		{
			currentState.newPosition.x -= speed * deltaTime;
		}
		if (currentRotation == 0.0f) //a
		{
			currentState.newPosition.z -= speed * deltaTime;
		}
		if (currentRotation == 180.0f) //d
		{
			currentState.newPosition.z += speed * deltaTime;
		}
		if (currentRotation == 315.0f) { //w a
			currentState.newPosition.x += speed * deltaTime;
			currentState.newPosition.z -= speed * deltaTime;
		}
		if (currentRotation == 225.0f) { //w d
			currentState.newPosition.x += speed * deltaTime;
			currentState.newPosition.z += speed * deltaTime;
		}
		if (currentRotation == 135.0f) { //s d
			currentState.newPosition.x -= speed * deltaTime;
			currentState.newPosition.z += speed * deltaTime;
		}
		if (currentRotation == 45.0f) { //s a
			currentState.newPosition.x -= speed * deltaTime;
			currentState.newPosition.z -= speed * deltaTime;
		}
	}

	else if (objType == ENEMY) {
		currentState.newPosition = currentPos;

		if (currentPos.x > Target.x) {
			currentState.newPosition.x -= speed * deltaTime;
		}
		else if (currentPos.x < Target.x) {
			currentState.newPosition.x += speed * deltaTime;
		}

		if (currentPos.z > Target.z) {
			currentState.newPosition.z -= speed * deltaTime;
		}
		else if (currentPos.z < Target.z) {
			currentState.newPosition.z += speed * deltaTime;
		}

		currentState.newRotation += 3.0f;
	}
	return currentState;
}

void ObjectManager::movement(AudioSystem& audio2, GLfloat deltaTime, float screenW, float screenH, bool isEnemy)
{
	glm::vec3 temp;
	temp = objPos;
	if (!isEnemy) {
		if (m_input.CheckKeyDown('w'))
		{

			if (temp.y < ((screenH - 100) / 2))
			{
				temp.y += speedBaby * deltaTime;
				objPos = temp;
			}

		}
		if (m_input.CheckKeyDown('a'))
		{
			if (temp.x > (0 - ((screenW + 240) / 2)))
			{
				temp.x -= speedBaby * deltaTime;
				objPos = temp;
			}
		}
		if (m_input.CheckKeyDown('s'))
		{
			if (temp.y > (0 - ((screenH - 100) / 2)))
			{
				temp.y -= speedBaby * deltaTime;
				objPos = temp;
			}
		}
		if (m_input.CheckKeyDown('d'))
		{
			if (temp.x < ((screenW - 450) / 2))
			{
				temp.x += speedBaby * deltaTime;
				objPos = temp;
			}
		}
	}
	else {
		if (objPos.x > (Target.x + 151)) {
			objPos.x -= speedFire * deltaTime;
			//go left
		}
		else if (objPos.x < (Target.x + 149)) {
			objPos.x += speedFire * deltaTime;
			//go right
		}
		if (objPos.y > Target.y) {
			objPos.y -= speedFire * deltaTime;
			//go down
		}
		else if (objPos.y < Target.y) {
			objPos.y += speedFire * deltaTime;
			//go up
		}
	}
}



