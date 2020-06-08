#include <iostream>
#include <stdlib.h>

#include "common.h"
#include "engine/GameEngine.h"

using namespace std;


/************************************************************************************************** DATA */

GameEngine* GameEngine::m_instance = NULL;
GameScreen* m_screen;
ScreenInfo* m_screenInfo;


/************************************************************************************************** {CON|DE}STRUCTORS */

GameEngine::GameEngine() {
	return;
}


GameEngine::~GameEngine() {
	return;
}


/************************************************************************************************** METHODS */

GameEngine* GameEngine::Instance() {
	if (!m_instance) {
		m_instance = new GameEngine();
	}

	return m_instance;
}


SDL_Surface* GameEngine::CreateMainSDLSurface(int width, int height, int depth, Uint32 flags) {
	m_sdlSurface = SDL_SetVideoMode(width, height, depth, flags);
	if (m_sdlSurface == NULL) {
		cout << "ERROR: SDL_CreateRGBSurface() failed: " << SDL_GetError();

	} else {
		m_screenInfo = new ScreenInfo();
		m_screenInfo->current_h = m_sdlSurface->h;
		m_screenInfo->current_w = m_sdlSurface->w;
	}

	return m_sdlSurface;
}


SDL_Surface* GameEngine::GetMainSurface() {
	return m_sdlSurface;
}


void GameEngine::PushScreen(GameScreen* screen) {
	m_screen = screen;
	return;
}


void GameEngine::PopScreen() {
	m_screen = NULL;
	return;
}


void GameEngine::Process(Uint32 code, SDL_Event event) {
	m_screen->Process(code, event);
	return;
}


void GameEngine::Update(Uint32 ticks) {
	m_screen->Update(ticks);
	return;
}


void GameEngine::Render(Uint32 code) {
	m_screen->Render(code);
	return;
}


ScreenInfo* GameEngine::GetScreenInfo() {
	return m_screenInfo;
}
