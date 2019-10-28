#include "Graphics.h"

ScreenInfo screen;
Cloth* cloth;
Terrain* terrian;
Camera* camera;
Model* tank;

Input input;

//Text
TextLabel text;


float deltaTime = 0;
float currentTime = 0;
float pasttime = 0;

glm::vec3 backColor = glm::vec3(1, 0, 0);

bool goingup = true;

void FlashRed(glm::vec3* inColor, float deltaTime) {
	float increase = deltaTime;
	if (goingup) {
		if (inColor->x >= 1) {
			goingup = !goingup;
		}
		else {
			inColor->x += increase;
		}
	}
	else {
		if (inColor->x <= 0) {
			goingup = !goingup;
		}
		else {
			inColor->x -= increase;
		}
	}
}

void Render() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(-6.5, 6, -9.0f);
	glRotatef(25, 0, 1, 0);

	glClearColor(backColor.x, backColor.y, backColor.z, 1.0);

	cloth->Render(camera);

	//terrian->Render(camera);

	tank->Render();

	text.Render();

	glutSwapBuffers();
}

void Update() {
	//Set deltaTime
	currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME));
	deltaTime = ((currentTime - pasttime) * 0.001f);
	pasttime = currentTime;
	
	//Object Ticks

	camera->Tick(screen, deltaTime);
	//FlashRed(&backColor, deltaTime);

	cloth->Tick(deltaTime);

	//Input
	float speed = 5;
	if (input.CheckKeyDown(87)) { //W
		camera->camPos.z += speed * deltaTime;
	}
	if (input.CheckKeyDown(83)) { //S
		camera->camPos.z -= speed * deltaTime;
	}
	if (input.CheckKeyDown(65)) { //A
		camera->camPos.x -= speed * deltaTime;
	}
	if (input.CheckKeyDown(68)) { //D
		camera->camPos.x += speed * deltaTime;
	}
	if (input.CheckKeyDown(82)) { //R
		camera->camPos.y += speed * deltaTime;
	}
	if (input.CheckKeyDown(70)) { //F
		camera->camPos.y -= speed * deltaTime;
	}
	if (input.CheckKeyDown(88)) { //X
		cloth->wireframe = true;
	}
	if (input.CheckKeyDown(90)) { //Z
		cloth->wireframe = false;
	}

	//terrian sinks
	terrian->position.y -= 1.0f * deltaTime;

	Render();
}

void InitializeOpenGL(int argc, char* argv[]) {
	Console_OutputLog(L"Initalizing OpenGL", LOGINFO);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowPosition(100, 50);
	glutInitWindowSize((int)screen.SCR_WIDTH, (int)screen.SCR_HEIGHT);
	glutCreateWindow("Cloth Simulator");

	if (glewInit() != GLEW_OK) {
		Console_OutputLog(L"Glew INIT FAILED! The program cannot recover from this error", LOGFATAL);
		system("pause");
		exit(0);
	}

	glutSetOption(GLUT_MULTISAMPLE, 16);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	glClearColor(1.0, 0.0, 0.0, 1.0);

	/*
			===============================================
			// CREATE, INITALISE AND ASSIGN GAME OBJECTS //
			===============================================
	*/


	/*
			============
			// CAMERA //
			============
	*/
	camera = new Camera();
	camera->initializeCamera();
	camera->SwitchMode(camera->CONTROL, glm::vec3(0,0,0), glm::vec3(0,0,1), glm::vec3(0,0,1), 1, 1);

	/*
			===========
			// TERRIAN //
			===========
	*/

	terrian = new Terrain();
	terrian->Initalise(camera, "mountain.raw", "Terrian");

	/*
			===========
			// CLOTH //
			===========
	*/

	cloth = new Cloth();
	cloth->Initalise(camera, glm::vec2(100,100), "Cloth");
	/*
			==========
			// TANK //
			==========
	*/
	tank = new Model("Resources/Models/Tank/Tank.obj", camera, "Tank", 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), "Resources/Shaders/3DObject_Diffuse.vs", "Resources/Shaders/3DObject_BlinnPhong.fs");
	
	/*
			==========
			// TEXT //
			==========
	*/

	text = TextLabel(screen, "This is a test", "Resources/Fonts/TerminusTTF-4.47.0.ttf", glm::vec2(screen.SCR_WIDTH * -0.48,screen.SCR_HEIGHT * 0.43));
	text.SetScale(static_cast<GLfloat>(1.0));

	//Start The Game

	glutDisplayFunc(Render);

	glutIdleFunc(Update);

	glutKeyboardFunc(Input::KeyboardDown);
	glutKeyboardUpFunc(Input::KeyboardUp);

	glutSpecialFunc(Input::specialCharDown);
	glutSpecialUpFunc(Input::specialCharUp);

	Console_OutputLog(L"Game Assets Initalised. Starting Game...", LOGINFO);

	glutMainLoop();

	return;
}

