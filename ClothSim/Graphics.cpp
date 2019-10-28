#include "Graphics.h"

Cloth* cloth;

float deltaTime = 0;
float currentTime = 0;
float pasttime = 0;

//Cloth effects

bool gravity = false;

void Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	glTranslatef(-30, -10, -50.0f);
	glRotatef(25, 0, 1, 0);

	cloth->Render();

	glutSwapBuffers();
}

void Update() {

	//Set deltaTime
	currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME));
	deltaTime = ((currentTime - pasttime) * 0.001f);
	pasttime = currentTime;

	//Cloth Effects
	if (gravity) {
		cloth->globalForce(glm::vec3(0, -9.8, 0) * deltaTime);
	}

	//Object Ticks

	cloth->Tick(deltaTime);

	Render();
}

void keyboardInput(unsigned char key, int x, int y) {
	wcout << L"PRESSED: " << key << endl;

	if (key == 27) { //esc
		glutLeaveMainLoop();
	}
	if (key == 100 || key == 68) { //d
		cloth->AllDyanmic();
	}
	if (key == 103 || key == 71) { //g
		gravity = !gravity;
	}
}

void Resize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h == 0)
		gluPerspective(80, (float)w, 1.0, 5000.0);
	else
		gluPerspective(80, (float)w / (float)h, 1.0, 5000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void InitializeOpenGL(int argc, char* argv[])
{

	Console_OutputLog(L"Initalizing OpenGL...", LOGINFO);

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);

	glutCreateWindow("CLOF");

	glClearColor(0.5, 0.0, 0.0, 1.0);

	glEnable(GL_DEPTH_TEST);

	Console_OutputLog(L"Creating Objects...", LOGINFO);

	cloth = new Cloth(glm::vec2(50,50), 1.0f);

	glutDisplayFunc(Render);
	glutIdleFunc(Update);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(keyboardInput);

	Console_OutputLog(L"Game Assets Initalised. Starting Game...", LOGINFO);

	glutMainLoop();

}
