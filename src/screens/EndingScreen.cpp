/**
 * EndingScreen.cpp
 *
 * Author: nrudzicz
 * Created on: Jun 27, 2009
 */

#include <iostream>

#include "common.h"
#include "engine/GameEngine.h"
#include "screens/EndingScreen.h"
#include "screens/MainGame.h"

using namespace std;


/************************************************************************************************** {CON|DE}STRUCTORS */

EndingScreen::EndingScreen() {
	if (FULLSCREEN) {
		fullscreenx = GameEngine::Instance()->GetScreenInfo()->current_w / 2 - WINDOW_SIZE / 2;
		fullscreeny = GameEngine::Instance()->GetScreenInfo()->current_h / 2 - WINDOW_SIZE / 2;
	}
	return;
}


EndingScreen::~EndingScreen() {
	return;
}


/************************************************************************************************** METHODS */

EndingScreen* EndingScreen::Instance() {
	static EndingScreen instance;
	return &instance;
}


void EndingScreen::Init() {
	engine = GameEngine::Instance();
	mainSurface = engine->GetMainSurface();
	hud = HUD::Instance();

	// push our reset messages
	vector<const char*> msg;
	msg.push_back("With a start I awoke.");
	msg.push_back("While my Nightmare had");
	msg.push_back("seemed without end, the");
	msg.push_back("stars above had");
	msg.push_back("scarcely moved.");
	hud->PushMessage(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	msg.push_back("That fevered dream had");
	msg.push_back("passed in an instant.");
	msg.push_back("The Overbourn Marshes");
	msg.push_back("would delay me");
	msg.push_back("a long time yet...");
	hud->PushMessage(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	// load the wind loop and start playing it right away
	windChunk = Mix_LoadWAV("assets/sounds/WindLoop.ogg");
	if (!windChunk) {
		cout << "ERROR: Could not open wind sound file!" << endl;
		cout << "  Error code: " << Mix_GetError() << endl;
	}

	if ((windChannel = Mix_FadeInChannel(-1, windChunk, -1, 500)) == -1) {
		cout << "ERROR: Could not play wind file!" << endl;
		cout << "  Error code: " << Mix_GetError() << endl;
	}

	return;
}


void EndingScreen::Clean() {
	Mix_FreeChunk(windChunk);
	return;
}


void EndingScreen::Process(Uint32 ticks, SDL_Event& event) {
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


void EndingScreen::Update(Uint32 ticks) {
	return;
}


void EndingScreen::Render(Uint32 ticks) {
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
		cout << "ERROR: Rendering ending screen!" << endl;
		exit(RETURN_ERROR);
	}

	// if the HUD has stopped displaying messages, we're done
	if (!hud->IsDisplaying()) {
		Mix_FadeOutChannel(windChannel, 1000);
		SDL_Delay(1000);
		exit(RETURN_SUCCESS);
	}

	return;
}
