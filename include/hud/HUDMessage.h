/**
 * HUDMessage.h
 *
 * Author: nrudzicz
 * Created on: 28-May-2009
 */

#ifndef HUDMESSAGE_H_
#define HUDMESSAGE_H_

#include <vector>
#include <SDL.h>

using namespace std;

enum class MsgPlacement { PLACE_BOTTOM, PLACE_LEFT_CENTRE };


class HUDMessage {
private:
	void PrepareMultiLineSurface();

protected:
	vector<const char*> msgVect;
	SDL_Surface* msgSurface;
	MsgPlacement placement;
	SDL_Rect screenPos;

public:
	HUDMessage(vector<const char*>, MsgPlacement);
	HUDMessage(HUDMessage*);
	virtual ~HUDMessage();

	SDL_Surface* GetMessageSurface();
	SDL_Rect* GetScreenPos();
	void SetScreenPos(int, int);

	void DestroyMessage();
};

#endif /* HUDMESSAGE_H_ */
