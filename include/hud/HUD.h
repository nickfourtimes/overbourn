/**
 * HUD.h
 *
 * Author: nrudzicz
 * Created on: 28-May-2009
 */

#ifndef HUD_H_
#define HUD_H_

#include <queue>

#include <SDL.h>
#include <SDL_ttf.h>

#include "hud/HUDMessage.h"

using namespace std;

class HUD {
private:
	HUD();	// singleton

	queue<HUDMessage*> messageQueue;

	TTF_Font* mainFont;

	SDL_Surface* maskSurface;

	void LoadMainFont();
	void CreateMaskSurface();

protected:
public:
	virtual ~HUD();
	static HUD* Instance();

	void Init();

	bool IsDisplaying();
	bool IsLastMessage();

	TTF_Font* GetMainFont() const;
	SDL_Surface* GetMaskSurface();

	void PushMessage(HUDMessage*);
	HUDMessage* Front();
	void PopMessage();
};

#endif /* HUD_H_ */
