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

const glm::vec3 zVector = glm::vec3(0, 0, 0);

struct clothNode {
protected:
	const float damping = 0.01f;
	const float phyStep = 0.25f;
public:
	bool staticNode = false;
	float mass = 10.0f;
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 oldPosition = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	glm::vec3 normal = glm::vec3(0, 0, 0); //Used for shading

	clothNode();
	clothNode(glm::vec3 pos) {
		position = pos;
		oldPosition = pos;
	};
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
	clothNode* p1 = nullptr;
	clothNode* p2 = nullptr;

	clothConstraint(clothNode* _p1, clothNode* _p2) {
		p1 = _p1;
		p2 = _p2;
		stableDistance = glm::length(glm::vec3(p2->position - p1->position));
	}

	void Tick(float deltaTime) {
		glm::vec3 direction = p2->position - p1->position; //get direction from p2 to p1
		currDistance = glm::length(direction); //get the distance between the points
		glm::vec3 corrVec = (direction * (1 - stableDistance / currDistance)) * 0.5f; //find the vector to move the points towards each other if they are stretching
		p1->moveBy(corrVec);
		p2->moveBy(-corrVec);
	}
};

class Cloth {
public:
	Cloth(glm::vec2 _size, float scaleFactor, float _gL) {

		Console_OutputLog(to_wstring("Creating Cloth Object Of Size: " + to_string(_size.x) + ":" + to_string(_size.y)), LOGINFO);
		
		groundLevel = _gL;

		//set size of cloth
		size = _size;

		//resize vectors to size
		clothNodes.resize(size.y);

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
			clothNodes.at(size.y-1).at(i)->staticNode = true;
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

		glBegin(GL_LINES);

		//Color
		glColor3f(1.0f, 1.0f, 1.0f);
		
		for (size_t y = 0; y < size.y - 1; y++)
		{
			for (size_t x = 0; x < size.x - 1; x++)
			{

				//Draw vertex and normals

				//A---B
				//| / |
				//C---D
				//A
				glVertex3f(clothNodes.at(y).at(x)->position.x, clothNodes.at(y).at(x)->position.y, clothNodes.at(y).at(x)->position.z);
				glm::vec3 normNormal = glm::normalize(clothNodes.at(y).at(x)->normal);
				glNormal3f(normNormal.x, normNormal.y, normNormal.z);
				//B
				glVertex3f(clothNodes.at(y).at(x + 1)->position.x, clothNodes.at(y).at(x + 1)->position.y, clothNodes.at(y).at(x + 1)->position.z);
				normNormal = glm::normalize(clothNodes.at(y).at(x + 1)->normal);
				glNormal3f(normNormal.x, normNormal.y, normNormal.z);
				//C
				glVertex3f(clothNodes.at(y + 1).at(x)->position.x, clothNodes.at(y + 1).at(x)->position.y, clothNodes.at(y + 1).at(x)->position.z);
				normNormal = glm::normalize(clothNodes.at(y + 1).at(x)->normal);
				glNormal3f(normNormal.x, normNormal.y, normNormal.z);

				//B
				glVertex3f(clothNodes.at(y).at(x + 1)->position.x, clothNodes.at(y).at(x + 1)->position.y, clothNodes.at(y).at(x + 1)->position.z);
				normNormal = glm::normalize(clothNodes.at(y).at(x + 1)->normal);
				glNormal3f(normNormal.x, normNormal.y, normNormal.z);
				//D
				glVertex3f(clothNodes.at(y + 1).at(x + 1)->position.x, clothNodes.at(y + 1).at(x + 1)->position.y, clothNodes.at(y + 1).at(x + 1)->position.z);
				normNormal = glm::normalize(clothNodes.at(y + 1).at(x + 1)->normal);
				glNormal3f(normNormal.x, normNormal.y, normNormal.z);
				//C
				glVertex3f(clothNodes.at(y + 1).at(x)->position.x, clothNodes.at(y + 1).at(x)->position.y, clothNodes.at(y + 1).at(x)->position.z);
				normNormal = glm::normalize(clothNodes.at(y + 1).at(x)->normal);
				glNormal3f(normNormal.x, normNormal.y, normNormal.z);

			}
		}


		glEnd();
	}

	void Tick(float deltaTime) {
		for (size_t i = 0; i < clothConstraints.size(); i++)
		{
			clothConstraints.at(i)->Tick(deltaTime);
		}

		for (size_t y = 0; y < size.y; y++)
		{
			for (size_t x = 0; x < size.x; x++)
			{
				clothNodes.at(y).at(x)->Tick(deltaTime, groundLevel);
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
		for (size_t y = 0; y < size.y; y++)
		{
			for (size_t x = 0; x < size.x; x++)
			{
				clothNodes.at(y).at(x)->addForce(dir, groundLevel);
			}
		}
	}

	glm::vec2 size;
	vector<vector<clothNode*>> clothNodes;
	vector<clothConstraint*> clothConstraints;
	float groundLevel = 0;

};