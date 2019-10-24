#include <iostream>
#include <freeglut.h>
#include <vector>
#include "Input.h"
#include "ObjectManager.h"
#include "glm.hpp"

using namespace std;

Input _input;

enum InputState
{
	INPUT_UP,
	INPUT_DOWN,
	INPUT_UP_FIRST,
	INPUT_DOWN_FIRST,
};

InputState KeyState[255];
InputState KeyState_S[255];

Input::Input()
{
}

Input::~Input()
{
}

bool Input::CheckKeyDownFirst(int key) {
	if (KeyState[key] == INPUT_DOWN_FIRST)
	{
		return(true);
	}
	return(false);
}

bool Input::CheckKeyDown(int key)
{
	if (KeyState[key] == INPUT_DOWN)
	{
		return(true);
	}
	return(false);
}

bool Input::CheckKeyDownS(int key)
{
	return false;
}

int Input::m_inputdelay(Input& input)
{
	if (_input.m_delay ==1)
	{
		input.m_delay++;
	}
	if (_input.m_delay == 2)
	{
		input.m_delay = 0;

	}
	return (input.m_delay);
}

void Input::KeyboardDown(unsigned char key, int x, int y)
{
	KeyState[key] = INPUT_DOWN;
	_input.m_delay = 1;
}

void Input::KeyboardUp(unsigned char key, int x, int y)
{
	KeyState[key] = INPUT_UP;
	_input.m_delay = 2;
}

void Input::specialCharDown(int key, int x, int y)
{
	KeyState_S[key] = INPUT_DOWN;
}

void Input::specialCharUp(int key, int x, int y)
{
	KeyState_S[key] = INPUT_UP;
}


