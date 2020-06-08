#pragma once

#include <SDL.h>

#include "engine/GameScreen.h"

class ScreenInfo {
public:
	Uint32 current_w, current_h;
};


class GameEngine {
private:
	// singleton
	static GameEngine* m_instance;

	SDL_Surface* m_sdlSurface;

protected:

public:
	GameEngine();
	virtual ~GameEngine();

	// singleton
	static GameEngine* Instance();

	SDL_Surface* CreateMainSDLSurface(int width, int height, int depth, Uint32 flags);

	SDL_Surface* GetMainSurface();

	void PushScreen(GameScreen* screen);
	void PopScreen();

	void Process(Uint32 code, SDL_Event event);
	void Update(Uint32 ticks);
	void Render(Uint32 code);

	ScreenInfo* GetScreenInfo();
};
