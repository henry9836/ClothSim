#pragma once
#include <iostream>
#include <fmod.hpp>
#include "ConsoleController.h"

class AudioSystem {
public:

	bool AudioInit();
	void Play(int track);
	void Tick();

	enum Tracks {
		SHOOT,
		HIT,
		AMBIENT
	};

	FMOD::System* audioSystem;
	FMOD::Sound* shoot;
	FMOD::Sound* hit;
	FMOD::Sound* ambient;
};