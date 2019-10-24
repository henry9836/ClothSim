#include "Sprite.h"

//Set up values for a sprite to be used ingame
void Sprite::Initalise(glm::vec3 _position, glm::vec3 _scale, std::string textureFilePath, std::string vShaderFilePath, std::string fShaderFilePath, GLuint Indices[], GLfloat Verts[], std::string _name)
{

	Console_OutputLog(to_wstring("Initalising Sprite: " + _name), LOGINFO);

	try
	{
		this->position = _position;
		this->scale = _scale;
		this->name = _name;

		//Texture

		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);

		int width, height;

		unsigned char* image = SOIL_load_image(textureFilePath.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
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

		//Program

		this->program = ShaderLoader::CreateProgram(vShaderFilePath.c_str(), fShaderFilePath.c_str());

		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glGenBuffers(1, &this->EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);

		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(GLfloat),
			(GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(GLfloat),
			(GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(
			2,
			2,
			GL_FLOAT,
			GL_FALSE,
			8 * sizeof(GLfloat),
			(GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		Console_OutputLog(to_wstring("Initalised Sprite: " + name), LOGINFO);
	}
	catch (...) {
		Console_OutputLog(to_wstring("Failed To Initalise Sprite: " + name), LOGWARN);
	}
}

void Sprite::Tick(float rotationAngle, glm::vec3 rotationAxisZ, Camera _Cam) {
	try {
		this->camera = _Cam;
		this->translationMatrix = glm::translate(glm::mat4(), this->position);
		this->rotationZ = glm::rotate(glm::mat4(), glm::radians(rotationAngle), rotationAxisZ);
		this->scaleMatrix = glm::scale(glm::mat4(), this->scale);
		this->model = this->translationMatrix * this->rotationZ * this->scaleMatrix;
		this->projCalc = this->camera.proj * this->camera.view * this->model;
	}
	catch (...) {
		Console_OutputLog(to_wstring("Sprite Failed Tick: " + name), LOGWARN);
	}
}

void Sprite::Render(Camera* cam, CubeMap* _skyBox)
{
	try {
		glUseProgram(this->program);
		GLuint mvpLoc2 = glGetUniformLocation(this->program, "proj_calc");
		glUniformMatrix4fv(mvpLoc2, 1, GL_FALSE, glm::value_ptr(this->projCalc));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glUniform1i(glGetUniformLocation(this->program, "tex"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _skyBox->texID);
		glUniform1i(glGetUniformLocation(program, "skybox"), 1);

		GLuint mvpLoc3 = glGetUniformLocation(this->program, "model");
		glUniformMatrix4fv(mvpLoc3, 1, GL_FALSE, glm::value_ptr(this->model));
		
		glUniform3fv(glGetUniformLocation(this->program, "camPos"), 1, glm::value_ptr(cam->camPos));

		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
	catch (...) {
		Console_OutputLog(to_wstring("Sprite Failed To Render: " + name), LOGWARN);
	}
}
