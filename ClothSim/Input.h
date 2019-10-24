#pragma once

class Input 
{

public:
	Input();
	~Input();
	bool CheckKeyDownFirst(int key);
	bool CheckKeyDown(int key);
	bool CheckKeyDownS(int key);
	bool firstDown = true;
	bool firstUp = false;
	static void KeyboardDown(unsigned char key, int x, int y);
	static void KeyboardUp(unsigned char key, int x, int y);
	static void specialCharDown(int key, int x, int y);
	static void specialCharUp(int key, int x, int y);
	int m_inputdelay(Input& input);
	int m_delay = 0;
};
