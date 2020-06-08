#pragma once

#include <SDL.h>

#include "engine/GameEngine.h"
#include "engine/GameScreen.h"
#include "hud/HUD.h"


class RestartScreen : public GameScreen {
private:
	GameEngine* engine;
	SDL_Surface* mainSurface;

	int fullscreenx, fullscreeny;

	HUD* hud;

protected:
	RestartScreen() {}

public:
	virtual ~RestartScreen() {}
	static RestartScreen* Instance();

	void Init();
	void Clean();

	void Process(Uint32, SDL_Event&);
	void Update(Uint32);
	void Render(Uint32);
};
