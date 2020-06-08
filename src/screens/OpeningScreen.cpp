/**
 * OpeningScreen.cpp
 *
 * Author: nrudzicz
 * Created on: Jun 27, 2009
 */

#include <iostream>

#include <SDL_image.h>

#include "common.h"
#include "screens/OpeningScreen.h"
#include "screens/MainGame.h"

using namespace std;


/************************************************************************************************** {CON|DE}STRUCTORS */

OpeningScreen::OpeningScreen() {
	m_presentSurface = IMG_Load("assets/images/NickPresents.png");
	if (!m_presentSurface) {
		cout << "ERROR: Opening Screen could not load NickPresents.png!" << endl;
		cout << "  Error code " << IMG_GetError() << endl;
		exit(RETURN_ERROR);
	}

	m_titleSurface = IMG_Load("assets/images/NightmareTitle.png");
	if (!m_titleSurface) {
		cout << "ERROR: Opening Screen could not load title image!" << endl;
		cout << "  Error code " << IMG_GetError() << endl;
		exit(RETURN_ERROR);
	}

	return;
}


OpeningScreen::~OpeningScreen() {
	SDL_FreeSurface(m_mainSurface);
	SDL_FreeSurface(m_presentSurface);
	SDL_FreeSurface(m_titleSurface);
	Mix_FreeChunk(windChunk);
	return;
}


/************************************************************************************************** METHODS */

OpeningScreen* OpeningScreen::Instance() {
	static OpeningScreen instance;
	return &instance;
}


void OpeningScreen::Init() {
	m_engine = GameEngine::Instance();
	m_mainSurface = m_engine->GetMainSurface();
	m_hud = HUD::Instance();

	// push our reset messages
	vector<const char*> msg;
	msg.push_back("My journeys had taken me");
	msg.push_back("over hill and plain;");
	msg.push_back("my Road had lain through");
	msg.push_back("mountain hall, and");
	msg.push_back("forests thick with dread.");
	m_hud->PushMessage(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	msg.push_back("Alone in the wilderness");
	msg.push_back("or among the realms of");
	msg.push_back("Men and other fair Folk,");
	msg.push_back("I travelled without rest.");
	m_hud->PushMessage(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	msg.push_back("One night, as my Road");
	msg.push_back("turned south, I wearily");
	msg.push_back("made camp beneath the");
	msg.push_back("stars, in the realm of the");
	msg.push_back("Overbourn Marshes.");
	m_hud->PushMessage(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	msg.push_back("There my Nightmare began.");
	m_hud->PushMessage(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	state = OpeningState::TEXT;

	if (FULLSCREEN) {
		fullscreenx = GameEngine::Instance()->GetScreenInfo()->current_w / 2 - WINDOW_SIZE / 2;
		fullscreeny = GameEngine::Instance()->GetScreenInfo()->current_h / 2 - WINDOW_SIZE / 2;
	}

	screenRect.x += fullscreenx;
	screenRect.y += fullscreeny;

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


void OpeningScreen::Clean() {
	return;
}


void OpeningScreen::Process(Uint32 ticks, SDL_Event& event) {
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_RETURN:
		case SDLK_SPACE:
			// are we showing some story text? or the "X presents"? or the title screen?
			switch (state) {
			case OpeningState::TEXT:
				m_hud->PopMessage();
				if (!m_hud->IsDisplaying()) {
					state = OpeningState::PRESENT;
				}
				break;
			case OpeningState::PRESENT:
				state = OpeningState::TITLE;
				break;
			case OpeningState::TITLE:
				Mix_FadeOutChannel(windChannel, 750);
				SDL_Delay(1500);
				m_engine->PopScreen();
				m_engine->PushScreen(MainGame::Instance());
				MainGame::Instance()->Init();
				break;
			}
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


void OpeningScreen::Update(Uint32 ticks) {
	return;
}


void OpeningScreen::Render(Uint32 ticks) {
	// clear the main surface
	SDL_FillRect(m_mainSurface, NULL, 0);

	switch (state) {
	case OpeningState::TEXT:	// get the HUD, if anything is available
	{
		// the current HUDMessage should have a text surface, and a position at which to place it
		SDL_Surface* hudText = m_hud->Front()->GetMessageSurface();
		SDL_Rect hudRect(*(m_hud->Front()->GetScreenPos()));	// we're creating this here, because we might alter it below
		if (FULLSCREEN) {
			// the current HUDMessage should have a text surface, and a position at which to place it
			hudRect.x += fullscreenx;
			hudRect.y += fullscreeny;
		}

		if (SDL_BlitSurface(hudText, NULL, m_mainSurface, &hudRect) != 0) {
			cout << "ERROR: Blitting HUD text!" << endl;
			exit(RETURN_ERROR);
		}
		break;
	}

	case OpeningState::PRESENT:
	{
		if (SDL_BlitSurface(m_presentSurface, NULL, m_mainSurface, FULLSCREEN ? &screenRect : NULL) != 0) {
			cout << "ERROR: Blitting 'presenting' surface!" << endl;
			exit(RETURN_ERROR);
		}
		break;
	}

	case OpeningState::TITLE:
	{
		if (SDL_BlitSurface(m_titleSurface, NULL, m_mainSurface, FULLSCREEN ? &screenRect : NULL) != 0) {
			cout << "ERROR: Blitting 'presenting' surface!" << endl;
			exit(RETURN_ERROR);
		}
		break;
	}

	}	// end switch(state)

	// update the whole screen
	if (SDL_Flip(m_mainSurface) != 0) {
		cout << "ERROR: Rendering main screen!" << endl;
		exit(RETURN_ERROR);
	}

	// if the HUD has stopped displaying messages, we can flip back to the game
	if (state == OpeningState::TEXT && !m_hud->IsDisplaying()) {
		state = OpeningState::PRESENT;
	}

	return;
}
