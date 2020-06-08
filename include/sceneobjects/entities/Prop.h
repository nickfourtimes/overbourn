#ifndef PROP_H_
#define PROP_H_

#include <iostream>

#include <SDL.h>

#include "common.h"
#include "hud/HUD.h"
#include "hud/HUDMessage.h"

using namespace std;


enum class PropType { MARKER, TOADSTOOL, SHRUB, RUINS, FLOWER, LIVETREE, BERRY, TOMB, STUMP };
const int NUM_PROP_TYPES = 9;	// including marker

class Prop {
private:

protected:
	HUD* hud;

	PropType type;
	HUDMessage* hudMessage;
	int numNeighbours;

	SDL_Surface* surface;

	void CreateDrawingSurface();
	void CreateDescription();

public:
	Prop(PropType, int);
	virtual ~Prop();

	inline PropType GetPropType() const { return type; }
	inline int GetNumNeighbours() const { return numNeighbours; }

	virtual void DescribeYourself();

	inline SDL_Surface* GetSurface() const { return surface; }
};

ostream &operator <<(ostream&, const PropType&);

#endif /*PROP_H_*/
