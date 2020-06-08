#ifndef MAINGAME_H_
#define MAINGAME_H_

#include <SDL.h>

#include "engine/Storyteller.h"
#include "hud/HUD.h"


class MainGame : public GameScreen {
private:
	SDL_Surface* mainSurface;

	int fullscreenx, fullscreeny;
	SDL_Rect screenRect;

	HUD* hud;
	Storyteller* storyteller;

protected:
	MainGame() {}

public:
	virtual ~MainGame() {}
	static MainGame* Instance();

	void Init();
	void Clean();

	void Process(Uint32, SDL_Event&);
	void Update(Uint32);
	void Render(Uint32);
};

#endif /*MAINGAME_H_*/
