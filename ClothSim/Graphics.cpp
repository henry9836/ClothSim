#include "Graphics.h"

Cloth* cloth;
Plane* ground;
Sphere* sphere;

glm::vec2 clothSize = glm::vec2(50, 50);
glm::vec2 windowSize = glm::vec2(1280, 720);
glm::vec2 mousePos = glm::vec2(0, 0);
glm::vec3 rayDirection = glm::vec3(1, 0, 0);

float deltaTime = 0;
float currentTime = 0;
float pasttime = 0;
float camAngle = 0;
float camDistance = -60.0f;
//Cloth effects

float windAmp = 1.0f;

bool gravity = true;
bool wind = false;
bool mouseDown = false;

glm::vec3 windDir = glm::vec3(0.0f, 0.5f, 0.5f);


void Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(-10, -10, camDistance);
	glRotatef(camAngle, 0, 1, 0);

	ground->Render();
	cloth->Render();
	sphere->Render();

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
	if (wind) {
		cloth->ApplyWind(windDir, windAmp);
	}

	cloth->CollisionLogic(sphere, rayDirection, glm::vec3(-10, -10, camDistance), mouseDown);

	//Object Ticks

	cloth->Tick(deltaTime);

	Render();
}

void keyboardOther(int key, int x, int y) {
	wcout << L"PRESSED: " << key << endl;

	//Arrow Keys
	if (key == 101) {
		if (windDir.y < 1) {
			windDir.y += 0.1f;
		}
	}
	if (key == 103) {
		if (windDir.y > -1) {
			windDir.y -= 0.1f;
		}
	}
	if (key == 102) {
		if (windDir.x < 1) {
			windDir.x += 0.1f;
		}
	}
	if (key == 100) {
		if (windDir.x > -1) {
			windDir.x -= 0.1f;
		}
	}
}

void keyboardInput(unsigned char key, int x, int y) {
	wcout << L"PRESSED: " << key << endl;

	if (key == 27) { //esc
		glutLeaveMainLoop();
	}
	if (key == 100 || key == 68) { //d (Deattach)
		cloth->AllDyanmic();
	}
	if (key == 103 || key == 71) { //g (Gravity)
		gravity = !gravity;
	}

	if (key == 114 || key == 82) { //r (Reset)
		gravity = true;
		wind = false;
		glm::vec3 windDir = glm::vec3(0.0f, 0.5f, 0.5f);
		windAmp = 1;
		camAngle = 0;
		camDistance = -60.0f;
		cloth->Reset();
	}

	if (key == 119 || key == 97) { //w (Wind)
		wind = !wind;
	}

	if (key == 13) { //Enter Windspeed -
		windAmp -= 1;
	}
	if (key == 92) { // \/ button Windspeed +
		windAmp += 1;
	}

	if (key == 122 || key == 90) {//Z rotate cam left
		camAngle -= 1.0f;
	}

	if (key == 120 || key == 88) {//Z rotate cam right
		camAngle += 1.0f;
	}

	if (key == 111 || key == 79) {//o get closer
		camDistance += 1.0f;
	}

	if (key == 108 || key == 76) {//l get further
		camDistance -= 1.0f;
	}

	//Cloth resizing

	if (key == 121 || key == 89) //y get x smaller
	{
		delete cloth;
		if (clothSize.x > 2) {
			clothSize.x -= 1;
		}
		cloth = new Cloth(clothSize, 1.0f, ground->transform.position.y + 1.0f);
	}
	if (key == 117 || key == 85) //u get x bigger
	{
		delete cloth;
		clothSize.x += 1;
		cloth = new Cloth(clothSize, 1.0f, ground->transform.position.y + 1.0f);
	}
	if (key == 72 || key == 104) //h get y smaller
	{
		delete cloth;
		if (clothSize.y > 2) {
			clothSize.y -= 1;
		}
		cloth = new Cloth(clothSize, 1.0f, ground->transform.position.y + 1.0f);
	}
	if (key == 106 || key == 74) //j get y bigger
	{
		delete cloth;
		clothSize.y += 1;
		cloth = new Cloth(clothSize, 1.0f, ground->transform.position.y + 1.0f);
	}

	//Damping

	if (key == 86 || key == 118) //v less damping
	{
		cloth->AdjustDamping(-0.01f);
	}
	if (key == 66 || key == 98) //b more damping
	{
		cloth->AdjustDamping(0.01f);
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
	windowSize.x = (float)w;
	windowSize.y = (float)h;
	glLoadIdentity();
}

void Raycast() {
	glm::vec2 NormMousePos = glm::vec2((2.0f * mousePos.x) / (float)windowSize.x - 1.0f, 1.0f - (2.0f * mousePos.y) / (float)windowSize.y);
	
	glm::vec2 normalizedScreenPos = NormMousePos;

	glm::vec4 clipCoords = glm::vec4(normalizedScreenPos.x, normalizedScreenPos.y, -1.0f, 1.0f);

	glm::mat4 projMat = glm::perspective(80.0f, (float)(windowSize.x / windowSize.y), 0.1f, 5000.0f);
	glm::mat4 viewMat = glm::lookAt(glm::vec3(-10.0f, -10.0f, camDistance), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	glm::mat4 invProjMat = glm::inverse(projMat);

	glm::vec4 eyeCoords = invProjMat * clipCoords;

	eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

	glm::mat4 invViewMat = glm::inverse(viewMat);

	glm::vec4 rayWorld = invViewMat * eyeCoords;

	rayDirection = glm::normalize((glm::vec3(rayWorld)));

}

void MouseAction(int button, int state, int x, int y) {
	mousePos = glm::vec2(x, y);

	if (button == 0) {
		mouseDown = true;
	}
	else{
		mouseDown = false;
	}
	Raycast();
}


void MouseMovement(int x, int y) {
	mousePos = glm::vec2(x, y);
	Raycast();
}

void InitializeOpenGL(int argc, char* argv[])
{

	Console_OutputLog(L"Initalizing OpenGL...", LOGINFO);

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);

	glutCreateWindow("CLOF");

	glClearColor(0.3f, 0.3f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	Console_OutputLog(L"Creating Objects...", LOGINFO);
	
	ground = new Plane(glm::vec3(0,-25,0), 1000, glm::vec3(0.3, 0.5, 0.3));

	cloth = new Cloth(clothSize, 1.0f, ground->transform.position.y+1.0f);

	sphere = new Sphere(25.0f, glm::vec3(0.25, 0.45, 0.25), glm::vec3(0,-30,0));

	glutDisplayFunc(Render);
	glutIdleFunc(Update);
	glutReshapeFunc(Resize);
	glutMotionFunc(MouseMovement);
	glutMouseFunc(MouseAction);
	glutKeyboardFunc(keyboardInput);
	glutSpecialFunc(keyboardOther);

	Console_OutputLog(L"Game Assets Initalised. Starting Game...", LOGINFO);

	glutMainLoop();

}
