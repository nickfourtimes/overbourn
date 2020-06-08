/**
 * Player.cpp
 *
 * Author: nrudzicz
 * Created on: 25-May-2009
 */

#include <iostream>

#include "engine/Storyteller.h"
#include "sceneobjects/entities/Player.h"

using namespace std;


/************************************************************************************************** {CON|DE}STRUCTORS */

Player::Player() {
	moveUp = moveDown = moveLeft = moveRight = false;

	// create the surface to which we'll draw
	drawingSurface = NULL;
	CreateDrawingSurface();

	return;
}


Player::~Player() {
	SDL_FreeSurface(drawingSurface);
	return;
}


/************************************************************************************************** METHODS */

void Player::CreateDrawingSurface() {
	// create a surface with properties that match the main display surface
	int sz = DRAWN_TILE_SIZE;
	SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE, sz, sz, 32, RMASK, GMASK, BMASK, AMASK);
	drawingSurface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	if (drawingSurface == NULL) {
		cout << "ERROR: Creating Player drawing surface: " << SDL_GetError() << endl;
	}

	// colour the player red
	SDL_FillRect(drawingSurface, NULL, SDL_MapRGB(drawingSurface->format, 71, 49, 49));

	// decorate the player with a dot in the middle
	SDL_Rect rect;
	rect.x = rect.y = 2;
	rect.w = rect.h = 4;
	SDL_FillRect(drawingSurface, &rect, SDL_MapRGB(drawingSurface->format, 0, 0, 0));

	return;
}


bool Player::IsDirty() {
	bool ret = dirty;
	dirty = false;
	return ret;
}


void Player::Process(Uint32 ticks, SDL_Event& event) {
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_UP:
			moveUp = true;
			break;
		case SDLK_DOWN:
			moveDown = true;
			break;
		case SDLK_LEFT:
			moveLeft = true;
			break;
		case SDLK_RIGHT:
			moveRight = true;
			break;
		default:
			break;
		}
		break;

	case SDL_KEYUP:
		switch (event.key.keysym.sym) {
		case SDLK_UP:
			moveUp = false;
			break;
		case SDLK_DOWN:
			moveDown = false;
			break;
		case SDLK_LEFT:
			moveLeft = false;
			break;
		case SDLK_RIGHT:
			moveRight = false;
			break;
		default:
			break;
		}
		break;
	}

	return;
}


void Player::Update(Uint32 ticks) {
	Uint32 time = SDL_GetTicks();
	Uint32 numTilesPerSide = map->GetNumTilesPerSide();

	if (moveUp) {
		if (map->GetUpTile(pos)->IsTraversable()) {
			if (time - lastMoveUp >= MOVE_REPEAT) {
				lastMoveUp = time;
				pos.SetPos(pos.X(), (pos.Y() - 1 + numTilesPerSide) % numTilesPerSide);
				dirty = true;
			}
		} else {
			moveUp = false;
		}
	} else if (moveDown) {
		if (map->GetDnTile(pos)->IsTraversable()) {
			if (time - lastMoveDown >= MOVE_REPEAT) {
				lastMoveDown = time;
				pos.SetPos(pos.X(), (pos.Y() + 1 + numTilesPerSide) % numTilesPerSide);
				dirty = true;
			}
		} else {
			moveDown = false;
		}
	}

	if (moveLeft) {
		if (map->GetLtTile(pos)->IsTraversable()) {
			if (time - lastMoveLeft >= MOVE_REPEAT) {
				lastMoveLeft = time;
				pos.SetPos((pos.X() - 1 + numTilesPerSide) % numTilesPerSide, pos.Y());
				dirty = true;
			}
		} else {
			moveLeft = false;
		}
	} else if (moveRight) {
		if (map->GetRtTile(pos)->IsTraversable()) {
			if (time - lastMoveRight >= MOVE_REPEAT) {
				lastMoveRight = time;
				pos.SetPos((pos.X() + 1 + numTilesPerSide) % numTilesPerSide, pos.Y());
				dirty = true;
			}
		} else {
			moveRight = false;
		}
	}

	return;
}


void Player::Render(Uint32 ticks) {
	return;
}


SDL_Surface* Player::GetAgentSurface() {
	return drawingSurface;
}
