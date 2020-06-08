/**
 * Storyteller.cpp
 *
 * Author: nrudzicz
 * Created on: May 27, 2009, 10:22
 */

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "engine/GameEngine.h"
#include "engine/Storyteller.h"
#include "sceneobjects/entities/MarkerProp.h"
#include "screens/EndingScreen.h"
#include "screens/RestartScreen.h"

using namespace std;


// map properties
const Uint32 MAP_SIZE = 128;
const Uint32 MAP_STARTBLOCK_SIZE = 16;
const float MAP_LAND_PROB = 0.5f;

// viewpoint properties
const int MAX_DIST_TO_CENTRE = 1;

// prop cluster properties
const int PROP_CLUSTER_SIZE = 10;
const int PROP_MAX_PLACEMENTS = 1000000;

// fading properties
const int FADE_MAGNITUDE = 10;


/************************************************************************************************** PRIVATE HELPERS */

bool clusterContainsPoint(Vector2Ui cluster, Vector2Ui pt) {
	bool hContains = false, vContains = false;

	if (cluster.X() <= MAP_SIZE - PROP_CLUSTER_SIZE) {	// cluster doesn't go off right side
		if (pt.X() >= cluster.X() && pt.X() <= cluster.X() + PROP_CLUSTER_SIZE - 1) {
			hContains = true;
		}
	} else {  // cluster goes off right side
		if (pt.X() >= cluster.X() || pt.X() <= (cluster.X() + PROP_CLUSTER_SIZE - 1) % MAP_SIZE) {
			hContains = true;
		}
	}

	// no horizontal overlap, no overlap at all
	if (!hContains) {
		return false;
	}

	if (cluster.Y() <= MAP_SIZE - PROP_CLUSTER_SIZE) {	// cluster doesn't go off bottom
		if (pt.Y() >= cluster.Y() && pt.Y() <= cluster.Y() + PROP_CLUSTER_SIZE - 1) {
			vContains = true;
		}
	} else {  // cluster goes off bottom
		if (pt.Y() >= cluster.Y() || pt.Y() <= (cluster.Y() + PROP_CLUSTER_SIZE - 1) % MAP_SIZE) {
			vContains = true;
		}
	}

	// assume horizontal contains already, just depend on v contain
	return vContains;
}


// just need to see if any of the four corners of one cluster are contained by the other cluster
bool clustersOverlap(Vector2Ui v1, Vector2Ui v2) {
	// top-left of v2
	if (clusterContainsPoint(v1, v2)) {
		return true;
	}

	Vector2Ui testpt;

	// top-right of v2
	testpt.SetPos((v2.X() + PROP_CLUSTER_SIZE - 1) % MAP_SIZE, v2.Y());
	if (clusterContainsPoint(v1, testpt)) {
		return true;
	}

	// bottom-left of v2
	testpt.SetPos(v2.X(), (v2.Y() + PROP_CLUSTER_SIZE - 1) % MAP_SIZE);
	if (clusterContainsPoint(v1, testpt)) {
		return true;
	}

	// bottom-right of v2
	testpt.SetPos((v2.X() + PROP_CLUSTER_SIZE - 1) % MAP_SIZE, (v2.Y() + PROP_CLUSTER_SIZE - 1) % MAP_SIZE);
	if (clusterContainsPoint(v1, testpt)) {
		return true;
	}

	// no overlaps, no containment
	return false;
}


void loadAudio(const char* filename, Mix_Chunk*& ptr) {
	ptr = Mix_LoadWAV(filename);
	if (!ptr) {
		cout << "ERROR: Could not open " << filename << "!" << endl;
		cout << "  Error code: " << Mix_GetError() << endl;
	}

	return;
}


/************************************************************************************************** {CON|DE}STRUCTORS */

Storyteller::Storyteller() {
	// load our sounds
	loadAudio("assets/sounds/InsectLoop.ogg", insectLoop);
	loadAudio("assets/sounds/InsectSpice1.ogg", insectSpices[0]);
	loadAudio("assets/sounds/InsectSpice2.ogg", insectSpices[1]);
	loadAudio("assets/sounds/InsectSpice3.ogg", insectSpices[2]);
	loadAudio("assets/sounds/Bell.ogg", bellChunk);
	return;
}


Storyteller::~Storyteller() {
	Mix_FreeChunk(insectLoop);
	for (int i = 0; i < 3; ++i) {
		Mix_FreeChunk(insectSpices[i]);
	}
	Mix_FreeChunk(bellChunk);
	return;
}


/************************************************************************************************** METHODS */

Storyteller* Storyteller::Instance() {
	static Storyteller m_instance;
	return &m_instance;
}


void Storyteller::Init() {
	// get a pointer to our HUD instance
	hud = HUD::Instance();
	hud->Init();

	// create a drawing surface
	CreateDrawingSurface();

	// create a new player
	player = new Player();
	controlState = ControlState::WALK;

	// start the game anew
	RestartGame();

	// play the loop as (surprisingly) a loop
	if ((insectLoopChannel = Mix_FadeInChannel(-1, insectLoop, -1, 500)) == -1) {
		cout << "ERROR: Could not play insect loop!" << endl;
		cout << "  Error code: " << Mix_GetError() << endl;
	}

	// get ready to play some spices
	RestartSpiceTimer();

	return;
}


void Storyteller::Process(Uint32 ticks, SDL_Event& event) {
	switch (controlState) {
	case ControlState::FADEIN:
		break;
	case ControlState::WALK:
		ProcessWalk(ticks, event);
		break;
	case ControlState::LOOKPROMPT:
		ProcessLookPrompt(ticks, event);
		break;
	case ControlState::PROPTALK:
		ProcessPropDisplay(ticks, event);
		break;
	case ControlState::MARKTALK:
		ProcessMarkerDisplay(ticks, event);
		break;
	case ControlState::MARKYES:
		controlState = ControlState::WALK;
		AdvanceLevel();
		sceneIsDirty = true;
		break;
	case ControlState::MARKNO:
		ProcessMarkerNo(ticks, event);
		break;
	}

	return;
}


void Storyteller::Update(Uint32 ticks) {
	// play one of the "spice" audio files on cue
	if (controlState == ControlState::FADEIN || controlState == ControlState::WALK) {
		if (spiceTimer.GetTicks() >= spiceTimeout) {
			if ((insectSpiceChannel = Mix_PlayChannel(-1, insectSpices[rand() % 3], 0)) == -1) {
				cout << "ERROR: Could not play spice!" << endl;
				cout << "  Error code: " << Mix_GetError() << endl;
			}
			RestartSpiceTimer();
		}
	}

	switch (controlState) {
	case ControlState::FADEIN:
		fadeLevel -= (int)Clamp<float>((float)FADE_MAGNITUDE * (float)ticks / 1000.0f / (float)MAX_FRAME_RATE, FADE_MAGNITUDE, 1000.0f);
		fadeLevel = Clamp<int>(fadeLevel, 0, 255);
		SDL_SetAlpha(fadeSurface, SDL_SRCALPHA, fadeLevel);
		sceneIsDirty = true;

		if (fadeLevel == 0) {
			controlState = ControlState::WALK;
		}

		break;

	case ControlState::WALK:
	{
		// update the player
		player->Update(ticks);
		if (player->IsDirty()) {
			sceneIsDirty = true;
		}

		RecenterPlayer();
		break;
	} // end case WALK

	case ControlState::MARKNO:
	{
		player->Update(ticks);
		if (player->IsDirty()) {
			sceneIsDirty = true;
		}

		Uint32 time = SDL_GetTicks();
		if (time - lastForget > FORGET_TIME) {
			map->Forget((int)forgetRate);
			lastForget = time;
			sceneIsDirty = true;
			if (map->AllForgotten()) {
				GameEngine::Instance()->PopScreen();
				GameEngine::Instance()->PushScreen(RestartScreen::Instance());
				RestartScreen::Instance()->Init();
			}
		}

		if (forgetRate != ForgetRate::FORGET_FASTEST) {
			if (time - lastForgetRateIncrease > FORGET_TIME_INCR) {
				lastForgetRateIncrease = time;
				switch (forgetRate) {
				case ForgetRate::FORGET_SLOW:
					forgetRate = ForgetRate::FORGET_MED;
					break;
				case ForgetRate::FORGET_MED:
					forgetRate = ForgetRate::FORGET_FAST;
					break;
				case ForgetRate::FORGET_FAST:
					forgetRate = ForgetRate::FORGET_FASTEST;
					break;
				default:
					break;
				}
				break;
			}
		}
		RecenterPlayer();
		break;
	} // end case MARKNO

	default:
		break;
	} // end switch(controlState)

	return;
}


SDL_Surface* Storyteller::DrawSceneSurface() {
	if (sceneIsDirty) {		// have to render it fully
		// clear the drawing surface
		SDL_FillRect(drawingSurface, NULL, 0);

		// get the drawn map
		SDL_Surface* mapsurface = map->DrawMapSurface(viewingCentre);
		if (mapsurface == NULL) {
			cout << "ERROR: Map returned null surface!" << endl;
			exit(RETURN_ERROR);
		}

		// get the drawn player
		SDL_Surface* playersurface = player->GetAgentSurface();
		if (playersurface == NULL) {
			cout << "ERROR: Player returned null surface!" << endl;
			exit(RETURN_ERROR);
		}

		// figure out where to draw the player, relative to the scene's centre point
		Vector2i hvdist = GetPlayerHVDist();
		SDL_Rect playerRect;
		playerRect.x = (DRAWN_TILE_RADIUS + hvdist.X()) * DRAWN_TILE_SIZE;
		playerRect.y = (DRAWN_TILE_RADIUS + hvdist.Y()) * DRAWN_TILE_SIZE;

		// blit all the scene's elements to a single drawing surface
		SDL_BlitSurface(mapsurface, NULL, drawingSurface, NULL);
		SDL_BlitSurface(playersurface, NULL, drawingSurface, &playerRect);

		if (controlState == ControlState::FADEIN) {
			SDL_BlitSurface(fadeSurface, NULL, drawingSurface, NULL);
		}

		sceneIsDirty = false;
	}

	// if the surface isn't dirty, we avoid the above rendering and jump right to this
	return drawingSurface;
}


void Storyteller::CreateDrawingSurface() {
	SDL_FreeSurface(drawingSurface);
	sceneIsDirty = true;

	// create a surface with properties that match the main display surface
	int sz = (2 * DRAWN_TILE_RADIUS + 1) * DRAWN_TILE_SIZE;
	SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE, sz, sz, 32, RMASK, GMASK, BMASK, AMASK);
	drawingSurface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	if (drawingSurface == NULL) {
		cout << "ERROR: Creating Storyteller drawing surface: " << SDL_GetError() << endl;
		exit(RETURN_ERROR);
	}

	// create the surface with which we will fade in and out
	temp = SDL_CreateRGBSurface(SDL_SWSURFACE, sz, sz, 32, RMASK, GMASK, BMASK, AMASK);
	fadeSurface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	return;
}


void Storyteller::RestartGame() {
	level = 1;

	// choose the type of the first level
	currentLvlType = (rand() % 2 == 0) ? RiddleType::HOWMANY : RiddleType::SUBMERGED;

	// create a new map
	NewMap();

	// start to fade in, from BLACK, since we're restarting
	controlState = ControlState::FADEIN;
	fadeLevel = 255;
	SDL_FillRect(fadeSurface, NULL, SDL_MapRGB(fadeSurface->format, 0, 0, 0));

	return;
}


void Storyteller::AdvanceLevel() {
	++level;
	if (level == 2) {
		switch (currentLvlType) {
		case RiddleType::HOWMANY:
			currentLvlType = RiddleType::SUBMERGED;
			break;
		case RiddleType::SUBMERGED:
			currentLvlType = RiddleType::HOWMANY;
			break;
		case RiddleType::ODDMAN:
			cout << "ERROR: Somehow we're advancing from ODDMAN!" << endl;
			exit(RETURN_ERROR);
		}

	} else if (level == 3) {
		currentLvlType = RiddleType::ODDMAN;

	} else if (level == 4) {
		Mix_FadeOutChannel(insectLoopChannel, 1000);
		GameEngine::Instance()->PopScreen();
		GameEngine::Instance()->PushScreen(EndingScreen::Instance());
		EndingScreen::Instance()->Init();
		return;

	} else {
		cout << "ERROR: WTF level is " << level << endl;
		exit(RETURN_ERROR);
	}

	// generate a new map
	NewMap();

	// start to fade in, from WHITE, since we're advancing a level
	controlState = ControlState::FADEIN;
	fadeLevel = 255;
	SDL_FillRect(fadeSurface, NULL, SDL_MapRGB(fadeSurface->format, 255, 255, 255));

	return;
}


void Storyteller::NewMap() {
	srand((Uint32)time(NULL));

	lastForget = lastForgetRateIncrease = SDL_GetTicks();
	forgetRate = ForgetRate::FORGET_SLOW;

	// delete the old map if we have one
	ClearMap();

	// create the map
	map = new Map();
	if (map->Generate(MAP_SIZE, MAP_STARTBLOCK_SIZE, MAP_LAND_PROB) == RETURN_ERROR) {
		cout << "ERROR: Could not generate map!" << endl;
	}

	// shuffle the props
	propList[0] = PropType::TOADSTOOL;
	propList[1] = PropType::SHRUB;
	propList[2] = PropType::RUINS;
	propList[3] = PropType::FLOWER;
	propList[4] = PropType::LIVETREE;
	propList[5] = PropType::BERRY;
	propList[6] = PropType::TOMB;
	propList[7] = PropType::STUMP;
	FisherYatesShuffle<PropType>(propList, 8);

	// populate it with objects
	Vector2Ui markerCluster;
	if (level < 3) {
		PopulateMap(markerCluster);
	} else if (level == 3) {
		PopulateFinalMap(markerCluster);
	} else {
		cout << "ERROR: Somehow level is " << level << "!" << endl;
		exit(RETURN_ERROR);
	}

	// place the marker
	int x = (markerCluster.X() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
	int y = (markerCluster.Y() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
	markerCluster.SetPos(x, y);
	map->GetTile(markerCluster)->GiveProp(new MarkerProp(PropType::MARKER, 0, currentLvlType, propList));

	// inform the player about the new map
	player->SetMap(map);

	// centre our view on a point
	x = rand() % MAP_SIZE;
	y = rand() % MAP_SIZE;
	viewingCentre.SetPos(x, y);
	player->SetPos(x, y);

	return;
}


void Storyteller::PopulateMap(Vector2Ui& markerCluster) {
	// create a vector of (non-overlapping) clusters---one for each type of prop
	vector<Vector2Ui*> clusterlist;
	vector<Vector2Ui*>::iterator iter;
	for (int i = 0; i < NUM_PROP_TYPES; ++i) {
		int x, y;
		bool overlaps, filled;
		int overlapCount = 0, noFillCount = 0;

		do {
			overlaps = false;
			filled = true;

			// generate x, y
			x = rand() % MAP_SIZE;
			y = rand() % MAP_SIZE;

			// check if it overlaps any other cluster
			for (iter = clusterlist.begin(); iter != clusterlist.end(); ++iter) {
				if (clustersOverlap(Vector2Ui(x, y), **iter) || clustersOverlap(**iter, Vector2Ui(x, y))) {
					overlaps = true;
					break;
				}
			}

			// die if we're taking too long
			if (overlaps) {
				if (++overlapCount > PROP_MAX_PLACEMENTS) {
					cout << "ERROR: Placing props is taking too long! Dying politely." << endl;
					exit(RETURN_ERROR);
				}
			}

			if (i == 0 && currentLvlType == RiddleType::SUBMERGED) {
				riddlePropSubmerged = rand() % 2 == 0;
				riddlePropNum = rand() % 3 + 1;

				if (DEBUG) {
					cout << "Current level type: SUBMERGED" << endl;
					if (riddlePropSubmerged) {
						cout << "  " << propList[0] << " is submerged." << endl;
					} else {
						cout << "  " << propList[0] << " is NOT submerged." << endl;
					}
				}

				// see if cluster has enough in/out of water as needed
				filled = ClusterHasEnoughType(Vector2Ui(x, y),
					riddlePropSubmerged ? TileType::WATER : TileType::LAND,
					riddlePropNum);

				// die if taking too long looking for fillable zones
				if (!filled) {
					if (++noFillCount > PROP_MAX_PLACEMENTS) {
						cout << "ERROR: Taking too long looking for fillable zones in submerged!" << endl;
						exit(RETURN_ERROR);
					}
				}
			}

		} while (overlaps || !filled);
		clusterlist.push_back(new Vector2Ui(x, y));
	}  // for(num_prop_types)

	// fill each individual cluster
	if (currentLvlType == RiddleType::SUBMERGED) {
		FillCluster(*clusterlist[0], propList[0], riddlePropNum, riddlePropSubmerged);
	} else if (currentLvlType == RiddleType::HOWMANY) {
		riddlePropNum = rand() % 3 + 1;
		FillCluster(*clusterlist[0], propList[0], riddlePropNum);

		if (DEBUG) {
			cout << "Current level type: HOWMANY" << endl;
			cout << "  " << propList[0] << " has " << riddlePropNum << " elements." << endl;
		}
	}
	FillCluster(*clusterlist[1], propList[1], rand() % 2 + 1);
	FillCluster(*clusterlist[2], propList[2], rand() % 2 + 1);
	FillCluster(*clusterlist[3], propList[3], rand() % 2 + 1);
	FillCluster(*clusterlist[4], propList[4], rand() % 2 + 1);
	FillCluster(*clusterlist[5], propList[5], rand() % 2 + 1);
	FillCluster(*clusterlist[6], propList[6], rand() % 2 + 1);
	FillCluster(*clusterlist[7], propList[7], rand() % 2 + 1);

	// return value showing where the marker is placed
	markerCluster.SetPos(clusterlist[8]->X(), clusterlist[8]->Y());

	// call the destructor on each generated Vector2Ui
	for (iter = clusterlist.begin(); iter < clusterlist.end(); ++iter) {
		delete* iter;
	}
	clusterlist.clear();

	return;
}


void Storyteller::PopulateFinalMap(Vector2Ui& markerCluster) {
	int sameNum, differNum;
	bool sameWater, differWater;
	vector<Vector2Ui*> clusterlist;
	vector<Vector2Ui*>::iterator iter;

	sameNum = differNum = -1;
	sameWater = differWater = false;

	// choose which type of property OddMan will use
	OddManProperty oddManProperty = (rand() % 2 == 0) ? OddManProperty::NUM : OddManProperty::SUB;

	// place Sames and Differ depending on what type of riddle we're asking
	switch (oddManProperty) {
	case OddManProperty::NUM:
		// choose how many of each Same prop there will be, and how much of the Differ
		sameNum = rand() % 3 + 1;
		do {
			differNum = rand() % 3 + 1;
		} while (differNum == sameNum);

		// choose if two Sames will be in or out of the water
		sameWater = rand() % 2 == 0;
		differWater = !sameWater;
		break; // end of case NUM

	case OddManProperty::SUB:
		// choose whether the Sames will be over water; Differ will be opposite
		sameWater = rand() % 2 == 0;
		differWater = !sameWater;

		// choose how many there will be of two Sames
		sameNum = rand() % 3 + 1;
		do {
			differNum = rand() % 3 + 1;
		} while (differNum == sameNum);

		break; // end of case SUB
	}

	// find one cluster for each Same and the Differ
	for (int i = 0; i < 4; ++i) {
		int x, y;
		bool overlaps, filled;
		int overlapCount = 0, noFillCount = 0;

		// find a cluster for this Same that is non-overlapping and which has enough land/water
		do {
			overlaps = false;
			filled = true;

			x = rand() % MAP_SIZE;
			y = rand() % MAP_SIZE;

			// check if it overlaps any other cluster
			for (iter = clusterlist.begin(); iter != clusterlist.end(); ++iter) {
				if (clustersOverlap(Vector2Ui(x, y), **iter) || clustersOverlap(**iter, Vector2Ui(x, y))) {
					overlaps = true;
					break;
				}
			}

			// die if taking too long looking for overlaps
			if (overlaps) {
				if (++overlapCount > PROP_MAX_PLACEMENTS) {
					cout << "ERROR: Taking too long to place things in level 3!" << endl;
					exit(RETURN_ERROR);
				}
			}

			// see if cluster has enough in/out of water as needed
			// first two Sames should have same water/land, third Same and Differ should have other water/land
			switch (oddManProperty) {
			case OddManProperty::NUM:
				if (i < 2) {
					filled = ClusterHasEnoughType(Vector2Ui(x, y), sameWater ? TileType::WATER : TileType::LAND, sameNum);
				} else {
					filled = ClusterHasEnoughType(Vector2Ui(x, y), differWater ? TileType::WATER : TileType::LAND, (i == 2) ? sameNum : differNum);
				}
				break;
			case OddManProperty::SUB:
				if (i < 3) {
					filled = ClusterHasEnoughType(Vector2Ui(x, y), sameWater ? TileType::WATER : TileType::LAND, (i < 2) ? sameNum : differNum);
				} else {
					filled = ClusterHasEnoughType(Vector2Ui(x, y), differWater ? TileType::WATER : TileType::LAND, differNum);
				}
			}

			// die if taking too long looking for fillable zones
			if (!filled) {
				if (++noFillCount > PROP_MAX_PLACEMENTS) {
					cout << "ERROR: Taking too long looking for fillable zones in level 3!" << endl;
					exit(RETURN_ERROR);
				}
			}
		} while (overlaps || !filled);

		// found a cluster that doesn't overlap, and has the right amount of land/water
		clusterlist.push_back(new Vector2Ui(x, y));
	} // end for(find a cluster for each Same)

	if (DEBUG) {
		switch (oddManProperty) {
		case OddManProperty::NUM:
			cout << "SAME: " << propList[0] << ", num: " << sameNum << ", water: " << sameWater << endl;
			cout << "SAME: " << propList[1] << ", num: " << sameNum << ", water: " << sameWater << endl;
			cout << "SAME: " << propList[2] << ", num: " << sameNum << ", water: " << differWater << endl;
			cout << "DIFF: " << propList[3] << ", num: " << differNum << ", water: " << differWater << endl;
			break;
		case OddManProperty::SUB:
			cout << "SAME: " << propList[0] << ", num: " << sameNum << ", water: " << sameWater << endl;
			cout << "SAME: " << propList[1] << ", num: " << sameNum << ", water: " << sameWater << endl;
			cout << "SAME: " << propList[2] << ", num: " << differNum << ", water: " << sameWater << endl;
			cout << "DIFF: " << propList[3] << ", num: " << differNum << ", water: " << differWater << endl;
			break;
		}
	}

	// have found clusters for the Sames and the Differ.
	// now find clusters for the rest.
	for (int i = 4; i < NUM_PROP_TYPES; ++i) {
		int count = 0;
		int x, y;
		bool overlaps;
		do {
			overlaps = false;

			// generate x, y
			x = rand() % MAP_SIZE;
			y = rand() % MAP_SIZE;

			// check if it overlaps any other cluster
			for (iter = clusterlist.begin(); iter != clusterlist.end(); ++iter) {
				if (clustersOverlap(Vector2Ui(x, y), **iter) || clustersOverlap(**iter, Vector2Ui(x, y))) {
					overlaps = true;
					break;
				}
			}

			// die if we're taking too long
			if (overlaps) {
				if (++count > PROP_MAX_PLACEMENTS) {
					cout << "ERROR: Placing props is taking too long! Dying politely." << endl;
					exit(RETURN_ERROR);
				}
			}

		} while (overlaps);

		clusterlist.push_back(new Vector2Ui(x, y));
	}  // for(the rest of the props)

	// fill each individual cluster
	FillCluster(*clusterlist[0], propList[0], sameNum, sameWater);
	FillCluster(*clusterlist[1], propList[1], sameNum, sameWater);
	switch (oddManProperty) {
	case OddManProperty::NUM:
		FillCluster(*clusterlist[2], propList[2], sameNum, differWater);
		break;
	case OddManProperty::SUB:
		FillCluster(*clusterlist[2], propList[2], differNum, sameWater);
		break;
	}
	FillCluster(*clusterlist[3], propList[3], differNum, differWater);
	FillCluster(*clusterlist[4], propList[4], rand() % 2 + 1);
	FillCluster(*clusterlist[5], propList[5], rand() % 2 + 1);
	FillCluster(*clusterlist[6], propList[6], rand() % 2 + 1);
	FillCluster(*clusterlist[7], propList[7], rand() % 2 + 1);

	// return value showing where the marker is placed
	markerCluster.SetPos(clusterlist[8]->X(), clusterlist[8]->Y());

	// call the destructor on each generated Vector2Ui
	for (iter = clusterlist.begin(); iter < clusterlist.end(); ++iter) {
		delete* iter;
	}
	clusterlist.clear();

	return;
}


void Storyteller::FillCluster(Vector2Ui startpt, PropType type, int num) {
	Vector2Ui v1, v2, v3;
	Uint32 x, y;

	// need at least one prop
	x = (startpt.X() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
	y = (startpt.Y() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
	v1.SetPos(x, y);

	// if we need a second prop
	if (num >= 2) {
		do {
			x = (startpt.X() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
			y = (startpt.Y() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
		} while (x == v1.X() && y == v1.Y());
		v2.SetPos(x, y);
	}

	// if we need a third prop
	if (num == 3) {
		do {
			x = (startpt.X() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
			y = (startpt.Y() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
		} while ((x == v1.X() && y == v1.Y()) || (x == v2.X() && y == v2.Y()));
		v3.SetPos(x, y);
	}

	// place all the props
	switch (num) {
	case 1:
		map->GetTile(v1)->GiveProp(new Prop(type, 0));
		break;
	case 2:
		map->GetTile(v1)->GiveProp(new Prop(type, 1));
		map->GetTile(v2)->GiveProp(new Prop(type, 1));
		break;
	case 3:
		map->GetTile(v1)->GiveProp(new Prop(type, 2));
		map->GetTile(v2)->GiveProp(new Prop(type, 2));
		map->GetTile(v3)->GiveProp(new Prop(type, 2));
		break;
	}

	return;
}


void Storyteller::FillCluster(Vector2Ui startpt, PropType type, int num, bool inWater) {
	// now, place props as required
	Vector2Ui v1, v2, v3;
	Uint32 x, y;

	if (inWater) {
		// need at least one prop
		do {
			x = (startpt.X() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
			y = (startpt.Y() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
		} while (map->GetTile(Vector2Ui(x, y))->GetTileType() != TileType::WATER);
		v1.SetPos(x, y);

		// if we need a second prop
		if (num >= 2) {
			do {
				x = (startpt.X() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
				y = (startpt.Y() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
			} while ((x == v1.X() && y == v1.Y()) ||
				map->GetTile(Vector2Ui(x, y))->GetTileType() != TileType::WATER);
			v2.SetPos(x, y);
		}

		// if we need a third prop
		if (num == 3) {
			do {
				x = (startpt.X() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
				y = (startpt.Y() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
			} while ((x == v1.X() && y == v1.Y()) ||
				(x == v2.X() && y == v2.Y()) ||
				map->GetTile(Vector2Ui(x, y))->GetTileType() != TileType::WATER);
			v3.SetPos(x, y);
		}
	} else {	// !inWater
		// need at least one prop
		do {
			x = (startpt.X() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
			y = (startpt.Y() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
		} while (map->GetTile(Vector2Ui(x, y))->GetTileType() != TileType::LAND);
		v1.SetPos(x, y);

		// if we need a second prop
		if (num >= 2) {
			do {
				x = (startpt.X() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
				y = (startpt.Y() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
			} while ((x == v1.X() && y == v1.Y()) ||
				map->GetTile(Vector2Ui(x, y))->GetTileType() != TileType::LAND);
			v2.SetPos(x, y);
		}

		// if we need a third prop
		if (num == 3) {
			do {
				x = (startpt.X() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
				y = (startpt.Y() + rand() % PROP_CLUSTER_SIZE) % MAP_SIZE;
			} while ((x == v1.X() && y == v1.Y()) ||
				(x == v2.X() && y == v2.Y()) ||
				map->GetTile(Vector2Ui(x, y))->GetTileType() != TileType::LAND);
			v3.SetPos(x, y);
		}
	}

	// place all the props
	switch (num) {
	case 1:
		map->GetTile(v1)->GiveProp(new Prop(type, 0));
		break;
	case 2:
		map->GetTile(v1)->GiveProp(new Prop(type, 1));
		map->GetTile(v2)->GiveProp(new Prop(type, 1));
		break;
	case 3:
		map->GetTile(v1)->GiveProp(new Prop(type, 2));
		map->GetTile(v2)->GiveProp(new Prop(type, 2));
		map->GetTile(v3)->GiveProp(new Prop(type, 2));
		break;
	}

	return;
}


bool Storyteller::ClusterHasEnoughType(Vector2Ui startpt, TileType type, int num) {
	// make sure we have enough water or land to place things appropriately
	int totalLand = 0, totalWater = 0;
	for (int i = 0; i < PROP_CLUSTER_SIZE; ++i) {
		for (int j = 0; j < PROP_CLUSTER_SIZE; ++j) {
			Uint32 row = (startpt.Y() + i) % MAP_SIZE;
			Uint32 col = (startpt.X() + j) % MAP_SIZE;
			if (map->GetTile(Vector2Ui(col, row))->GetTileType() == TileType::LAND) {
				++totalLand;
			} else {
				++totalWater;
			}
		}
	}

	switch (type) {
	case TileType::LAND:
		return totalLand >= num;
	case TileType::WATER:
		return totalWater >= num;
	default:
		return false;
	}
}


void Storyteller::ClearMap() {
	delete map;
	return;
}


void Storyteller::RestartSpiceTimer() {
	spiceTimer.Stop();
	spiceTimeout = rand() % 10000 + 5000; // anywhere from 5 to 15 seconds
	spiceTimer.Start();
	return;
}


Vector2i Storyteller::GetPlayerHVDist() {
	Vector2i dist;
	int count;

	// get horizontal distance
	count = 0;
	while (true) {
		if ((viewingCentre.X() - count + MAP_SIZE) % MAP_SIZE == player->GetPos().X()) {
			dist.SetPos(-count, 0);
			break;
		} else if ((viewingCentre.X() + count) % MAP_SIZE == player->GetPos().X()) {
			dist.SetPos(count, 0);
			break;
		}
		++count;
	}

	// get vertical distance
	count = 0;
	while (true) {
		if ((viewingCentre.Y() - count + MAP_SIZE) % MAP_SIZE == player->GetPos().Y()) {
			dist.SetPos(dist.X(), -count);
			break;
		} else if ((viewingCentre.Y() + count) % MAP_SIZE == player->GetPos().Y()) {
			dist.SetPos(dist.X(), count);
			break;
		}
		++count;
	}

	return dist;
}


void Storyteller::RecenterPlayer() {
	// if the player's gone too far left or right, or up or down
	Vector2i hvdist = GetPlayerHVDist();
	if (hvdist.X() < -MAX_DIST_TO_CENTRE) {
		viewingCentre.SetPos((viewingCentre.X() - 1 + MAP_SIZE) % MAP_SIZE, viewingCentre.Y());
		sceneIsDirty = true;
	} else if (hvdist.X() > MAX_DIST_TO_CENTRE) {
		viewingCentre.SetPos((viewingCentre.X() + 1) % MAP_SIZE, viewingCentre.Y());
		sceneIsDirty = true;
	}
	// if the player's gone too far up or down...
	if (hvdist.Y() < -MAX_DIST_TO_CENTRE) {
		viewingCentre.SetPos(viewingCentre.X(), (viewingCentre.Y() - 1 + MAP_SIZE) % MAP_SIZE);
		sceneIsDirty = true;
	} else if (hvdist.Y() > MAX_DIST_TO_CENTRE) {
		viewingCentre.SetPos(viewingCentre.X(), (viewingCentre.Y() + 1) % MAP_SIZE);
		sceneIsDirty = true;
	}

	return;
}


void Storyteller::ProcessWalk(Uint32 ticks, SDL_Event& event) {
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_l:
			hud->PushMessage(new HUDMessage(vector<const char*>(1, "LOOK -- Direction?"), MsgPlacement::PLACE_BOTTOM));
			controlState = ControlState::LOOKPROMPT;
			break;
		case SDLK_UP:
		case SDLK_DOWN:
		case SDLK_LEFT:
		case SDLK_RIGHT:
			player->Process(ticks, event);
			break;
		default:
			break;
		}

	case SDL_KEYUP:
		switch (event.key.keysym.sym) {
		case SDLK_UP:
		case SDLK_DOWN:
		case SDLK_LEFT:
		case SDLK_RIGHT:
			switch (controlState) {
			case ControlState::WALK:
				player->Process(ticks, event);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	} // switch(event.type)

	return;
}


void Storyteller::ProcessLookPrompt(Uint32 ticks, SDL_Event& event) {
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_UP:
			hud->PopMessage();
			if (map->GetUpTile(player->GetPos())->HasProp()) {
				if (map->GetUpTile(player->GetPos())->GetPropType() == PropType::MARKER) {
					controlState = ControlState::MARKTALK;
				} else {
					controlState = ControlState::PROPTALK;
				}
			} else {
				controlState = ControlState::PROPTALK;
			}
			map->GetUpTile(player->GetPos())->DescribeYourself();
			break;
		case SDLK_DOWN:
			hud->PopMessage();
			if (map->GetDnTile(player->GetPos())->HasProp()) {
				if (map->GetDnTile(player->GetPos())->GetPropType() == PropType::MARKER) {
					controlState = ControlState::MARKTALK;
				} else {
					controlState = ControlState::PROPTALK;
				}
			} else {
				controlState = ControlState::PROPTALK;
			}
			map->GetDnTile(player->GetPos())->DescribeYourself();
			break;
		case SDLK_LEFT:
			hud->PopMessage();
			if (map->GetLtTile(player->GetPos())->HasProp()) {
				if (map->GetLtTile(player->GetPos())->GetPropType() == PropType::MARKER) {
					controlState = ControlState::MARKTALK;
				} else {
					controlState = ControlState::PROPTALK;
				}
			} else {
				controlState = ControlState::PROPTALK;
			}
			map->GetLtTile(player->GetPos())->DescribeYourself();
			break;
		case SDLK_RIGHT:
			hud->PopMessage();
			if (map->GetRtTile(player->GetPos())->HasProp()) {
				if (map->GetRtTile(player->GetPos())->GetPropType() == PropType::MARKER) {
					controlState = ControlState::MARKTALK;
				} else {
					controlState = ControlState::PROPTALK;
				}
			} else {
				controlState = ControlState::PROPTALK;
			}
			map->GetRtTile(player->GetPos())->DescribeYourself();
			break;
		case SDLK_RETURN:
		case SDLK_SPACE:
			hud->PopMessage();
			controlState = ControlState::WALK;
		default:
			break;
		}

	case SDL_KEYUP:
		break;
	}

	return;
}


void Storyteller::ProcessPropDisplay(Uint32 ticks, SDL_Event& event) {
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_RETURN:
		case SDLK_SPACE:
			hud->PopMessage();

			// if answering a query and finished displaying text, back to game mode
			if (!hud->IsDisplaying()) {
				controlState = ControlState::WALK;
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


void Storyteller::ProcessMarkerDisplay(Uint32 ticks, SDL_Event& event) {
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_RETURN:
		case SDLK_SPACE:
			hud->PopMessage();

			// if answering a query and finished displaying text, back to game mode
			if (!hud->IsDisplaying()) {
				controlState = ControlState::WALK;
			}
			break;
		case SDLK_1:	// answer "How many" riddle
		case SDLK_2:
		case SDLK_3:
			if (currentLvlType == RiddleType::HOWMANY && hud->IsLastMessage()) {
				AnswerHowMany(event.key.keysym.sym);
			}
			break;
		case SDLK_y:	// answer "Is it submerged" riddle
		case SDLK_n:
			if (currentLvlType == RiddleType::SUBMERGED && hud->IsLastMessage()) {
				AnswerSubmerged(event.key.keysym.sym);
			}
			break;
		case SDLK_t:	// answer "Odd man out" riddle
		case SDLK_r:
		case SDLK_f:
		case SDLK_e:
		case SDLK_b:
		case SDLK_o:
		case SDLK_s:
		case SDLK_p:
			if (currentLvlType == RiddleType::ODDMAN && hud->IsLastMessage()) {
				AnswerOddMan(event.key.keysym.sym);
			}
			break;

		default:
			break;
		}
		break;

	case SDL_KEYUP:
		break;
	}

	if (controlState == ControlState::MARKNO) {	// we answered incorrectly
		if ((bellChannel = Mix_PlayChannel(-1, bellChunk, 2)) == -1) {
			cout << "ERROR: Could not ring bell!" << endl;
			cout << "  Error code: " << Mix_GetError() << endl;
		}

		// fade out any and all insect sounds
		Mix_FadeOutChannel(insectLoopChannel, 1000);
		if (bellChannel != insectSpiceChannel) { // channels won't be the same if spice is currently playing
			Mix_FadeOutChannel(insectSpiceChannel, 1000);
		}
	}

	return;
}


void Storyteller::ProcessMarkerYes(Uint32 ticks, SDL_Event& event) {
	return;
}


void Storyteller::ProcessMarkerNo(Uint32 ticks, SDL_Event& event) {
	switch (event.type) {
	case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		case SDLK_UP:
		case SDLK_DOWN:
		case SDLK_LEFT:
		case SDLK_RIGHT:
			player->Process(ticks, event);
			break;
		default:
			break;
		}

	case SDL_KEYUP:
		switch (event.key.keysym.sym) {
		case SDLK_UP:
		case SDLK_DOWN:
		case SDLK_LEFT:
		case SDLK_RIGHT:
			switch (controlState) {
			case ControlState::WALK:
			case ControlState::MARKNO:
			case ControlState::MARKYES:
				player->Process(ticks, event);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	} // switch(event.type)

	return;
}


void Storyteller::AnswerHowMany(SDLKey key) {
	hud->PopMessage();

	switch (key) {
	case SDLK_1:
		if (riddlePropNum == 1) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	case SDLK_2:
		if (riddlePropNum == 2) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	case SDLK_3:
		if (riddlePropNum == 3) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	default:
		break;
	}

	return;
}


void Storyteller::AnswerSubmerged(SDLKey key) {
	hud->PopMessage();

	switch (key) {
	case SDLK_y:
		if (riddlePropSubmerged) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	case SDLK_n:
		if (!riddlePropSubmerged) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	default:
		break;
	}

	return;
}


void Storyteller::AnswerOddMan(SDLKey key) {
	hud->PopMessage();

	switch (key) {
	case SDLK_t:
		if (propList[3] == PropType::TOADSTOOL) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	case SDLK_r:
		if (propList[3] == PropType::RUINS) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	case SDLK_f:
		if (propList[3] == PropType::FLOWER) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	case SDLK_e:
		if (propList[3] == PropType::LIVETREE) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	case SDLK_b:
		if (propList[3] == PropType::BERRY) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	case SDLK_o:
		if (propList[3] == PropType::TOMB) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	case SDLK_p:
		if (propList[3] == PropType::STUMP) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	case SDLK_s:
		if (propList[3] == PropType::SHRUB) {
			controlState = ControlState::MARKYES;
		} else {
			controlState = ControlState::MARKNO;
		}
		break;
	default:
		break;
	}

	return;
}
