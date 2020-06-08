#include <iostream>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>

#include "engine/GameEngine.h"
#include "screens/MainGame.h"

using namespace std;


/************************************************************************************************** METHODS */

MainGame* MainGame::Instance() {
	static MainGame m_instance;
	return &m_instance;
}


void MainGame::Init() {
	// get the main surface
	mainSurface = GameEngine::Instance()->GetMainSurface();

	if (FULLSCREEN) {
		fullscreenx = GameEngine::Instance()->GetScreenInfo()->current_w / 2 - WINDOW_SIZE / 2;
		fullscreeny = GameEngine::Instance()->GetScreenInfo()->current_h / 2 - WINDOW_SIZE / 2;
		screenRect.x = fullscreenx;
		screenRect.y = fullscreeny;
	}

	hud = HUD::Instance();

	storyteller = Storyteller::Instance();
	storyteller->Init();

	return;
}


void MainGame::Clean() {
	return;
}


void MainGame::Process(Uint32 ticks, SDL_Event& event) {
	storyteller->Process(ticks, event);
	return;
}


void MainGame::Update(Uint32 ticks) {
	storyteller->Update(ticks);
	return;
}


void MainGame::Render(Uint32 ticks) {
	// clear the main surface
	SDL_FillRect(mainSurface, NULL, 0);

	// get a SDL_Surface with the drawn scene
	SDL_Surface* sceneimage = storyteller->DrawSceneSurface();
	if (sceneimage == NULL) {
		cout << "ERROR: Storyteller returns null surface!" << endl;
		exit(RETURN_ERROR);
	}

	// blit the scene surface to the main screen
	if (SDL_BlitSurface(sceneimage, NULL, mainSurface, FULLSCREEN ? &screenRect : NULL) != 0) {
		cout << "ERROR: Blitting scene image!" << endl;
		exit(RETURN_ERROR);
	}

	// get the HUD, if anything is available
	if (hud->IsDisplaying()) {
		SDL_Surface* mask = hud->GetMaskSurface();

		// blit the translucent mask, so that the screen darkens
		if (SDL_BlitSurface(mask, NULL, mainSurface, FULLSCREEN ? &screenRect : NULL) != 0) {
			cout << "ERROR: Blitting MASK image!" << endl;
			exit(RETURN_ERROR);
		}

		SDL_Surface* hudText = hud->Front()->GetMessageSurface();
		SDL_Rect hudRect(*(hud->Front()->GetScreenPos()));	// we're creating this here, because we might alter it below
		if (FULLSCREEN) {
			// the current HUDMessage should have a text surface, and a position at which to place it
			hudRect.x += fullscreenx;
			hudRect.y += fullscreeny;
		}

		// blit the hud text over the main surface
		if (SDL_BlitSurface(hudText, NULL, mainSurface, &hudRect) != 0) {
			cout << "ERROR: Blitting HUD text!" << endl;
			exit(RETURN_ERROR);
		}
	}

	// update the whole screen
	if (SDL_Flip(mainSurface) != 0) {
		cout << "ERROR: Rendering main screen!" << endl;
		exit(RETURN_ERROR);
	}

	return;
}
