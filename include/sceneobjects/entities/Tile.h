/**
 * Tile.h
 *
 * Author: nrudzicz
 * Created on: 23-May-2009
 */

#ifndef TILE_H_
#define TILE_H_

#include <vector>

#include <SDL.h>

#include "hud/HUD.h"
#include "sceneobjects/entities/Prop.h"

using namespace std;

enum class TileType { LAND, WATER };


class Tile {
private:
protected:
	HUD* m_hud;

	TileType m_tileType;
	SDL_Surface* m_surface;
	Prop* m_prop;
	HUDMessage* m_tileMessage;

	bool traversable;
	bool visited;
	bool nearWater;
	bool forgotten;

	void InitialiseStaticMessages();
	void CreateDrawingSurface();

public:
	Tile(TileType);
	virtual ~Tile();

	void DescribeYourself();

	SDL_Surface* GetSurface();

	inline TileType GetTileType() { return m_tileType; }
	inline void SetTileType(TileType t) { m_tileType = t; }

	void GiveProp(Prop*);
	bool HasProp();
	PropType GetPropType();

	bool IsTraversable();

	inline bool IsVisited() { return visited; }
	void Visit();

	void SetNearWater(bool);

	void Forget();
};

#endif /* TILE_H_ */
