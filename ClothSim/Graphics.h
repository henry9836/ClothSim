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

#include "3D.h"
#include "ConsoleController.h"
#include "ScreenInfo.h"
#include "Input.h"
#include "Camera.h"

void InitializeOpenGL(int argc, char* argv[]);
