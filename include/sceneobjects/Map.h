/**
 * Map.h
 *
 * Author: nrudzicz
 * Created on: 9-May-2009
 */

#ifndef MAP_H_
#define MAP_H_

#include <SDL.h>

#include "common.h"
#include "Vector2.h"
#include "entities/Tile.h"


class Map {
private:
	Uint32 numTilesPerSide;
	Tile*** map;
	Uint32 forgetInd;
	Uint32* forgetTiles;

	// display variables
	SDL_Surface* drawingSurface;

	// the process of generating a new map
	void Seed(bool**, Uint32, float);
	void DetailIteration(bool**, Uint32);
	void FinaliseMap(bool**);

	// place objects on the map
	void DoObjectPlacement();

	// create a drawing surface of appropriate size
	void GetNewDrawingSurface();

	// finished with a given map
	void ClearMap();

public:
	Map();
	virtual ~Map();

	int Generate(Uint32, Uint32, float);

	inline Uint32 GetNumTilesPerSide() { return numTilesPerSide; }

	Tile* GetUpTile(Vector2Ui);
	Tile* GetDnTile(Vector2Ui);
	Tile* GetLtTile(Vector2Ui);
	Tile* GetRtTile(Vector2Ui);
	Tile* GetTile(Vector2Ui);

	void Forget(int);
	bool AllForgotten();

	/**
	 * Draw a portion of the map.
	 * @param centre The centre point of drawing
	 */
	SDL_Surface* DrawMapSurface(Vector2Ui centre);
};

#endif /* MAP_H_ */
