/**
 * main.c
 *
 * Author: nrudzicz
 * Created on: 9-May-2009
 */

#include <iostream>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>
#include <SDL_mixer.h>

#include "common.h"
#include "engine/GameEngine.h"
#include "engine/Timer.h"
#include "screens/OpeningScreen.h"

using namespace std;


/************************************************************************************************** HELPERS */

int InitSDL(GameEngine* gameEngine) {
	// initialise SDL
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		return RETURN_ERROR;
	}

	// create window
	int sz = FULLSCREEN ? 0 : WINDOW_SIZE;
	if (gameEngine->CreateMainSDLSurface(sz, sz, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF | (FULLSCREEN ? SDL_FULLSCREEN : 0)) == NULL) {
		return RETURN_ERROR;
	}

	SDL_ShowCursor(SDL_DISABLE);

	// set window title
	SDL_WM_SetCaption("Nightmare in Overbourn Marshes", NULL);

	// try to set up the sound system
	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0) {
		cout << "ERROR: Initialising audio!" << endl;
		cout << "  Error code: " << Mix_GetError() << endl;
		return RETURN_ERROR;
	}

	return RETURN_SUCCESS;
}


/************************************************************************************************** MAIN */

#undef main
int main(int argc, char** argv) {
	srand(time(NULL));
	atexit(SDL_Quit);

	// get and initialise our SDL engine
	GameEngine* gameEngine = GameEngine::Instance();
	if (InitSDL(gameEngine) == RETURN_ERROR) {
		cout << "ERROR: Could not initialise SDL!" << endl;
		return RETURN_ERROR;
	}

	// push our starting screen
	OpeningScreen::Instance()->Init();
	gameEngine->PushScreen(OpeningScreen::Instance());

	Timer fpsRegulator;
	SDL_Event event;
	bool quit = false;
	while (!quit) {
		fpsRegulator.Start();

		// check for new events
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:		// window quit, i.e., hitting the X in the window bar
				quit = true;
				break;

			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					quit = true;
					break;
				}
			}

			gameEngine->Process(0, event);
		}

		// update everything in the current screens
		gameEngine->Update(fpsRegulator.GetTicks());

		// render
		gameEngine->Render(0);

		// cap framerate
		if (fpsRegulator.GetTicks() < 1000 / MAX_FRAME_RATE) {
			SDL_Delay(1000 / MAX_FRAME_RATE - fpsRegulator.GetTicks());
		}

	} // while(!quit)

	return RETURN_SUCCESS;
}
