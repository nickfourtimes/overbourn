/**
 * OpeningScreen.h
 *
 * Author: nrudzicz
 * Created on: Jun 27, 2009
 */

#pragma once

#include <SDL.h>
#include <SDL_mixer.h>

#include "engine/GameEngine.h"
#include "engine/GameScreen.h"
#include "hud/HUD.h"


enum class OpeningState { TEXT, PRESENT, TITLE };


class OpeningScreen : public GameScreen {
private:
	GameEngine* m_engine;
	HUD* m_hud;

	SDL_Surface* m_mainSurface;
	SDL_Surface* m_presentSurface;
	SDL_Surface* m_titleSurface;

	int fullscreenx, fullscreeny;
	SDL_Rect screenRect;

	Mix_Chunk* windChunk;
	int windChannel;

	OpeningState state;

protected:
	OpeningScreen();

public:
	virtual ~OpeningScreen();
	static OpeningScreen* Instance();

	void Init();
	void Clean();

	void Process(Uint32, SDL_Event&);
	void Update(Uint32);
	void Render(Uint32);
};
