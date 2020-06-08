/**
 * HUDMessage.cpp
 *
 * Author: nrudzicz
 * Created on: 28-May-2009
 */

#include <iostream>

#include "SDL.h"

#include "common.h"
#include "hud/HUD.h"
#include "hud/HUDMessage.h"

using namespace std;

const int LINE_SPACING = 5;		// in pixels
const int MSG_LEFT_SPACING = 20;


/************************************************************************************************** {CON|DE}STRUCTORS */

HUDMessage::HUDMessage(vector<const char*> msgs, MsgPlacement place) : msgVect(msgs), placement(place) {
	//if(msgVect.size() == 1) {
	//	PrepareSingleLineSurface();
	//} else {
	PrepareMultiLineSurface();
	//}
	return;
}


HUDMessage::HUDMessage(HUDMessage* hm) {
	msgVect = hm->msgVect;
	msgSurface = hm->msgSurface;
	placement = hm->placement;
	screenPos = hm->screenPos;
	return;
}


HUDMessage::~HUDMessage() {
	// DO NOT FREE SDL SURFACE HERE!!!
	// HUDMessage copies share pointers to the SDL_Surface (to avoid duplication).
	// Since the HUD queue destroys popped messages, we must not erase the original surface here.
	// Use DestroyMessage() when finished.
	return;
}


/************************************************************************************************** METHODS */

void HUDMessage::PrepareMultiLineSurface() {
	HUD* hud = HUD::Instance();

	// create each individual line's surface, and get maximal dimensions
	SDL_Surface** surfaces = new SDL_Surface * [msgVect.size()];
	int maxwidth = -1;
	for (int i = 0; i < (int)msgVect.size(); ++i) {
		surfaces[i] = TTF_RenderText_Blended(hud->GetMainFont(), msgVect[i], SDL_CLR_WHITE);
		if (!surfaces[i]) {
			cout << "HUDMESSAGE ERROR: Could not create message \"" << msgVect[i] << "\"!" << endl;
			exit(RETURN_ERROR);
		}

		if (surfaces[i]->w > maxwidth) {
			maxwidth = surfaces[i]->w;
		}
	}

	// create a surface big enough for each message
	SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE, maxwidth, msgVect.size() * TTF_FontLineSkip(hud->GetMainFont()), SCREEN_BPP, RMASK, GMASK, BMASK, AMASK);
	msgSurface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	if (NULL == msgSurface) {
		cout << "Error display format alpha: " << SDL_GetError() << endl;
	}

	Uint32 transparentclr = SDL_MapRGB(msgSurface->format, 0, 0, 0);
	SDL_FillRect(msgSurface, NULL, transparentclr);
	SDL_SetColorKey(msgSurface, SDL_SRCCOLORKEY, transparentclr);

	// blit each line to its place on the main surface, then free the temps
	SDL_Rect rect;
	rect.x = 0;
	for (int i = 0; i < (int)msgVect.size(); ++i) {
		rect.y = i * TTF_FontLineSkip(hud->GetMainFont());

		if (SDL_BlitSurface(surfaces[i], NULL, msgSurface, &rect) != 0) {
			cout << "HUDMESSAGE ERROR: Error blitting \"" << msgVect[i] << "\"!" << endl;
			exit(RETURN_ERROR);
		}

		SDL_FreeSurface(surfaces[i]);
	}

	if (!msgSurface) {
		cout << "HUDMESSAGE ERROR: Surface is now blank!" << endl;
		exit(RETURN_ERROR);
	}

	switch (placement) {
	case MsgPlacement::PLACE_LEFT_CENTRE:
		// centre this message vertically
		screenPos.x = MSG_LEFT_SPACING;
		screenPos.y = (WINDOW_SIZE - msgSurface->h) / 2;
		break;
	case MsgPlacement::PLACE_BOTTOM:
		screenPos.x = (WINDOW_SIZE - msgSurface->w) / 2;
		screenPos.y = WINDOW_SIZE - 1 - msgSurface->h - 20;
		break;
	}

	return;
}


SDL_Surface* HUDMessage::GetMessageSurface() {
	return msgSurface;
}


SDL_Rect* HUDMessage::GetScreenPos() {
	return &screenPos;
}


void HUDMessage::SetScreenPos(int x, int y) {
	screenPos.x = x;
	screenPos.y = y;
	return;
}


void HUDMessage::DestroyMessage() {
	SDL_FreeSurface(msgSurface);
	msgVect.clear();
	return;
}
