#include <iostream>

#include "common.h"
#include "engine/Storyteller.h"
#include "sceneobjects/entities/Prop.h"

using namespace std;


/************************************************************************************************** {CON|DE}STRUCTORS */

Prop::Prop(PropType pt, int nn) : type(pt), numNeighbours(nn) {
	hud = HUD::Instance();

	// create the drawing surface
	CreateDrawingSurface();

	// figure out our description
	CreateDescription();

	return;
}


Prop::~Prop() {
	hudMessage->DestroyMessage();
	delete hudMessage;
	return;
}


/************************************************************************************************** METHODS */

void Prop::CreateDrawingSurface() {
	// create a surface with properties that match the main display surface
	int sz = DRAWN_TILE_SIZE;
	SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE, sz, sz, 32, RMASK, GMASK, BMASK, AMASK);
	surface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	if (surface == NULL) {
		cout << "ERROR: Creating Prop drawing surface: " << SDL_GetError() << endl;
	}

	// colour the different props
	switch (type) {
	case PropType::MARKER:
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 207, 207, 207));
		break;
	case PropType::TOADSTOOL:
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 181, 69, 69));
		break;
	case PropType::SHRUB:
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 44, 73, 38));
		break;
	case PropType::RUINS:
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 169, 171, 158));
		break;
	case PropType::FLOWER:
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 128, 91, 138));
		break;
	case PropType::LIVETREE:
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 95, 174, 93));
		break;
	case PropType::BERRY:
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 112, 131, 25));
		break;
	case PropType::TOMB:
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 95, 95, 95));
		break;
	case PropType::STUMP:
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 88, 63, 43));
		break;
	}

	// mark the props with a dot to make them visible
	SDL_Rect rect;
	rect.x = rect.y = 3;
	rect.w = rect.h = 2;
	SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 255, 255));

	return;
}


void Prop::CreateDescription() {
	vector<const char*> msg;

	switch (type) {
	case PropType::TOADSTOOL:
		msg.push_back("A putrid toadstool");
		msg.push_back("quivers gently,");
		msg.push_back("releasing dark spores.");
		break;
	case PropType::SHRUB:
		msg.push_back("A dark, thorny shrub");
		msg.push_back("shakes in the wind.");
		break;
	case PropType::RUINS:
		msg.push_back("An ancient, weather-worn");
		msg.push_back("ruin points skyward like");
		msg.push_back("a skeletal finger.");
		break;
	case PropType::FLOWER:
		msg.push_back("A once-bright flower");
		msg.push_back("droops under the weight");
		msg.push_back("of Overbourn's pall.");
		break;
	case PropType::LIVETREE:
		msg.push_back("A stunted tree stands");
		msg.push_back("defiantly in the marsh.");
		break;
	case PropType::BERRY:
		msg.push_back("A brambly bush puts forth");
		msg.push_back("bloated, noisome berries.");
		break;
	case PropType::TOMB:
		msg.push_back("A decrepit tombstone");
		msg.push_back("lies as forgotten");
		msg.push_back("as its owner.");
		break;
	case PropType::STUMP:
		msg.push_back("A decaying tree stump");
		msg.push_back("still bears scars of an");
		msg.push_back("act of hideous violence.");
		break;
	default:
		break;
	}

	hudMessage = new HUDMessage(msg, MsgPlacement::PLACE_BOTTOM);

	return;
}


void Prop::DescribeYourself() {
	hud->PushMessage(new HUDMessage(hudMessage));
	return;
}


/************************************************************************************************** OTHER */

ostream& operator<<(ostream& out, const PropType& type) {
	switch (type) {
	case PropType::MARKER:
		out << "PROP:MARKER";
		break;
	case PropType::TOADSTOOL:
		out << "PROP:TOADSTOOL";
		break;
	case PropType::SHRUB:
		out << "PROP:SHRUB";
		break;
	case PropType::RUINS:
		out << "PROP:RUINS";
		break;
	case PropType::FLOWER:
		out << "PROP:FLOWER";
		break;
	case PropType::LIVETREE:
		out << "PROP:TREE";
		break;
	case PropType::BERRY:
		out << "PROP:BERRY";
		break;
	case PropType::TOMB:
		out << "PROP:TOMB";
		break;
	case PropType::STUMP:
		out << "PROP:STUMP";
		break;
	}

	return out;
}
