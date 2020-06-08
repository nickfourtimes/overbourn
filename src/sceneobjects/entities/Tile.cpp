/**
 * Tile.cpp
 *
 * Author: nrudzicz
 * Created on: 23-May-2009
 */

#include "common.h"
#include "engine/Storyteller.h"
#include "sceneobjects/entities/Tile.h"

const int NEARWATER_DARKEN = 20;

static vector<HUDMessage*> LAND_MESSAGES;
static vector<HUDMessage*> WATER_MESSAGES;


/************************************************************************************************** {CON|DE}STRUCTORS */

Tile::Tile(TileType tt) : m_tileType(tt) {
	m_hud = HUD::Instance();

	// the first available tile should create the appropriate messages
	if (LAND_MESSAGES.size() == 0 || WATER_MESSAGES.size() == 0) {
		InitialiseStaticMessages();
	}

	// create our drawing surface
	CreateDrawingSurface();

	// assign a random HUDMessage based on our type
	int ind;
	switch (m_tileType) {
	case TileType::LAND:
		ind = rand() % LAND_MESSAGES.size();
		m_tileMessage = LAND_MESSAGES[ind];
		break;
	case TileType::WATER:
		ind = rand() % WATER_MESSAGES.size();
		m_tileMessage = WATER_MESSAGES[ind];
		break;
	}

	// initialise the other variables
	m_prop = NULL;
	traversable = true;
	visited = false;
	nearWater = false;
	forgotten = false;

	return;
}


Tile::~Tile() {
	SDL_FreeSurface(m_surface);
	delete m_prop;
	return;
}


/************************************************************************************************** PRIVATE HELPERS */

void getTileColours(TileType type, Uint8& r, Uint8& g, Uint8& b) {
	switch (type) {
	case TileType::LAND:
		switch (rand() % 5) {
		case 0:
			r = 71; g = 97; b = 41;
			break;
		case 1:
			r = 74; g = 104; b = 39;
			break;
		case 2:
			r = 61; g = 83; b = 35;
			break;
		case 3:
			r = 74; g = 102; b = 42;
			break;
		case 4:
			r = 69; g = 90; b = 44;
			break;
		}
		break;
	case TileType::WATER:
		switch (rand() % 2) {
		case 0:
			r = 50;
			g = 64;
			b = 94;
			break;
		case 1:
			r = 45;
			g = 59;
			b = 89;
			break;
		}
		break;
	}

	return;
}


/************************************************************************************************** METHODS */

void Tile::InitialiseStaticMessages() {
	vector<const char*> msg;

	// land messages
	msg.push_back("Grasses and spongy mosses");
	msg.push_back("cover the ground.");
	LAND_MESSAGES.push_back(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	msg.push_back("The ground teems with");
	msg.push_back("plant life and the");
	msg.push_back("thrum of insects.");
	LAND_MESSAGES.push_back(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	// water message
	msg.push_back("Dark, brackish waters");
	msg.push_back("conceal the depths below.");
	WATER_MESSAGES.push_back(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	msg.push_back("The stagnant waters");
	msg.push_back("of the Overbourn.");
	WATER_MESSAGES.push_back(new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM));
	msg.clear();

	return;
}


void Tile::CreateDrawingSurface() {
	SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE, DRAWN_TILE_SIZE, DRAWN_TILE_SIZE, 32, RMASK, GMASK, BMASK, AMASK);
	m_surface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	// fill surface with black until we become visited
	SDL_FillRect(m_surface, NULL, 0);

	return;
}


SDL_Surface* Tile::GetSurface() {
	if (!visited || m_prop == NULL) {
		return m_surface;
	} else {
		return m_prop->GetSurface();
	}
}


void Tile::GiveProp(Prop* p) {
	m_prop = p;
	return;
}


bool Tile::HasProp() {
	return m_prop != NULL;
}


PropType Tile::GetPropType() {
	if (m_prop == NULL) {
		cout << "ERROR: Accessing tile with NULL prop!" << endl;
		exit(RETURN_ERROR);
	} else {
		return m_prop->GetPropType();
	}
}


bool Tile::IsTraversable() {
	return m_prop == NULL;
}


void Tile::DescribeYourself() {
	if (m_prop == NULL) {
		m_hud->PushMessage(new HUDMessage(m_tileMessage));
		return;
	} else {
		m_prop->DescribeYourself();

		// mention if the prop is underwater
		switch (m_tileType) {
		case TileType::LAND:
			break;
		case TileType::WATER:
			if (m_prop->GetPropType() != PropType::MARKER) {
				m_hud->PushMessage(new HUDMessage(vector<const char*>(1, "It is partially submerged."), MsgPlacement::PLACE_BOTTOM));
			}
			break;
		}

		// mention if there are neighbouring props.
		switch (m_prop->GetNumNeighbours()) {
		case 0:
			break;
		case 1:
			m_hud->PushMessage(new HUDMessage(vector<const char*>(1, "Another stands nearby."), MsgPlacement::PLACE_BOTTOM));
			break;
		case 2:
			m_hud->PushMessage(new HUDMessage(vector<const char*>(1, "Two others stand nearby."), MsgPlacement::PLACE_BOTTOM));
			break;
		}

		return;
	}
}


void Tile::Visit() {
	if (!visited && !forgotten) {
		Uint8 red, green, blue;
		getTileColours(m_tileType, red, green, blue);
		if (m_tileType == TileType::LAND && nearWater) {
			red -= NEARWATER_DARKEN;
			green -= NEARWATER_DARKEN;
			blue -= NEARWATER_DARKEN;
		}

		SDL_FillRect(m_surface, NULL, SDL_MapRGB(m_surface->format, red, green, blue));
	}

	visited = true;

	return;
}


void Tile::SetNearWater(bool b) {
	nearWater = b;
	return;
}


void Tile::Forget() {
	forgotten = true;
	SDL_FillRect(m_surface, NULL, SDL_MapRGB(m_surface->format, 0, 0, 0));
	return;
}
