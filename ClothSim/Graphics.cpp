#include "Graphics.h"

ScreenInfo screen;

float deltaTime = 0;
float currentTime = 0;
float pasttime = 0;

void Render() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	glutSwapBuffers();
}

void Update() {
	//Set deltaTime
	currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME));
	deltaTime = ((currentTime - pasttime) * 0.001f);
	pasttime = currentTime;
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
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glClearColor(1.0, 0.0, 0.0, 1.0);

	/*
			===============================================
			// CREATE, INITALISE AND ASSIGN GAME OBJECTS //
			===============================================
	*/

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