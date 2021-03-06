#pragma once
#include <iostream>
#include <glew.h>
#include <freeglut.h>
#include <SOIL.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "ConsoleController.h"
#include "Util.h"
#include "SimpleObjects.h"

const glm::vec3 zVector = glm::vec3(0, 0, 0);

struct clothNode {
protected:
	const float phyStep = 0.25f;
public:
	bool staticNode = false;
	float mass = 10.0f;
	float damping = 0.01f;
	glm::vec3 spawnPos = glm::vec3(0, 0, 0);
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 oldPosition = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	glm::vec3 normal = glm::vec3(0, 0, 0); //Used for shading

	clothNode();
	clothNode(glm::vec3 pos) {
		spawnPos = pos;
		position = pos;
		oldPosition = pos;
	};

	void Reset() {
		position = spawnPos;
		oldPosition = spawnPos;
		acceleration = zVector;
		damping = 0.01f;
	}

	void addForce(glm::vec3 force, float groundLevel)
	{
		//Check if on ground
		if (position.y < groundLevel) {
			//if we are get rid of the y force
			force.y = 0.0f;
		}

		acceleration += force / mass;
	};
	void Tick(float deltaTime, float groundLevel) {
		
		//Are we below the ground?
		if (position.y < groundLevel) {
			//put us back on the ground
			position.y = groundLevel;
			acceleration.y = 0;
		}

		//If moveable
		if (!staticNode) {
			//Are we on ground?
			if (position.y > groundLevel) {
				glm::vec3 _p = position;
				position = position + (position - oldPosition) * (1.0f - damping) + acceleration * phyStep;
				oldPosition = _p;
				acceleration = zVector;
			}
			else {
				//acceleration = zVector;
				acceleration.y = 0;
			}
		}
	}
	void moveBy(glm::vec3 pos) {
		if (!staticNode) {
			position += pos;
		}
	}

};

struct clothConstraint {
public:
	float stableDistance = 0.0f;
	float currDistance = 0.0f;
	float tearDistance = 10.0f;
	bool torn = false;
	clothNode* p1 = nullptr;
	clothNode* p2 = nullptr;

	clothConstraint(clothNode* _p1, clothNode* _p2) {
		p1 = _p1;
		p2 = _p2;
		stableDistance = glm::length(glm::vec3(p2->position - p1->position));
		tearDistance = stableDistance * 7.0f;
	}

	~clothConstraint() {
		delete p1;
		delete p2;
	}

	glm::vec4 FindStress() {

		//white - red
		//good  - extreme stress
		//1.0f  - 0.0f
		float stress = 1.0f;

		//Find current distance between the points
		glm::vec3 direction = p2->position - p1->position; //get direction from p2 to p1
		currDistance = glm::length(direction); //get the distance between the points

		//If we are torn
		if ((currDistance > tearDistance) || torn) {
			return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		//Find how much stress we are under
		stress = ((currDistance / tearDistance)-1)*-1;

		//Return a color
		return glm::vec4(1.0f, stress, stress, 1.0f);

	}

	void Reset() {
		stableDistance = glm::length(glm::vec3(p2->position - p1->position));
		tearDistance = stableDistance * 10.0f;
		torn = false;
	}

	void Tick(float deltaTime) {
		//Tear constraint if we go beyond our tear distance
		if (currDistance > tearDistance) {
			torn = true;
		}

		glm::vec3 direction = p2->position - p1->position; //get direction from p2 to p1
		currDistance = glm::length(direction); //get the distance between the points
		glm::vec3 corrVec = (direction * (1 - stableDistance / currDistance)) * 0.5f; //find the vector to move the points towards each other if they are stretching

		//If the cloth is not torn
		if (!torn) {
			p1->moveBy(corrVec);
			p2->moveBy(-corrVec);
		}
	}
};

class Cloth {
public:
	~Cloth() {

	}

	Cloth(glm::vec2 _size, float _scaleFactor, float _gL) {

		Console_OutputLog(to_wstring("Creating Cloth Object Of Size: " + to_string(_size.x) + ":" + to_string(_size.y)), LOGINFO);
		
		groundLevel = _gL;

		//set size of cloth
		size = _size;
		this->scaleFactor = _scaleFactor;
		//resize vectors to size
		clothNodes.resize((unsigned int) size.y);

		//Generate nodes
		Console_OutputLog(L"Creating Cloth Nodes", LOGINFO);
		for (size_t y = 0; y < size.y; y++)
		{
			for (size_t x = 0; x < size.x; x++)
			{
				clothNodes.at(y).push_back(new clothNode(glm::vec3(x,y,0)*scaleFactor));
			}
		}

		//Connect nodes
		Console_OutputLog(L"Connecting Cloth Nodes", LOGINFO);
		for (size_t y = 0; y < size.y-1; y++)
		{
			for (size_t x = 0; x < size.x-1; x++)
			{
				//A---B
				//| / |
				//C---D

				//A-B
				clothConstraints.push_back(new clothConstraint(clothNodes.at(y).at(x), clothNodes.at(y).at(x + 1)));
				//B-C
				clothConstraints.push_back(new clothConstraint(clothNodes.at(y).at(x + 1), clothNodes.at(y + 1).at(x)));
				//C-A
				clothConstraints.push_back(new clothConstraint(clothNodes.at(y + 1).at(x), clothNodes.at(y).at(x)));

				//B-D
				clothConstraints.push_back(new clothConstraint(clothNodes.at(y).at(x + 1), clothNodes.at(y + 1).at(x + 1)));
				//D-C
				clothConstraints.push_back(new clothConstraint(clothNodes.at(y + 1).at(x + 1), clothNodes.at(y + 1).at(x)));
				//C-B
				clothConstraints.push_back(new clothConstraint(clothNodes.at(y + 1).at(x), clothNodes.at(y).at(x + 1)));
			}
		}

		//Static Nodes
		Console_OutputLog(L"Setting Static Cloth Nodes", LOGINFO);
		for (size_t i = 0; i < size.x; i++)
		{
			clothNodes.at(((unsigned int)size.y)-1).at(i)->staticNode = true;
		}


		Console_OutputLog(L"Created Cloth Object", LOGINFO);
	}

	glm::vec3 calcNormalSection(clothNode* p1, clothNode* p2, clothNode* p3)
	{
		glm::vec3 v1 = p2->position - p1->position;
		glm::vec3 v2 = p3->position - p1->position;

		return glm::cross(v1, v2);
	}

	void calcNormals() {

		//reset normals
		for (size_t y = 0; y < size.y; y++)
		{
			for (size_t x = 0; x < size.x; x++)
			{
				clothNodes.at(y).at(x)->normal = zVector;
			}
		}

		for (size_t y = 0; y < size.y - 1; y++)
		{
			for (size_t x = 0; x < size.x - 1; x++)
			{
				//A---B
				//| / |
				//C---D
				
				//A-B-C
				glm::vec3 normal = calcNormalSection(clothNodes.at(y).at(x), clothNodes.at(y).at(x + 1), clothNodes.at(y + 1).at(x));
				clothNodes.at(y).at(x)->normal += normal;
				clothNodes.at(y).at(x + 1)->normal += normal;
				clothNodes.at(y + 1).at(x)->normal += normal;

				//B-D-C
				normal = calcNormalSection(clothNodes.at(y).at(x + 1), clothNodes.at(y + 1).at(x + 1), clothNodes.at(y + 1).at(x));
				clothNodes.at(y).at(x + 1)->normal += normal;
				clothNodes.at(y + 1).at(x + 1)->normal += normal;
				clothNodes.at(y + 1).at(x)->normal += normal;
			}
		}
	}

	void Render() {

		calcNormals();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBegin(GL_LINES);

		//Color
		//glColor3f(1.0f, 1.0f, 1.0f);
		
		int i = 0;

		for (size_t y = 0; y < size.y - 1; y++)
		{
			for (size_t x = 0; x < size.x - 1; x++)
			{

				//Draw vertex and normals

				//A---B
				//| / |
				//C---D

				//Color
				glm::vec4 color = clothConstraints.at(i)->FindStress();
				glColor4f(color.x, color.y, color.z, color.w);
				i++;

				//Don't Render if it is transparent
				if (color.w > 0.5) {
					//A
					glVertex3f(clothNodes.at(y).at(x)->position.x, clothNodes.at(y).at(x)->position.y, clothNodes.at(y).at(x)->position.z);
					glm::vec3 normNormal = glm::normalize(clothNodes.at(y).at(x)->normal);
					glNormal3f(normNormal.x, normNormal.y, normNormal.z);
					//B
					glVertex3f(clothNodes.at(y).at(x + 1)->position.x, clothNodes.at(y).at(x + 1)->position.y, clothNodes.at(y).at(x + 1)->position.z);
					normNormal = glm::normalize(clothNodes.at(y).at(x + 1)->normal);
					glNormal3f(normNormal.x, normNormal.y, normNormal.z);
				}


				//Color
				color = clothConstraints.at(i)->FindStress();
				glColor4f(color.x, color.y, color.z, color.w);
				i++;

				if (color.w > 0.5) {
					//C
					glVertex3f(clothNodes.at(y + 1).at(x)->position.x, clothNodes.at(y + 1).at(x)->position.y, clothNodes.at(y + 1).at(x)->position.z);
					glm::vec3 normNormal = glm::normalize(clothNodes.at(y + 1).at(x)->normal);
					glNormal3f(normNormal.x, normNormal.y, normNormal.z);

					//B
					glVertex3f(clothNodes.at(y).at(x + 1)->position.x, clothNodes.at(y).at(x + 1)->position.y, clothNodes.at(y).at(x + 1)->position.z);
					normNormal = glm::normalize(clothNodes.at(y).at(x + 1)->normal);
					glNormal3f(normNormal.x, normNormal.y, normNormal.z);
				}
				//Color
				color = clothConstraints.at(i)->FindStress();
				glColor4f(color.x, color.y, color.z, color.w);
				i++;

				if (color.w > 0.5) {
					//C
					glVertex3f(clothNodes.at(y + 1).at(x)->position.x, clothNodes.at(y + 1).at(x)->position.y, clothNodes.at(y + 1).at(x)->position.z);
					glm::vec3 normNormal = glm::normalize(clothNodes.at(y + 1).at(x)->normal);
					glNormal3f(normNormal.x, normNormal.y, normNormal.z);

					//A
					glVertex3f(clothNodes.at(y).at(x)->position.x, clothNodes.at(y).at(x)->position.y, clothNodes.at(y).at(x)->position.z);
					normNormal = glm::normalize(clothNodes.at(y).at(x)->normal);
					glNormal3f(normNormal.x, normNormal.y, normNormal.z);
				}

				//Second Triangle

				//Color
				color = clothConstraints.at(i)->FindStress();
				glColor4f(color.x, color.y, color.z, color.w);
				i++;

				if (color.w > 0.5) {
					//B
					glVertex3f(clothNodes.at(y).at(x + 1)->position.x, clothNodes.at(y).at(x + 1)->position.y, clothNodes.at(y).at(x + 1)->position.z);
					glm::vec3 normNormal = glm::normalize(clothNodes.at(y).at(x + 1)->normal);
					glNormal3f(normNormal.x, normNormal.y, normNormal.z);

					//D
					glVertex3f(clothNodes.at(y + 1).at(x + 1)->position.x, clothNodes.at(y + 1).at(x + 1)->position.y, clothNodes.at(y + 1).at(x + 1)->position.z);
					normNormal = glm::normalize(clothNodes.at(y + 1).at(x + 1)->normal);
					glNormal3f(normNormal.x, normNormal.y, normNormal.z);
				}

				//Color
				color = clothConstraints.at(i)->FindStress();
				glColor4f(color.x, color.y, color.z, color.w);
				i++;

				if (color.w > 0.5) {
					//D
					glVertex3f(clothNodes.at(y + 1).at(x + 1)->position.x, clothNodes.at(y + 1).at(x + 1)->position.y, clothNodes.at(y + 1).at(x + 1)->position.z);
					glm::vec3 normNormal = glm::normalize(clothNodes.at(y + 1).at(x + 1)->normal);
					glNormal3f(normNormal.x, normNormal.y, normNormal.z);

					//C
					glVertex3f(clothNodes.at(y + 1).at(x)->position.x, clothNodes.at(y + 1).at(x)->position.y, clothNodes.at(y + 1).at(x)->position.z);
					normNormal = glm::normalize(clothNodes.at(y + 1).at(x)->normal);
					glNormal3f(normNormal.x, normNormal.y, normNormal.z);
				}

				//Color
				color = clothConstraints.at(i)->FindStress();
				glColor4f(color.x, color.y, color.z, color.w);
				i++;

				if (color.w > 0.5) {
					//C
					glVertex3f(clothNodes.at(y + 1).at(x)->position.x, clothNodes.at(y + 1).at(x)->position.y, clothNodes.at(y + 1).at(x)->position.z);
					glm::vec3 normNormal = glm::normalize(clothNodes.at(y + 1).at(x)->normal);
					glNormal3f(normNormal.x, normNormal.y, normNormal.z);

					//B
					glVertex3f(clothNodes.at(y).at(x + 1)->position.x, clothNodes.at(y).at(x + 1)->position.y, clothNodes.at(y).at(x + 1)->position.z);
					normNormal = glm::normalize(clothNodes.at(y).at(x + 1)->normal);
					glNormal3f(normNormal.x, normNormal.y, normNormal.z);
				}
			}
		}

		//glDisable(GL_BLEND);

		glEnd();
	}

	void Reset() {
		for (size_t y = 0; y < size.y; y++)
		{
			for (size_t x = 0; x < size.x; x++)
			{
				clothNodes.at(y).at(x)->Reset();
			}
		}

		for (size_t i = 0; i < clothConstraints.size(); i++)
		{
			clothConstraints.at(i)->Reset();
		}

		//Static Nodes
		Console_OutputLog(L"Setting Static Cloth Nodes", LOGINFO);
		for (size_t i = 0; i < size.x; i++)
		{
			clothNodes.at(((unsigned int)size.y) - 1).at(i)->staticNode = true;
		}
	}

	void Tick(float deltaTime) {
		for (size_t i = 0; i < clothConstraints.size(); i++)
		{
			clothConstraints.at(i)->Tick(deltaTime);
		}

		if (clothNodes.size() > 0) {
			for (size_t y = 0; y < size.y; y++)
			{
				for (size_t x = 0; x < size.x; x++)
				{
					clothNodes.at(y).at(x)->Tick(deltaTime, groundLevel);
				}
			}
		}
	}

	void AdjustDamping(float amount) {
		for (size_t y = 0; y < clothNodes.size(); y++)
		{
			for (size_t x = 0; x < clothNodes.at(0).size(); x++)
			{
				clothNodes.at(y).at(x)->damping += amount;
			}
		}
	}

	void CollisionLogic(Sphere* sphere, glm::vec3 rayDir, glm::vec3 camPos, bool mouseDown) {
		//Check if a node in inside a sphere
		for (size_t y = 0; y < clothNodes.size(); y++)
		{
			for (size_t x = 0; x < clothNodes.at(0).size(); x++)
			{
				//If we are in a sphere
				if (glm::distance(sphere->transform.position, clothNodes.at(y).at(x)->position) < sphere->radius) {
					//Push out of sphere

					glm::vec3 directionVec = clothNodes.at(y).at(x)->position - sphere->transform.position;

					directionVec = glm::normalize(directionVec);

					//clothNodes.at(y).at(x)->moveBy(directionVec - (sphere->radius - 10)); //black hole
					
					clothNodes.at(y).at(x)->moveBy(directionVec);
				}
			}
		}

		if (mouseDown) {
			//If we are holding mouse down
			for (size_t y = 0; y < clothNodes.size(); y++)
			{
				for (size_t x = 0; x < clothNodes.at(0).size(); x++)
				{
					//If the ray hit our node
					float Scale = 0.1f;

					glm::vec3 dirfrac;

					// r.dir is unit direction vector of ray
					dirfrac.x = 1.0f / rayDir.x;
					dirfrac.y = 1.0f / rayDir.y;
					dirfrac.z = 1.0f / rayDir.z;
					// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
					// r.org is origin of ray
					float t1 = ((clothNodes.at(y).at(x)->position.x - Scale) - camPos.x) * dirfrac.x;
					float t2 = ((clothNodes.at(y).at(x)->position.x + Scale) - camPos.x) * dirfrac.x;
					float t3 = ((clothNodes.at(y).at(x)->position.y - Scale) - camPos.y) * dirfrac.y;
					float t4 = ((clothNodes.at(y).at(x)->position.y + Scale) - camPos.y) * dirfrac.y;
					float t5 = ((clothNodes.at(y).at(x)->position.z) - camPos.z) * dirfrac.z;
					float t6 = ((clothNodes.at(y).at(x)->position.z) - camPos.z) * dirfrac.z;

					float tmin = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
					float tmax = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

					// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
					if (tmax < 0)
					{
						//Console_OutputLog(L"BEHIND", LOGINFO);
						clothNodes.at(y).at(x)->addForce(-rayDir * 15.0f, groundLevel);
					}
					// if tmin > tmax, ray doesn't intersect AABB
					else if (tmin > tmax)
					{
						//Console_OutputLog(L"MISSED", LOGINFO);
					}
					else {
						//Apply Force
						Console_OutputLog(L"HIT", LOGINFO);
						clothNodes.at(y).at(x)->addForce(-rayDir * 15.0f, groundLevel);
					}
				}
			}
		}

	}
	
	void ApplyWind(glm::vec3 windDir, float amp) {
		for (size_t y = 0; y < size.y-1; y++)
		{
			for (size_t x = 0; x < size.x-1; x++)
			{
				//Add force onto the section of the cloth
				glm::vec3 windNorm = glm::normalize(calcNormalSection(clothNodes.at(y).at(x), clothNodes.at(y).at(x + 1), clothNodes.at(y + 1).at(x)));

				float dot = glm::dot(windDir, windNorm);

				glm::vec3 force = (windNorm * dot) * amp;

				clothNodes.at(y).at(x)->addForce(force, groundLevel);
			}
		}
	}

	void AllDyanmic() {
		for (size_t y = 0; y < size.y; y++)
		{
			for (size_t x = 0; x < size.x; x++)
			{
				clothNodes.at(y).at(x)->staticNode = false;
			}
		}
	}

	void globalForce(glm::vec3 dir) {
		if (clothNodes.size() > 0) {
			for (size_t y = 0; y < size.y; y++)
			{
				for (size_t x = 0; x < size.x; x++)
				{
					clothNodes.at(y).at(x)->addForce(dir, groundLevel);
				}
			}
		}
	}

	glm::vec2 size;
	vector<vector<clothNode*>> clothNodes;
	vector<clothConstraint*> clothConstraints;
	float groundLevel = 0;
	float scaleFactor = 10;

};