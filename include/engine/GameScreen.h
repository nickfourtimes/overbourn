#pragma once

#include <SDL.h>

class GameScreen {
private:
protected:
public:
	GameScreen();
	virtual ~GameScreen();

	virtual void Init() = 0;
	virtual void Clean() = 0;

	virtual void Process(Uint32, SDL_Event&) = 0;
	virtual void Update(Uint32) = 0;
	virtual void Render(Uint32) = 0;
};
