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

struct Transform {
public:
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 rotation = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);
	float rotationAngle = 0;
};

class Plane {
public:
	Plane();

	Plane(glm::vec3 pos, float scale, glm::vec3 _color) {


		transform.position = pos;
		color = _color;

		//Create verts

		//A---B
		//| / |
		//C---D

		//A
		verts.push_back(glm::vec3(pos.x - scale, pos.y, pos.z + scale));
		//B
		verts.push_back(glm::vec3(pos.x + scale, pos.y, pos.z + scale));
		//C
		verts.push_back(glm::vec3(pos.x - scale, pos.y, pos.z - scale));

		//B
		verts.push_back(glm::vec3(pos.x + scale, pos.y, pos.z + scale));
		//D
		verts.push_back(glm::vec3(pos.x + scale, pos.y, pos.z - scale));
		//C
		verts.push_back(glm::vec3(pos.x - scale, pos.y, pos.z - scale));


	}

	void Render() {
		glBegin(GL_TRIANGLES);

		glColor3f(color.x, color.y, color.z);

		for (size_t i = 0; i < verts.size(); i++)
		{
			glVertex3f(verts.at(i).x, verts.at(i).y, verts.at(i).z);
		}

		glEnd();
	
	};

	vector<glm::vec3> verts;
	Transform transform;
	glm::vec3 color;
};

class Sphere {
public:
	Sphere();
	Sphere(float _radius, glm::vec3 _color, glm::vec3 _position) {
		radius = _radius;
		color = _color;
		transform.position = _position;
	};
	void Render() {
		glPushMatrix();

		glTranslatef(transform.position.x, transform.position.y, transform.position.z);

		glColor3f(color.x, color.y, color.z);
		glutSolidSphere(radius, 50, 50);

		glPopMatrix();
	};
	Transform transform;
	glm::vec3 color;
	float radius = 1.0f;
};