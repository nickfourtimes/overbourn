/**
 * EndingScreen.h
 *
 * Author: user
 * Created on: Jun 27, 2009
 */

#pragma once

#include <SDL.h>
#include <SDL_mixer.h>

#include "engine/GameEngine.h"
#include "engine/GameScreen.h"
#include "hud/HUD.h"


class EndingScreen : public GameScreen {
private:
	GameEngine* engine;
	SDL_Surface* mainSurface;

	int fullscreenx, fullscreeny;

	HUD* hud;

	Mix_Chunk* windChunk;
	int windChannel;

protected:
	EndingScreen();

public:
	virtual ~EndingScreen();
	static EndingScreen* Instance();

	void Init();
	void Clean();

	void Process(Uint32, SDL_Event&);
	void Update(Uint32);
	void Render(Uint32);
};
