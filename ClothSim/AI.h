#pragma once

#include "3D.h"
#include <cstdlib>
#include <iostream>
#include <ctime>

class AIObject {
public:

	//Different AIModes that can be used
	enum AIMODE {
		SEEK,
		FLEE,
		WANDER,
		PURSUIT,
		EVADE,
		LEADER
	};

	//Variables
	glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };
	Model* object;
	bool amAllowedAlive = true;
	float maxSpeed = 3.0f;
	float maxForce = 0.1f;
	int mMode = SEEK;

	//Deconstructer
	~AIObject();

	//Name:				CheckNAN
	//Parameters:		Pointer to a glm::vec3
	//Return Type:		NULL
	//Description:		This function checks if any value inside a vec3 is nan and if it is changes it to a 0.0f
	void CheckNAN(glm::vec3* vec) {
		if (isnan(vec->x))
			vec->x = 0.0f;
		if (isnan(vec->y))
			vec->y = 0.0f;
		if(isnan(vec->z))
			vec->z = 0.0f;
	}

	//Name:				Init
	//Parameters:		Pointer to a model
	//Return Type:		NULL
	//Description:		This function initalises values
	void Init(Model* _Object) {
		object = _Object;
		std::srand((unsigned int)std::time(NULL));
	};

	//Name:				Tick
	//Parameters:		float deltaTime, glm::vec4 worldSize
	//Return Type:		NULL
	//Description:		This function selects points for wander mode to travel to (only if the AI Object is in Wander mode) and it also
	//confines the AI within the worldSize
	void Tick(float deltaTime, glm::vec4 worldSize) {
		if (this->mMode == WANDER) { //if we are in wander mode
			this->time += deltaTime; //update our time
			//if we have expired the lifetime of the wander
			if (this->time >= this->wanderLifeTime) { //if we have execeeded the wanderLifeTime
				this->wanderLifeTime = ((float)rand() / RAND_MAX)* (allowedWanderLifeTimes.y - allowedWanderLifeTimes.x) + allowedWanderLifeTimes.x; //make a new random lifetime
				this->time = 0.0f; //reset our time
				this->wanderTarget = glm::vec3((rand() % 35) + 2, 1.0f, (rand() % 50) - 25); //set a new wander target
			}
		}
		else {
			//reset wander values
			wanderLifeTime = 0.0f;
			time = 0.0f;
		}

		//Confine AI Objects to the world size

		if (this->object->position.z > worldSize.z) {
			this->object->position.z = worldSize.z;
		}
		else if (this->object->position.z < worldSize.x) {
			this->object->position.z = worldSize.x;
		}
		if (this->object->position.x > worldSize.w) {
			this->object->position.x = worldSize.w;
		}
		else if (this->object->position.x < worldSize.y) {
			this->object->position.x = worldSize.y;
		}

		CheckNAN(&velocity); //make sure velocity does not contain any nan
	};

	//Name:				AIWander
	//Parameters:		float deltaTime, float maxForce, float arrrivalDistance
	//Return Type:		glm::vec3
	//Description:		This function wanders between points 
	glm::vec3 AIWander(float maxSpeed, float maxForce, float arrivalDistance) {
		//Console_OutputLog(L"Wandering", LOGINFO);
		//Go to wander target
		glm::vec3 wanderResult = Seek(this->object->position, this->velocity, this->wanderTarget, maxSpeed, maxForce, arrivalDistance);
		return wanderResult;
	}

	//Name:				Limit
	//Parameters:		glm::vec3 vec, float maxForce
	//Return Type:		glm::vec3
	//Description:		This function takes a vector and limits it if it beyond an acceptable range
	static glm::vec3 Limit(glm::vec3 vec, float maxForce) {
		if (vec == glm::vec3(0.0f, 0.0f, 0.0f)) { //if the vector is 0, 0, 0 return it
			return glm::vec3();
		}
		glm::vec3 retV = vec; //create a new vector that will return our result
		if (glm::length(retV) > maxForce) { //if the vector is exceeding the maxForce limit it by normalizing it
			retV = glm::normalize(retV) * maxForce;
		}
		return retV;
	}

	//Name:				Seek
	//Parameters:		glm::vec3 objPos, glm::vec3 objVelocity, glm::vec3 targetPos, float maxSpeed, float maxForce, float arrivalDistance
	//Return Type:		glm::vec3
	//Description:		This function takes an Objects's position, velocity and a target's position and calculates the required vector
	//to reach it
	static glm::vec3 Seek(glm::vec3 objPos, glm::vec3 objVelocity, glm::vec3 targetPos, float maxSpeed, float maxForce, float arrivalDistance) {

		//Calculated the desired velocity based on the object's position and the target position
		glm::vec3 desiredVelocity = targetPos - objPos;
		//Calculate the distance from the object to the target
		float distance = glm::length(desiredVelocity);
		//Normalize Desired Velocity
		desiredVelocity = glm::normalize(desiredVelocity) * maxSpeed;

		//If the distance to the target is less than the approach distance
		if (distance < arrivalDistance) {
			//Reduce the speed to get a smoother arrival
			desiredVelocity *= (distance / arrivalDistance);
		}

		//Caluclate Max Force
		glm::vec3 steering = desiredVelocity - objVelocity;
		steering = Limit(steering, maxForce);

		//Avoid Glitch Town
		steering.y = 0.0f;

		return (steering);
	};

	//Name:				Flee
	//Parameters:		glm::vec3 objPos, glm::vec3 objVelocity, glm::vec3 targetPos, float maxSpeed, float maxForce, float arrivalDistance
	//Return Type:		glm::vec3
	//Description:		This function takes an Objects's position, velocity and a target's position and calculates the required vector
	//to flee it
	static glm::vec3 Flee(glm::vec3 objPos, glm::vec3 objVelocity, glm::vec3 targetPos, float maxSpeed, float maxForce, float arrivalDistance) {
		//Console_OutputLog(L"Fleeing", LOGINFO);
		//Calculated the desired velocity based on the object's position and the target position
		glm::vec3 desiredVelocity =  objPos - targetPos;
		//Calculate the distance from the object to the target
		float distance = glm::length(desiredVelocity);
		//Normalize Desired Velocity
		desiredVelocity = glm::normalize(desiredVelocity) * maxSpeed;

		//If the distance to the target is less than the approach distance
		if (distance < arrivalDistance) {
			//Reduce the speed to get a smoother arrival
			desiredVelocity *= (distance / arrivalDistance);
		}

		//Caluclate Max Force
		glm::vec3 steering = desiredVelocity - objVelocity;
		steering = Limit(steering, maxForce);

		//Avoid Glitch Town
		steering.y = 0.0f;

		return (steering);
	};

	//Name:				Pursuit
	//Parameters:		glm::vec3 objPos, glm::vec3 objVelocity, glm::vec3 targetPos, glm::vec3 targetVelocity, float maxSpeed, float maxForce, float arrivalDistance
	//Return Type:		glm::vec3
	//Description:		This function takes an Objects's position, velocity and a target's position and velocity. It then calculates the required vector
	//to pursue it by predicting where the target will be in the future
	static glm::vec3 Pursuit(glm::vec3 objPos, glm::vec3 objVelocity, glm::vec3 targetPos, glm::vec3 targetVelocity, float maxSpeed, float maxForce, float arrivalDistance) {
		//Console_OutputLog(L"Pursuiting", LOGINFO);
		//get our desired velocity to pursue the target
		glm::vec3 desiredVelocity = targetPos - objPos;
		//find the distance between us and the target
		float distance = glm::length(desiredVelocity) * 3;
		//use our distance from the target to see how far ahead we should predict the target
		float predictValue = distance / maxSpeed;
		//calcualte the new targetPos and move to it
		targetPos = targetPos + glm::normalize(targetVelocity) * predictValue;
		return Seek(objPos, objVelocity, targetPos, maxSpeed, maxForce, arrivalDistance);
	};

	//Name:				Evade
	//Parameters:		glm::vec3 objPos, glm::vec3 objVelocity, glm::vec3 targetPos, glm::vec3 targetVelocity, float maxSpeed, float maxForce, float arrivalDistance
	//Return Type:		glm::vec3
	//Description:		This function takes an Objects's position, velocity and a target's position and velocity. It then calculates the required vector
	//to evade it by predicting where the target will be in the future
	static glm::vec3 Evade(glm::vec3 objPos, glm::vec3 objVelocity, glm::vec3 targetPos, glm::vec3 targetVelocity, float maxSpeed, float maxForce, float arrivalDistance) {
		//Console_OutputLog(L"Evading", LOGINFO);
		//get our desired velocity to evade the target
		glm::vec3 desiredVelocity = targetPos - objPos;
		//find the distance between us and the target
		float distance = glm::length(desiredVelocity);
		//use our distance from the target to see how far ahead we should predict the target
		float predictValue = distance / maxSpeed;
		//calcualte the new targetPos and move away from it
		targetPos = targetPos + glm::normalize(targetVelocity) * predictValue;
		return Flee(objPos, objVelocity, targetPos, maxSpeed, maxForce, arrivalDistance);
	};

	//Name:				Seperate
	//Parameters:		glm::vec3 objPos, glm::vec3 objVec,vector<AIObject*> aiObjects
	//Return Type:		glm::vec3
	//Description:		This function takes a vector of all the AI Objects in the scene and then checks the distance between objPos and the other objects, if
	//our object is too close to another objects then we create a steering force away from the offending object
	static glm::vec3 Separate(glm::vec3 objPos, glm::vec3 objVec,vector<AIObject*> aiObjects)
	{
		//create values to help with getting the return vec
		glm::vec3 tarVec;
		int changeCount = 0;

		//for each AI Object in the scene
		for (size_t i = 0; i < aiObjects.size(); i++)
		{
			//check our distance from the object
			float Distance = glm::length(aiObjects.at(i)->object->position - objPos);
			//If our distance is between 0.0 and 2.0 then
			if ((Distance > 0) && (Distance < 2.0f))
			{
				//add a velocity onto our vector
				tarVec += aiObjects.at(i)->object->position - objPos;
				changeCount++;
			}
		}

		//if we changed any objects 
		if (changeCount > 0) {
			//devide our target velocity by the amount of objects in our scene
			tarVec /= aiObjects.size();
			//invert the target velocity
			tarVec = -tarVec;
		}
		//otherwise return our target velocity
		else {
			return tarVec;
		}

		//return a velocity
		glm::normalize(tarVec);
		glm::vec3 steer = tarVec - objVec;
		steer = Limit(steer, 6.0f);

		return steer;
	}

	//Name:				Leader
	//Parameters:		glm::vec3 objPos, glm::vec3 objVelocity, glm::vec3 targetPos, glm::vec3 targetVelocity, float maxSpeed, float maxForce, float arrivalDistance
	//Return Type:		glm::vec3
	//Description:		This function takes an Objects's position, velocity and a target's position and velocity. It then calculates the required vector
	//to follow the target
	static glm::vec3 Leader(glm::vec3 objPos, glm::vec3 objVelocity, glm::vec3 targetPos, glm::vec3 targetVelocity, float maxSpeed, float maxForce, float arrivalDistance, vector<AIObject*> aiObjects) {
		
		//Aim behind leader
		glm::vec3 tmpPos;
		//invert the velocity
		tmpPos = -targetVelocity; 
		//distance behind from leader
		tmpPos = normalize(tmpPos) * 1.0f; 
		//update new target pos
		targetPos = targetPos + tmpPos; 

		
		//Evade if in front of leader
		float radiusEvade = 3.0f;
		glm::vec3 evadeAreaPos;
		tmpPos = targetVelocity;
		//distance in front from leader
		tmpPos = normalize(tmpPos) * 1.0f;
		evadeAreaPos = targetPos + tmpPos;
		//get distance to evade area
		glm::vec3 evadeVelo = evadeAreaPos - objPos;
		float distance = glm::length(evadeVelo);
		evadeVelo = glm::normalize(evadeVelo) * maxSpeed;

		//If we are inside the evade area
		if (distance < radiusEvade) {
			//Evade
			glm::vec3 resultVec = Evade(objPos, objVelocity, evadeAreaPos, evadeVelo, maxSpeed, maxForce, arrivalDistance);
			resultVec = glm::normalize(resultVec) * maxSpeed;
			//Seperate the objects
			resultVec = resultVec + Separate(objPos, resultVec, aiObjects);
			return resultVec;
		}

		glm::vec3 desiredVelo = Seek(objPos, objVelocity, targetPos, maxSpeed, maxForce, arrivalDistance);
		//Seperate the objects
		desiredVelo = desiredVelo + Separate(objPos, desiredVelo, aiObjects);

		return desiredVelo;
	}

private:
	//Used to wander
	float wanderLifeTime = 0.0f;
	float time = 0.0f;
	glm::vec2 allowedWanderLifeTimes = glm::vec2(0.1f, 3.0f);
	glm::vec3 wanderTarget;
};