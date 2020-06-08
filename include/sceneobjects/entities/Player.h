/**
 * Player.h
 *
 * Author: nrudzicz
 * Created on: 25-May-2009
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <SDL.h>

const Uint32 MOVE_REPEAT = 150;

class Player {
private:
	SDL_Surface* drawingSurface;
	Map* map;
	Vector2Ui pos;

	// move the player
	bool moveUp, moveDown, moveLeft, moveRight;
	Uint32 lastMoveUp, lastMoveDown, lastMoveLeft, lastMoveRight;

	// whether the player has moved or not
	bool dirty;

	void CreateDrawingSurface();

protected:

public:
	Player();
	virtual ~Player();

	inline Vector2Ui GetPos() { return pos; }
	inline void SetPos(Uint32 x, Uint32 y) { pos.SetPos(x, y); }

	inline void MoveUp() { moveUp = true; lastMoveUp = SDL_GetTicks(); }
	inline void StopMoveUp() { moveUp = false; }
	inline void MoveDown() { moveDown = true; lastMoveDown = SDL_GetTicks(); }
	inline void StopMoveDown() { moveDown = false; }
	inline void MoveLeft() { moveLeft = true; lastMoveLeft = SDL_GetTicks(); }
	inline void StopMoveLeft() { moveLeft = false; }
	inline void MoveRight() { moveRight = true; lastMoveRight = SDL_GetTicks(); }
	inline void StopMoveRight() { moveRight = false; }

	inline void SetMap(Map* m) { map = m; }

	bool IsDirty();

	void Process(Uint32, SDL_Event&);
	void Update(Uint32);
	void Render(Uint32);
	SDL_Surface* GetAgentSurface();
};

#endif /* PLAYER_H_ */
