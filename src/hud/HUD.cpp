/**
 * HUD.cpp
 *
 * Author: nrudzicz
 * Created on: 28-May-2009
 */

#include <iostream>

#include "common.h"
#include "hud/HUD.h"

using namespace std;

const int FONT_SIZE = 40;

const int MASK_ALPHA = 75;	// 0 is transparent, 255 is opaque


/************************************************************************************************** {CON|DE}STRUCTORS */

HUD::HUD() {
	// get our main font
	if (TTF_Init() != 0) {
		cout << "ERROR: Initialising TTF: " << TTF_GetError() << endl;
		exit(RETURN_ERROR);
	}
	LoadMainFont();

	return;
}


HUD::~HUD() {
	SDL_FreeSurface(maskSurface);
	return;
}


/************************************************************************************************** METHODS */

void HUD::LoadMainFont() {
	mainFont = TTF_OpenFont("assets/fonts/VeniceClassic/VeniceClassic.ttf", FONT_SIZE);
	if (!mainFont) {
		cout << "ERROR: HUD could not open main font!" << endl;
		exit(RETURN_ERROR);
	}

	return;
}


void HUD::CreateMaskSurface() {
	SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE, WINDOW_SIZE, WINDOW_SIZE, SCREEN_BPP, RMASK, GMASK, BMASK, AMASK);
	SDL_SetAlpha(temp, SDL_SRCALPHA, MASK_ALPHA);
	maskSurface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	return;
}


HUD* HUD::Instance() {
	static HUD m_instance;
	return &m_instance;
}


void HUD::Init() {
	// create our surfaces
	CreateMaskSurface();
	return;
}


bool HUD::IsDisplaying() {
	return messageQueue.size() >= 1;
}


bool HUD::IsLastMessage() {
	return messageQueue.size() == 1;
}


TTF_Font* HUD::GetMainFont() const {
	return mainFont;
}


SDL_Surface* HUD::GetMaskSurface() {
	return maskSurface;
}


void HUD::PushMessage(HUDMessage* msg) {
	messageQueue.push(msg);
	return;
}


HUDMessage* HUD::Front() {
	return messageQueue.front();
}


void HUD::PopMessage() {
	if (messageQueue.empty()) {
		return;
	}

	// get rid of our previous message
	messageQueue.pop();

	return;
}
