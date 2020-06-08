/**
 * Map.cpp
 *
 * Author: nrudzicz
 * Created on: 9-May-2009
 */

#include <algorithm>
#include <iostream>

#include <SDL.h>

#include "engine/GameEngine.h"
#include "engine/Storyteller.h"
#include "sceneobjects/Map.h"

using namespace std;

const int MAX_FOG = 200;
const int MAX_FOG_TAKEAWAY = 150;


/************************************************************************************************** {CON|DE}STRUCTORS */

Map::Map() {
	// init with garbage
	numTilesPerSide = 0;
	drawingSurface = NULL;

	// set up our array for forgetting tiles, if necessary
	forgetInd = 0;
	forgetTiles = NULL;

	return;
}


Map::~Map() {
	ClearMap();
	SDL_FreeSurface(drawingSurface);
	return;
}


/************************************************************************************************** PRIVATE HELPERS */

int getUpIndex(Uint32 y, Uint32 size) {
	return (y + size - 1) % size;
}


int getDnIndex(Uint32 y, Uint32 size) {
	return (y + 1) % size;
}


int getLtIndex(Uint32 x, Uint32 size) {
	return (x + size - 1) % size;
}


int getRtIndex(Uint32 x, Uint32 size) {
	return (x + 1) % size;
}


/************************************************************************************************** METHODS */

/**
 * Set the initial large-scale distribution of tiles
 * NOTE: Assume the map is already allocated
 */
void Map::Seed(bool** boolarray, Uint32 blockSize, float landProbability) {
	int numBlocks = numTilesPerSide / blockSize;

	// allocate a temp grid and fill it with random values
	bool** tempgrid = new bool* [numBlocks];
	for (int i = 0; i < numBlocks; ++i) {
		tempgrid[i] = new bool[numBlocks];
		for (int j = 0; j < numBlocks; ++j) {
			if (RandomTrueFalse(landProbability)) {
				tempgrid[i][j] = true;
			} else {
				tempgrid[i][j] = false;
			}
		}
	}

	// set map values to their appropriate values from tempgrid
	for (Uint32 i = 0; i < numTilesPerSide; ++i) {
		for (Uint32 j = 0; j < numTilesPerSide; ++j) {
			boolarray[i][j] = tempgrid[i / blockSize][j / blockSize];
		}
	}

	// delete the temp grid
	for (int i = 0; i < numBlocks; ++i) {
		delete[] tempgrid[i];
	}
	delete[]tempgrid;

	return;
}


void Map::DetailIteration(bool** boolarray, Uint32 blockSize) {
	int numBlocks = numTilesPerSide / blockSize;

	// allocate a temp grid
	bool** tempgrid = new bool* [numBlocks];
	for (int i = 0; i < numBlocks; ++i) {
		tempgrid[i] = new bool[numBlocks];

		// each tile gets the same value as the tiles on the underlying map
		for (int j = 0; j < numBlocks; ++j) {
			tempgrid[i][j] = boolarray[i * blockSize][j * blockSize];
		}
	}

	// check each tile in the temp grid, to see if/how it should be modified
	for (int i = 0; i < numBlocks; ++i) {
		for (int j = 0; j < numBlocks; ++j) {
			int numland = 0; // only need to know number of land tiles (can deduce water, anyway)

			int c0 = getLtIndex(j, numBlocks);
			int c1 = getRtIndex(j, numBlocks);
			int r0 = getUpIndex(i, numBlocks);
			int r1 = getDnIndex(i, numBlocks);

			if (tempgrid[r0][c0]) {	// above-left
				++numland;
			}

			if (tempgrid[r0][j]) {	// above
				++numland;
			}

			if (tempgrid[r0][c1]) {	// above-right
				++numland;
			}

			if (tempgrid[i][c0]) {	// left
				++numland;
			}

			if (tempgrid[i][c1]) {	// right
				++numland;
			}

			if (tempgrid[r1][c0]) {	// below-left
				++numland;
			}

			if (tempgrid[r1][j]) {	// below
				++numland;
			}

			if (tempgrid[r1][c1]) {	// below-right
				++numland;
			}

			float landProbability = (float)numland / 8.0f;
			bool landTile = RandomTrueFalse(landProbability);

			// change the map tiles appropriately
			for (Uint32 m = 0; m < blockSize; ++m) {
				for (Uint32 n = 0; n < blockSize; ++n) {
					boolarray[i * blockSize + m][j * blockSize + n] = landTile;
				}
			}
		}
	}

	// delete the temp grid
	for (int i = 0; i < numBlocks; ++i) {
		delete[] tempgrid[i];
	}
	delete[] tempgrid;

	if (blockSize > 1) {
		DetailIteration(boolarray, blockSize / 2);
	} else {
		return;
	}

	return;
}


void Map::FinaliseMap(bool** boolarray) {
	int numWater = 0, numLand = 0;

	map = new Tile * *[numTilesPerSide];
	for (Uint32 i = 0; i < numTilesPerSide; ++i) {
		map[i] = new Tile * [numTilesPerSide];
		for (Uint32 j = 0; j < numTilesPerSide; ++j) {
			if (boolarray[i][j]) {
				map[i][j] = new Tile(TileType::LAND);
				++numLand;
			} else {
				map[i][j] = new Tile(TileType::WATER);
				++numWater;
			}
		}
	}

	// check if we can darken
	for (Uint32 i = 0; i < numTilesPerSide; ++i) {
		for (Uint32 j = 0; j < numTilesPerSide; ++j) {
			if (map[i][j]->GetTileType() == TileType::LAND) {
				Vector2Ui v(j, i);
				if (GetUpTile(v)->GetTileType() == TileType::WATER) {
					map[i][j]->SetNearWater(true);
					continue;
				}
				if (GetDnTile(v)->GetTileType() == TileType::WATER) {
					map[i][j]->SetNearWater(true);
					continue;
				}
				if (GetLtTile(v)->GetTileType() == TileType::WATER) {
					map[i][j]->SetNearWater(true);
					continue;
				}
				if (GetRtTile(v)->GetTileType() == TileType::WATER) {
					map[i][j]->SetNearWater(true);
					continue;
				}
			}
		}
	}

	return;
}


void Map::DoObjectPlacement() {
	return;
}


// create an appropriately-sized drawing surface, if need be
void Map::GetNewDrawingSurface() {
	SDL_FreeSurface(drawingSurface);

	// create a surface with properties that match the main display surface
	int sz = (2 * DRAWN_TILE_RADIUS + 1) * DRAWN_TILE_SIZE;
	SDL_Surface* temp = SDL_CreateRGBSurface(SDL_SWSURFACE, sz, sz, 32, RMASK, GMASK, BMASK, AMASK);
	drawingSurface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	if (drawingSurface == NULL) {
		cout << "ERROR: Creating Map drawing surface: " << SDL_GetError() << endl;
	}

	return;
}


void Map::ClearMap() {
	// deallocate memory
	if (numTilesPerSide != 0) {
		for (Uint32 i = 0; i < numTilesPerSide; ++i) {
			delete[] map[i];
		}
		delete map;
	}

	// remove our set of forgotten tiles
	delete forgetTiles;

	return;
}


int Map::Generate(Uint32 sz, Uint32 blockSize, float landProbability) {
	// error checking
	if (!IsPowerOfTwo(sz)) {
		cout << "ERROR: Map size (" << sz << ") must be power of two!" << endl;
		return RETURN_ERROR;
	}
	if (!IsPowerOfTwo(blockSize)) {
		cout << "ERROR: Granularity (" << blockSize << ") must be power of two!" << endl;
		return RETURN_ERROR;
	}
	if (blockSize >= sz) {
		cout << "ERROR: Granularity (" << blockSize << ") must be strictly less than size (" << sz << ")!" << endl;
		return RETURN_ERROR;
	}

	ClearMap();

	numTilesPerSide = sz;

	// faster (and simpler) if we do generation with bools instead of calling tons of constructors
	// NOTE: "true" means LAND, "false" means WATER
	bool** boolarray = new bool* [numTilesPerSide];
	for (Uint32 i = 0; i < numTilesPerSide; ++i) {
		boolarray[i] = new bool[numTilesPerSide];
		for (Uint32 j = 0; j < numTilesPerSide; ++j) {
			boolarray[i][j] = false;
		}
	}

	// seed the map with randomness, then iterate in finer detail
	Seed(boolarray, blockSize, landProbability);
	DetailIteration(boolarray, blockSize);

	// convert the temporary bools into actual tiles
	FinaliseMap(boolarray);

	// delete the temporary bool array
	for (Uint32 i = 0; i < numTilesPerSide; ++i) {
		delete[] boolarray[i];
	}
	delete[] boolarray;

	// place the player and other objects
	DoObjectPlacement();

	// create an SDL_Surface we can blit
	GetNewDrawingSurface();

	// create the array of indices that we will "forget" later on
	forgetTiles = new Uint32[sz * sz];
	for (int i = 0; i < (int)(sz * sz); ++i) {
		forgetTiles[i] = i;
	}
	FisherYatesShuffle<Uint32>(forgetTiles, sz * sz);

	return RETURN_SUCCESS;
}


Tile* Map::GetUpTile(Vector2Ui v) {
	return map[(v.Y() + numTilesPerSide - 1) % numTilesPerSide][v.X()];
}


Tile* Map::GetDnTile(Vector2Ui v) {
	return map[(v.Y() + numTilesPerSide + 1) % numTilesPerSide][v.X()];
}


Tile* Map::GetLtTile(Vector2Ui v) {
	return map[v.Y()][(v.X() + numTilesPerSide - 1) % numTilesPerSide];
}


Tile* Map::GetRtTile(Vector2Ui v) {
	return map[v.Y()][(v.X() + numTilesPerSide + 1) % numTilesPerSide];
}


Tile* Map::GetTile(Vector2Ui index) {
	return map[index.Y()][index.X()];
}


void Map::Forget(int num) {
	for (int i = 0; i < num; ++i) {
		if (forgetInd >= numTilesPerSide * numTilesPerSide) {
			return;
		}

		int row = (int)(forgetTiles[forgetInd] / numTilesPerSide);
		int col = forgetTiles[forgetInd] % numTilesPerSide;
		map[row][col]->Forget();

		++forgetInd;
	}

	return;
}


bool Map::AllForgotten() {
	return forgetInd == numTilesPerSide * numTilesPerSide;
}


SDL_Surface* Map::DrawMapSurface(Vector2Ui centre) {
	// clear the drawing surface
	SDL_FillRect(drawingSurface, NULL, 0);

	// draw each tile from the map
	SDL_Rect rect;
	rect.w = rect.h = DRAWN_TILE_SIZE;
	for (int i = -(int)DRAWN_TILE_RADIUS; i < (int)DRAWN_TILE_RADIUS; ++i) {
		for (int j = -(int)DRAWN_TILE_RADIUS; j < (int)DRAWN_TILE_RADIUS; ++j) {
			// what tile is this?
			int maprow = (centre.Y() + i + numTilesPerSide) % numTilesPerSide;
			int mapcol = (centre.X() + j + numTilesPerSide) % numTilesPerSide;

			// "visit" the tile if it's within viewing distance
			int radiusFraction = Clamp<int>(MAX_FOG * (i * i + j * j) / DRAWN_TILE_RADIUS_SQ, 0, MAX_FOG_TAKEAWAY);
			if (radiusFraction < MAX_FOG_TAKEAWAY) {
				map[maprow][mapcol]->Visit();
			}

			// get this tile's drawn surface
			SDL_Surface* tileSurface = map[maprow][mapcol]->GetSurface();

			// where do I display this on the map's surface?
			rect.x = (DRAWN_TILE_RADIUS + j) * rect.w;
			rect.y = (DRAWN_TILE_RADIUS + i) * rect.h;

			// apply fog
			SDL_SetAlpha(tileSurface, SDL_SRCALPHA, 255 - radiusFraction);

			// draw the tile
			int blitResult;
			if ((blitResult = SDL_BlitSurface(tileSurface, NULL, drawingSurface, &rect)) != 0) {
				cout << "ERROR: Drawing tile " << maprow << "," << mapcol << " (" << blitResult << ")" << endl;
			}
		}
	}

	// finished drawing to drawingSurface
	return drawingSurface;
}
