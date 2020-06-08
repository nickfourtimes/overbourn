#include <iostream>
#include <vector>

#include "common.h"
#include "hud/HUDMessage.h"
#include "screens/RestartScreen.h"	// order matters for these last two
#include "screens/MainGame.h"

using namespace std;


/************************************************************************************************** METHODS */

RestartScreen* RestartScreen::Instance() {
	static RestartScreen m_screen;
	return &m_screen;
}


void RestartScreen::Init() {
	engine = GameEngine::Instance();
	mainSurface = engine->GetMainSurface();
	hud = HUD::Instance();

	if (FULLSCREEN) {
		fullscreenx = GameEngine::Instance()->GetScreenInfo()->current_w / 2 - WINDOW_SIZE / 2;
		fullscreeny = GameEngine::Instance()->GetScreenInfo()->current_h / 2 - WINDOW_SIZE / 2;
	}

	// push our reset messages
	vector<const char*> msg;
	msg.push_back("Darkness enveloped me.");
	msg.push_back("And then...");
	hud->PushMessage(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	msg.push_back("Once more, I faced the");
	msg.push_back("loneliness of the Marsh.");
	hud->PushMessage(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	return;
}


void RestartScreen::Clean() {
	return;
}


void RestartScreen::Process(Uint32 ticks, SDL_Event& event) {
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_RETURN:
		case SDLK_SPACE:
			hud->PopMessage();
			break;
		default:
			break;
		}
		break;

	case SDL_KEYUP:
		break;
	}

	return;
}


void RestartScreen::Update(Uint32 ticks) {
	return;
}


void RestartScreen::Render(Uint32 ticks) {
	// clear the main surface
	SDL_FillRect(mainSurface, NULL, 0);

	// get the HUD, if anything is available
	if (hud->IsDisplaying()) {
		// the current HUDMessage should have a text surface, and a position at which to place it
		SDL_Surface* hudtext = hud->Front()->GetMessageSurface();
		SDL_Rect rect(*(hud->Front()->GetScreenPos()));

		if (FULLSCREEN) {
			rect.x += fullscreenx;
			rect.y += fullscreeny;
		}

		if (SDL_BlitSurface(hudtext, NULL, mainSurface, &rect) != 0) {
			cout << "ERROR: Blitting HUD text!" << endl;
			exit(RETURN_ERROR);
		}
	}

	// update the whole screen
	if (SDL_Flip(mainSurface) != 0) {
		cout << "ERROR: Rendering main screen!" << endl;
		exit(RETURN_ERROR);
	}

	// if the HUD has stopped displaying messages, we can flip back to the game
	if (!hud->IsDisplaying()) {
		SDL_Delay(500);
		engine->PopScreen();
		engine->PushScreen(MainGame::Instance());
		MainGame::Instance()->Init();
	}

	return;
}
