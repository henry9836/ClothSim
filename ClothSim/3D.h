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
#include <fstream>

#include "Camera.h"
#include "ShaderLoader.h"
#include "ConsoleController.h"
#include "TextureLoader.h"
#include "Model.h"
#include "Input.h"
#include "AI.h"

//CubeMaps
class CubeMap {
public:

	//Create the Cubemap Object
	void Initalise(Camera* _cam, std::string _pathToCubeMap, std::string _name) {

		Console_OutputLog(to_wstring("Initalising CubeMap: " + _name), LOGINFO);

		this->camera = _cam;
		this->name = _name;

		//Create Vertcies and Indices

		GLfloat CubeMapVertices[24] = {};

		GLuint CubeMapIndices[36] = {};

		for (int i = 0; i < 8; i++)
		{
			if (i < 4)
			{
				CubeMapVertices[i * 3] = -1.0f;
				for (int j = 0; j < 4; j++)
				{
					if (j < 2)
					{
						CubeMapVertices[(j * 3) + 1] = 1.0f;
					}
					else
					{
						CubeMapVertices[(j * 3) + 1] = -1.0f;
					}

					if (j % 2 == 0)
					{
						CubeMapVertices[(j * 3) + 2] = 1.0f;
					}
					else
					{
						CubeMapVertices[(j * 3) + 2] = -1.0f;
					}
				}
			}
			else
			{
				CubeMapVertices[i * 3] = 1.0f;
				for (int j = 4; j < 8; j++)
				{
					if (j < 6)
					{
						CubeMapVertices[(j * 3) + 1] = 1.0f;
					}
					else
					{
						CubeMapVertices[(j * 3) + 1] = -1.0f;
					}

					if (j % 2 == 0)
					{
						CubeMapVertices[(j * 3) + 2] = 1.0f;
					}
					else
					{
						CubeMapVertices[(j * 3) + 2] = -1.0f;
					}
				}
			}
		}

		GLuint temp[] = {
			3,2,6,
			3,6,7,

			1,4,0,
			1,5,4,

			2,1,0,
			2,3,1,

			3,5,1,
			3,7,5,

			6,4,5,
			6,5,7,

			4,2,0,
			4,6,2,

		};

		for (int i = 0; i < (int)sizeof(temp) / sizeof(temp[0]); i++)
		{
			CubeMapIndices[i] = temp[i];
		}

		//Bind and Generate Info

		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CubeMapVertices), CubeMapVertices, GL_STATIC_DRAW);

		glGenBuffers(1, &this->EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeMapIndices), CubeMapIndices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);

		//Face Image Names

		this->faces.push_back("right.jpg");
		this->faces.push_back("left.jpg");
		this->faces.push_back("top.jpg");
		this->faces.push_back("bottom.jpg");
		this->faces.push_back("back.jpg");
		this->faces.push_back("front.jpg");

		//Create program

		this->program = ShaderLoader::CreateProgram("Resources/Shaders/CubeMapFog.vs", "Resources/Shaders/CubeMapFog.fs");

		int width, height;
		unsigned char* image;

		//Assign the faces onto sides
		for (GLuint i = 0; i < 6; i++)
		{
			std::string tmpPath = _pathToCubeMap;
			tmpPath.append(this->faces.at(i));
			image = SOIL_load_image(tmpPath.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			SOIL_free_image_data(image);
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		this->texID = this->texture;

		Console_OutputLog(to_wstring("CubeMap: " + _name + " Initalised"), LOGINFO);
	}

	//Update rotation, scale and position of cube map so it renders correctly
	void Update()
	{
		this->model = glm::scale(glm::mat4(), glm::vec3(2000.0f, 2000.0f, 2000.0f));
		this->projCalc = this->camera->getMVP(this->position, this->scale, this->rotationZ) * this->model;
	}

	//Render the Cubemap
	void Render()
	{
		glUseProgram(this->program);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);
		glUniform1i(glGetUniformLocation(this->program, "cubeSampler"), 0);
		glUniformMatrix4fv(glGetUniformLocation(this->program, "proj_calc"), 1, GL_FALSE, glm::value_ptr(this->projCalc));

		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	GLuint texID = NULL;

private:

	std::string name = "Untitled CubeMap";
	glm::mat4 model;
	glm::mat4 projCalc;
	glm::mat4 rotationZ;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	Camera* camera = nullptr;
	vector<std::string> faces;
	GLuint VAO = NULL;
	GLuint VBO = NULL;
	GLuint EBO = NULL;
	GLuint texture = NULL;
	GLuint image = NULL;
	GLuint program = NULL;
};

//Simple 3D Objects
class Simple3DObject {
public:
	std::string name = "Untitled Basic 3D";

	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(10.0f, 10.0f, 10.0f);
	glm::vec3 rotationAxisZ = glm::vec3(0.0f, 1.0f, 0.0f);

	bool reflective = false;

	GLuint VAO = NULL;
	GLuint VBO = NULL;
	GLuint EBO = NULL;
	GLuint texture = NULL;
	GLuint image = NULL;
	GLuint program = NULL;

	float rotationAngle = 0;

	glm::mat4 model;
	glm::mat4 projCalc;
	glm::mat4 rotationZ;
	glm::mat4 translationMatrix;
	glm::mat4 scaleMatrix;

	int m_size = 0;

	void Initalise(glm::vec3 _position, glm::vec3 _scale, std::string textureFilePath, std::string vShaderFilePath, std::string fShaderFilePath, GLuint Indices[], GLfloat Verts[], std::string _name, bool _reflective);
	void Render(Camera* cam, CubeMap* _skyBox, GLuint Indices[]);

};

//Bullets
class Bullet {
public:
	Bullet(Model* mObject, float deltaTime);
	~Bullet();
	void Tick(float deltaTime);
	bool amAllowedAlive();
	Model* object;
	float lifeTime = 5.0f;
	float time = 0.0f;
	float deadLifeTime = 1000.0f;
	bool isOnPlayerTeam = false;
};

//Enemies
class Enemy {
public:
	Enemy(Model* mObject, float deltaTime);
	~Enemy();
	void Tick(float deltaTime);
	bool amAllowedAlive = true;
	Model* object;
	float moveSpeed = 5.0f;
};

class Player {
	public:
	Model* object;
	glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };
	glm::vec3 targetPos = { 0.0f, 0.0f, 0.0f };
	float maxSpeed = 7.0f;
	float maxForce = 3.0f;

	//DEBUG
	bool Flee = false;
	bool Wander = false;
	bool Pursuit = false;
	bool Evade = false;
	bool Leader = false;

	Player(Model* mObject);
	~Player();

	void Update(GLfloat deltaTime, glm::vec4 maxWorldSize);

};

class Terrain {

public:

	void findNormal() {
		// Estimate normals for interior nodes using central difference.
		float invTwoDX = 1.0f / (2.0f * 1.0f);
		float invTwoDZ = 1.0f / (2.0f * 1.0f);
		for (UINT i = 2; i < this->imageSize.y; ++i)
		{
			for (UINT j = 2; j < this->imageSize.x; ++j)
			{
				float t = this->heightInfo[(i - 1) * (unsigned int)this->imageSize.x + j];
				float b = this->heightInfo[(i + 1) * (unsigned int)this->imageSize.x + j];
				float l = this->heightInfo[i * (unsigned int)this->imageSize.x + j - 1];
				float r = this->heightInfo[i * (unsigned int)this->imageSize.x + j + 1];

				glm::vec3 tanZ(0.0f, (t - b) * invTwoDZ, 1.0f);
				glm::vec3 tanX(1.0f, (r - l) * invTwoDX, 0.0f);

				glm::vec3 N;
				N = glm::cross(tanZ, tanX);
				glm::normalize(N);

				this->TerrianNormals[(i - 2) * (unsigned int)this->imageSize.x + (j - 2)] = N;
			}
		}
	}

	float width()const
	{
		return (this->imageSize.x - 1) * 1;
	}

	float depth()const
	{
		return (this->imageSize.y - 1) * 1;
	}

	float getHeight(float x, float z)const
	{
		try {
			// Transform from terrain local space to "cell" space.
			float c = (x + 0.5f * width()) / 1;
			float d = (z - 0.5f * depth()) / -1;

			// Get the row and column we are in.
			int row = (int)floorf(d);
			int col = (int)floorf(c);

			if (row >= 0 && col >= 0) {

				// Grab the heights of the cell we are in.
				// A*--*B
				//  | /|
				//  |/ |
				// C*--*D
				float A = heightInfo[row * (unsigned int)this->imageSize.x + col];
				float B = heightInfo[row * (unsigned int)this->imageSize.x + col + 1];
				float C = heightInfo[(row + 1) * (unsigned int)this->imageSize.x + col];
				float D = heightInfo[(row + 1) * (unsigned int)this->imageSize.x + col + 1];

				// Where we are relative to the cell.
				float s = c - (float)col;
				float t = d - (float)row;

				// If upper triangle ABC.
				if (s + t <= 1.0f)
				{
					float uy = B - A;
					float vy = C - A;
					return A + s * uy + t * vy;
				}
				else // lower triangle DCB.
				{
					float uy = C - D;
					float vy = B - D;
					return D + (1.0f - s) * uy + (1.0f - t) * vy;
				}
			}
			else {
				return 0;
			}
		}
		catch (...) {
			return 0;
		}
	}

	void Initalise(Camera* _cam, std::string _pathToHeightMap, std::string _name) {
		Console_OutputLog(to_wstring("Initalising Terrain: " + _name), LOGINFO);

		this->name = _name;
		this->camera = _cam;
		

		//Create Vertcies and Indices


		int totalSize = 0;

		//Get Info From Map
		ifstream heightMap;
		heightMap.open(_pathToHeightMap.c_str(), std::ios_base::binary);
		if (heightMap.fail())
		{
			Console_OutputLog(L"Could not load height map", LOGWARN);
			return;
		}
		else {
			//Get Image Size
			
			int w, h, f = 0;

			unsigned char* image = SOIL_load_image
			(
				_pathToHeightMap.c_str(),
				&w, &h, 0,
				SOIL_LOAD_L
			);
			
			if (image == nullptr) {
				Console_OutputLog(L"Could not load height map with SOIL defaulting to 512x512 image size", LOGWARN);
				w = 513;
				h = 513;
			}

			totalSize = w * h;

			/*
			char c;


			while (heightMap >> c) {
				totalSize++;
			}
			*/

			//Resize vectors to image size

			this->rawData.resize(totalSize);
			this->heightInfo.resize(totalSize * 2);

			this->imageSize.x = (float)sqrt((unsigned int)this->rawData.size());
			this->imageSize.y = this->imageSize.x;

			heightMap.close();

			heightMap.open(_pathToHeightMap.c_str(), std::ios_base::binary);

			//Put map info into vector

			heightMap.read((char*)&this->rawData[0], (std::streamsize)this->rawData.size());
		}

		heightMap.close();


		for (UINT i = 0; i < rawData.size(); ++i)
		{
			//heightInfo[i] = (float)rawData[i] * mInfo.HeightScale + mInfo.HeightOffset;
			this->heightInfo[i] = (float)this->rawData[i];
		}

		//Create Vertices From HeightInfo

		int row = 0;
		int col = 0;


		float halfWidth = (this->imageSize.x - 1) * 1.0f * 0.5f;
		float halfDepth = (this->imageSize.y - 1) * 1.0f * 0.5f;

		float du = 1.0f / (this->imageSize.x - 1);
		float dv = 1.0f / (this->imageSize.y - 1);

		TerrianNormals.resize(totalSize + 1);
		int inter = 0;
		findNormal();

		//Create collision vectors
		collisionInfo.resize((unsigned int)this->imageSize.y);
		for (size_t i = 0; i < collisionInfo.size(); i++)
		{
			collisionInfo.at(i).resize((unsigned int)this->imageSize.x);
		}

		for (UINT i = 0; i < (unsigned int)this->imageSize.y -1; ++i)
		{
			float z = halfDepth - i * 1.0f;
			for (UINT j = 0; j < (unsigned int)this->imageSize.x; ++j)
			{

				float x = -halfWidth + j * 1.0f;
				float y = heightInfo[i * (unsigned int)this->imageSize.x + j];


				//Positions
				this->TerrianVertices.push_back(x);
				this->TerrianVertices.push_back(y);
				this->TerrianVertices.push_back(z);
				
				//Normal
				this->TerrianVertices.push_back(TerrianNormals[inter].x);
				this->TerrianVertices.push_back(TerrianNormals[inter].y);
				this->TerrianVertices.push_back(TerrianNormals[inter].z);

				inter++;

				//Stretch texture over grid.
				this->TerrianVertices.push_back(j * du);
				this->TerrianVertices.push_back(i * dv);

			}
		}

		//Create Indices

		// Iterate over each quad and compute indices.

		this->TerrianIndices.resize(totalSize*6);

		int k = 0;
		for (unsigned int i = 0; i < (unsigned int)this->imageSize.y - 1; ++i) {
			for (unsigned int j = 0; j < (unsigned int)this->imageSize.x - 1; ++j) {
				this->TerrianIndices[k] = i * (unsigned int)this->imageSize.x + j;
				this->TerrianIndices[k + 1] = i * (unsigned int)this->imageSize.x + j + 1;
				this->TerrianIndices[k + 2] = (i + 1) * (unsigned int)this->imageSize.x + j;

				this->TerrianIndices[k + 3] = (i + 1) * (unsigned int)this->imageSize.x + j;
				this->TerrianIndices[k + 4] = i * (unsigned int)this->imageSize.x + j + 1;
				this->TerrianIndices[k + 5] = (i + 1) * (unsigned int)this->imageSize.x + j + 1;

				k += 6; // next quad
			}
		}


		//Bind and Generate Info

		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);

		int width, height;

		unsigned char* image = SOIL_load_image("Resources/Textures/map.png", &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_BLEND);

		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->TerrianVertices.size() * sizeof(GLfloat), &this->TerrianVertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &this->EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->TerrianIndices.size() * sizeof(GLuint), &this->TerrianIndices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2); // REMEMBER UR INDICES

		//Create program

		//this->program = ShaderLoader::CreateProgram("Resources/Shaders/3DObjectColor.vs", "Resources/Shaders/3DObjectColor.fs");
		//this->program = ShaderLoader::CreateProgram("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs"); //Renders 2D
		//this->program = ShaderLoader::CreateProgram("Resources/Shaders/Basic3D.vs", "Resources/Shaders/Basic3D.fs"); //Render
		this->program = ShaderLoader::CreateProgram("Resources/Shaders/3DObject_Diffuse.vs", "Resources/Shaders/3DObject_BlinnPhong.fs");
		//this->program = ShaderLoader::CreateProgram("Resources/Shaders/3DObject_Diffuse.vs", "Resources/Shaders/3DObject_DiffuseColor.fs");

		Console_OutputLog(to_wstring("Terrian: " + _name + " Initalised"), LOGINFO);


	}

	void Render(Camera* camera) {

		glUseProgram(this->program);
		glBindVertexArray(this->VAO);

		glm::mat4 model;
		glm::mat4 translationMatrix = glm::translate(glm::mat4(), position);
		glm::mat4 rotationZ = glm::rotate(glm::mat4(), glm::radians(this->rotationAngle), this->rotationAxisZ);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(), scale);
		model = translationMatrix * rotationZ * scaleMatrix;
		glm::mat4 mvp = camera->proj * camera->view * model;
		glm::vec3 camPos = camera->camPos;

		//POSITION AND SCALE
		glm::mat4 projCalc = camera->proj * camera->view * model;

		//PATCH

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture);

		glUniform1i(glGetUniformLocation(this->program, "texture_diffuse1"), 0);


		//PATCH END
		GLint mvpLoc = glGetUniformLocation(program, "proj_calc");
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(projCalc));
		//GLint mvpLoc2 = glGetUniformLocation(program, "vp");
		//glUniformMatrix4fv(mvpLoc2, 1, GL_FALSE, glm::value_ptr(projCalc));
		GLint modelPass = glGetUniformLocation(program, "model");
		glUniformMatrix4fv(modelPass, 1, GL_FALSE, glm::value_ptr(model));
		GLint camPosPass = glGetUniformLocation(program, "camPos");
		glUniformMatrix3fv(camPosPass, 1, GL_FALSE, glm::value_ptr(camPos));


		

		glDrawElements(GL_TRIANGLES, this->TerrianIndices.size(), GL_UNSIGNED_INT, 0);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		//Clearing the vertex array
		glBindVertexArray(0);
		glUseProgram(0);

	}

	vector<vector<float>> collisionInfo;
	glm::vec3 position = glm::vec3(0.0f, -150.0f, 0.0f);

private:
	std::string name = "Untitled Terrian";
	glm::mat4 model;
	glm::mat4 projCalc;
	glm::mat4 rotationZ;
	
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 imageSize;
	glm::vec3 rotationAxisZ = glm::vec3(1.0f, 0.0f, 0.0f);

	vector<unsigned char> rawData;
	vector<float> heightInfo;
	vector<GLfloat> TerrianVertices;
	vector<GLuint> TerrianIndices;
	vector<glm::vec3> TerrianNormals;
	

	Camera* camera = nullptr;
	GLuint VAO = NULL;
	GLuint VBO = NULL;
	GLuint EBO = NULL;
	GLuint texture = NULL;
	GLuint image = NULL;
	GLuint program = NULL;

	float rotationAngle = 0;

};

class GeoShape {
public:
	
	void Render(Camera* cam) {
		glUseProgram(program);

		glm::mat4 model;
		glm::mat4 translationMatrix = glm::translate(glm::mat4(), position);
		glm::mat4 rotationZ = glm::rotate(glm::mat4(), glm::radians(this->rotationAngle), this->rotationAxisZ);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(), scale);
		model = translationMatrix * rotationZ * scaleMatrix;

		glm::mat4 mvp = cam->proj * cam->view * model;
		GLint vpLoc = glGetUniformLocation(program, "mvp");
		glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
	};

	GLuint VAO, VBO, EBO;
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 rotationAxisZ = glm::vec3(1.0f, 0.0f, 0.0f);
	float rotationAngle = 90;
	GLuint program;
};

struct ClothNode {
public:
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	glm::vec3 normal = glm::vec3(0, 0, 0);
	
	float mass = 1.0f;
	float grav = 9.8f;
	float damping = 0.1f;
	
	bool staticNode = false;

	ClothNode() {

	}

	ClothNode(glm::vec3 pos) {
		this->position = pos;
	}

	ClothNode(glm::vec3 pos, bool isStatic) {
		this->position = pos;
		staticNode = isStatic;
	}

	void Update() {

		glm::vec3 gravityForce(0, mass * grav, 0);
		this->ApplyForce(gravityForce);
		acceleration -= velocity * damping / mass;
	}
	void ApplyForce(glm::vec3 force) {
		acceleration += force / mass;
	}
};

struct ClothConstraint {
public:
	const float stiffness = 0.8f;
	const float restingDistance = 5;
	ClothNode* p1;
	ClothNode* p2;

	ClothConstraint(ClothNode* _p1, ClothNode* _p2) {
		p1 = _p1;
		p2 = _p2;
	}

	void Update() {
		glm::vec3 delta = p2->position - p1->position;
		glm::vec3 deltaLength = sqrt(delta * delta);
		glm::vec3 difference = (deltaLength - restingDistance) / deltaLength;
		float im1 = 1 / p1->mass;
		float im2 = 1 / p2->mass;

		p1->position -= delta * (im1 / (im1 + im2)) * stiffness * difference;
		p2->position += delta * (im2 / (im1 + im2)) * stiffness * difference;

		p1->Update();
		p2->Update();
	}
};

class Cloth {

public:

	glm::vec3 crossMultiply(glm::vec3 a, glm::vec3 b) {
		glm::vec3 result = glm::vec3(0, 0, 0);

		return result;
	}

	glm::vec3 findNodeTriangleNormal(ClothNode* n1, ClothNode* n2, ClothNode* n3) {
		glm::vec3 result = glm::vec3(0,0,0);

		glm::vec3 v1 = n2->position - n1->position;
		glm::vec3 v2 = n3->position - n1->position;

		result = crossMultiply(v1, v2);

		return result;
	}

	void findNormal() {
		// Estimate normals for interior nodes using central difference.
		float invTwoDX = 1.0f / (2.0f * 1.0f);
		float invTwoDZ = 1.0f / (2.0f * 1.0f);
		for (UINT i = 2; i < this->size.y; ++i)
		{
			for (UINT j = 2; j < this->size.x; ++j)
			{
				float t = this->heightInfo[(i - 1) * (unsigned int)this->size.x + j];
				float b = this->heightInfo[(i + 1) * (unsigned int)this->size.x + j];
				float l = this->heightInfo[i * (unsigned int)this->size.x + j - 1];
				float r = this->heightInfo[i * (unsigned int)this->size.x + j + 1];

				glm::vec3 tanZ(0.0f, (t - b) * invTwoDZ, 1.0f);
				glm::vec3 tanX(1.0f, (r - l) * invTwoDX, 0.0f);

				glm::vec3 N;
				N = glm::cross(tanZ, tanX);
				glm::normalize(N);

				this->ClothNormals[(i - 2) * (unsigned int)this->size.x + (j - 2)] = N;
			}
		}

		//Update Cloth Node Normals

		for (size_t i = 0; i < this->size.y - 1; i++)
		{
			for (size_t j = 0; j < this->size.x - 1; j++)
			{
				clothNodes.at(i).at(j)->normal = glm::vec3(0, 0, 0);
				clothNodes.at(i).at(j)->normal = findNodeTriangleNormal();

				/*Vec3 normal = calcTriangleNormal(getParticle(x + 1, y), getParticle(x, y), getParticle(x, y + 1));
				getParticle(x + 1, y)->addToNormal(normal);
				getParticle(x, y)->addToNormal(normal);
				getParticle(x, y + 1)->addToNormal(normal);

				normal = calcTriangleNormal(getParticle(x + 1, y + 1), getParticle(x + 1, y), getParticle(x, y + 1));
				getParticle(x + 1, y + 1)->addToNormal(normal);
				getParticle(x + 1, y)->addToNormal(normal);
				getParticle(x, y + 1)->addToNormal(normal);*/
			}
		}

	}

	float width()const
	{
		return (this->size.x - 1) * 1;
	}

	float depth()const
	{
		return (this->size.y - 1) * 1;
	}

	float getHeight(float x, float z)const
	{
		try {
			// Transform from terrain local space to "cell" space.
			float c = (x + 0.5f * width()) / 1;
			float d = (z - 0.5f * depth()) / -1;

			// Get the row and column we are in.
			int row = (int)floorf(d);
			int col = (int)floorf(c);

			if (row >= 0 && col >= 0) {

				// Grab the heights of the cell we are in.
				// A*--*B
				//  | /|
				//  |/ |
				// C*--*D
				float A = heightInfo[row * (unsigned int)this->size.x + col];
				float B = heightInfo[row * (unsigned int)this->size.x + col + 1];
				float C = heightInfo[(row + 1) * (unsigned int)this->size.x + col];
				float D = heightInfo[(row + 1) * (unsigned int)this->size.x + col + 1];

				// Where we are relative to the cell.
				float s = c - (float)col;
				float t = d - (float)row;

				// If upper triangle ABC.
				if (s + t <= 1.0f)
				{
					float uy = B - A;
					float vy = C - A;
					return A + s * uy + t * vy;
				}
				else // lower triangle DCB.
				{
					float uy = C - D;
					float vy = B - D;
					return D + (1.0f - s) * uy + (1.0f - t) * vy;
				}
			}
			else {
				return 0;
			}
		}
		catch (...) {
			return 0;
		}
	}

	void Initalise(Camera* _cam, glm::vec2 _size, std::string _name) {
		Console_OutputLog(to_wstring("Initalising Cloth: " + _name), LOGINFO);

		this->name = _name;
		this->camera = _cam;
		this->size = _size;

		int totalSize = (int)(_size.x * _size.y);

		//Resize vectors to cloth size

		this->rawData.resize(totalSize);
		this->heightInfo.resize(totalSize * 2);


		for (UINT i = 0; i < rawData.size(); ++i)
		{
			this->heightInfo[i] = 50;
		}

		//Create Vertices From HeightInfo

		int row = 0;
		int col = 0;


		float halfWidth = (this->size.x - 1) * 1.0f * 0.5f;
		float halfDepth = (this->size.y - 1) * 1.0f * 0.5f;

		float du = 1.0f / (this->size.x - 1);
		float dv = 1.0f / (this->size.y - 1);

		ClothNormals.resize(totalSize + 1);
		int inter = 0;
		findNormal();

		//size collision vectors
		collisionInfo.resize((unsigned int)this->size.y);
		for (size_t i = 0; i < collisionInfo.size(); i++)
		{
			collisionInfo.at(i).resize((unsigned int)this->size.x);
		}
		
		//Resize node vector
		clothNodes.resize((unsigned int)this->size.y);

		//Create Verts
		for (UINT i = 0; i < (unsigned int)this->size.y - 1; ++i)
		{
			float z = halfDepth - i * 1.0f;
			for (UINT j = 0; j < (unsigned int)this->size.x; ++j)
			{

				float x = -halfWidth + j * 1.0f;
				float y = heightInfo[i * (unsigned int)this->size.x + j];


				//Positions
				this->ClothVertices.push_back(x);
				this->ClothVertices.push_back(y);
				this->ClothVertices.push_back(z);

				//clothNode Position
				this->clothNodes.at(i).push_back(new ClothNode(glm::vec3(x,y,z)));

				//Normal
				this->ClothVertices.push_back(ClothNormals[inter].x);
				this->ClothVertices.push_back(ClothNormals[inter].y);
				this->ClothVertices.push_back(ClothNormals[inter].z);

				inter++;

				//Stretch texture over grid.
				this->ClothVertices.push_back(j * du);
				this->ClothVertices.push_back(i * dv);

			}
		}

		//Create Indices

		// Iterate over each quad and compute indices.

		this->ClothIndices.resize(totalSize * 6);

		int k = 0;
		for (unsigned int i = 0; i < this->size.y - 1; ++i) { //y
			for (unsigned int j = 0; j < this->size.x - 1; ++j) { //x

				this->ClothIndices[k] = i * (unsigned int)this->size.x + j;
				this->ClothIndices[k + 1] = i * (unsigned int)this->size.x + j + 1;
				this->ClothIndices[k + 2] = (i + 1) * (unsigned int)this->size.x + j;

				this->ClothIndices[k + 3] = (i + 1) * (unsigned int)this->size.x + j;
				this->ClothIndices[k + 4] = i * (unsigned int)this->size.x + j + 1;
				this->ClothIndices[k + 5] = (i + 1) * (unsigned int)this->size.x + j + 1;

				k += 6; // next quad
			}
		}

		//Create Constraints

		for (size_t i = 0; i < this->size.y - 2; i++) //y
		{
			for (size_t j = 0; j < this->size.x - 2; j++) //x
			{
				//A---B
				//| / |
				//C---D

				//A-B
				this->clothConstraints.push_back(new ClothConstraint(clothNodes.at(i).at(j), clothNodes.at(i).at(j + 1)));
				//B-C
				this->clothConstraints.push_back(new ClothConstraint(clothNodes.at(i + 1).at(j), clothNodes.at(i).at(j + 1)));
				//C-A
				this->clothConstraints.push_back(new ClothConstraint(clothNodes.at(i).at(j), clothNodes.at(i + 1).at(j)));


				//B-D
				this->clothConstraints.push_back(new ClothConstraint(clothNodes.at(i).at(j + 1), clothNodes.at(i + 1).at(j + 1)));
				//D-C
				this->clothConstraints.push_back(new ClothConstraint(clothNodes.at(i + 1).at(j), clothNodes.at(i + 1).at(j + 1)));
				//C-B
				this->clothConstraints.push_back(new ClothConstraint(clothNodes.at(i + 1).at(j), clothNodes.at(i).at(j + 1)));
			}
		}

		//Bind and Generate Info

		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);

		int width, height;

		unsigned char* image = SOIL_load_image("Resources/Textures/cloth.png", &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_BLEND);

		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->ClothVertices.size() * sizeof(GLfloat), &this->ClothVertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &this->EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->ClothIndices.size() * sizeof(GLuint), &this->ClothIndices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2); // REMEMBER UR INDICES

		//Create program

		//this->program = ShaderLoader::CreateProgram("Resources/Shaders/3DObjectColor.vs", "Resources/Shaders/3DObjectColor.fs");
		//this->program = ShaderLoader::CreateProgram("Resources/Shaders/Basic.vs", "Resources/Shaders/Basic.fs"); //Renders 2D
		//this->program = ShaderLoader::CreateProgram("Resources/Shaders/Basic3D.vs", "Resources/Shaders/Basic3D.fs"); //Render
		this->program = ShaderLoader::CreateProgram("Resources/Shaders/3DObject_Diffuse.vs", "Resources/Shaders/3DObject_BlinnPhong.fs");
		//this->program = ShaderLoader::CreateProgram("Resources/Shaders/3DObject_Diffuse.vs", "Resources/Shaders/3DObject_DiffuseColor.fs");

		Console_OutputLog(to_wstring("Cloth: " + _name + " Initalised"), LOGINFO);


	}

	void Tick(float deltaTime) {
		for (size_t i = 0; i < this->clothConstraints.size(); i++)
		{
			this->clothConstraints.at(i)->Update();
		}

		int k = 0;
		for (UINT i = 0; i < this->size.y - 1; ++i)
		{
			for (UINT j = 0; j < this->size.x; ++j)
			{
				//ClothVertices.at(k) = this->clothNodes.at(i).at(j)->position.x; //x
				//ClothVertices.at(k+1) = this->clothNodes.at(i).at(j)->position.y; //y
				//ClothVertices.at(k+2) = this->clothNodes.at(i).at(j)->position.z; //z

				//next section of verts
				//k += 6;

				ClothVertices.at(k) *= 2;
				ClothVertices.at(k+1) *= 2;
				ClothVertices.at(k+2) *= 2;

				k += 6;
			}
		}

		wcout << L"LP";

	}

	void Render(Camera* camera) {

		//Find normals for nodes

		findNormal();

		glBegin(GL_TRIANGLES);
		
		for (size_t i = 0; i < clothNodes.size(); i++)
		{
			glm::vec3(1.0, 1.0, 1.0);
			glNormal3fv((GLfloat*) & (p1->getNormal().normalized()));
			glVertex3fv((GLfloat*) & (p1->getPos()));

			glNormal3fv((GLfloat*) & (p2->getNormal().normalized()));
			glVertex3fv((GLfloat*) & (p2->getPos()));

			glNormal3fv((GLfloat*) & (p3->getNormal().normalized()));
			glVertex3fv((GLfloat*) & (p3->getPos()));
		}

		glEnd();


		//glUseProgram(this->program);
		//glBindVertexArray(this->VAO);

		//glm::mat4 model;
		//glm::mat4 translationMatrix = glm::translate(glm::mat4(), position);
		//glm::mat4 rotationZ = glm::rotate(glm::mat4(), glm::radians(this->rotationAngle), this->rotationAxisZ);
		//glm::mat4 scaleMatrix = glm::scale(glm::mat4(), scale);
		//model = translationMatrix * rotationZ * scaleMatrix;
		//glm::mat4 mvp = camera->proj * camera->view * model;
		//glm::vec3 camPos = camera->camPos;

		////POSITION AND SCALE
		//glm::mat4 projCalc = camera->proj * camera->view * model;

		////PATCH

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		//if (!wireframe) {

		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, this->texture);

		//	glUniform1i(glGetUniformLocation(this->program, "texture_diffuse1"), 0);
		//}


		////PATCH END
		//GLint mvpLoc = glGetUniformLocation(program, "proj_calc");
		//glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(projCalc));
		//GLint modelPass = glGetUniformLocation(program, "model");
		//glUniformMatrix4fv(modelPass, 1, GL_FALSE, glm::value_ptr(model));
		//GLint camPosPass = glGetUniformLocation(program, "camPos");
		//glUniformMatrix3fv(camPosPass, 1, GL_FALSE, glm::value_ptr(camPos));
		//if (!wireframe) {
		//	glDrawElements(GL_TRIANGLES, this->ClothIndices.size(), GL_UNSIGNED_INT, 0);
		//}
		//else {
		//	glDrawElements(GL_LINES, this->ClothIndices.size(), GL_UNSIGNED_INT, 0);
		//}
		//glDisable(GL_CULL_FACE);
		//glDisable(GL_BLEND);

		////Clearing the vertex array
		//glBindVertexArray(0);
		//glUseProgram(0);

	}

	vector<vector<float>> collisionInfo;
	glm::vec3 position = glm::vec3(0.0f, -150.0f, 0.0f);
	bool wireframe = true;
private:
	std::string name = "Untitled Cloth";
	glm::mat4 model;
	glm::mat4 projCalc;
	glm::mat4 rotationZ;

	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 size = glm::vec2(1.0f, 1.0f);
	glm::vec3 rotationAxisZ = glm::vec3(1.0f, 0.0f, 0.0f);

	vector<unsigned char> rawData;
	vector<float> heightInfo;
	vector<GLfloat> ClothVertices;
	vector<GLuint>  ClothIndices;
	vector<glm::vec3>  ClothNormals;
	vector<vector<ClothNode*>> clothNodes;
	vector<ClothConstraint*> clothConstraints;

	Camera* camera = nullptr;
	GLuint VAO = NULL;
	GLuint VBO = NULL;
	GLuint EBO = NULL;
	GLuint texture = NULL;
	GLuint image = NULL;
	GLuint program = NULL;

	float rotationAngle = 0;
	

};