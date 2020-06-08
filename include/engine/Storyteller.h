#ifndef STORYTELLER_H_
#define STORYTELLER_H_

#include <SDL.h>
#include <SDL_mixer.h>

#include "Vector2.h"
#include "engine/Timer.h"
#include "hud/HUD.h"
#include "sceneobjects/Map.h"
#include "sceneobjects/entities/Player.h"
#include "sceneobjects/entities/Prop.h"

const Uint32 DRAWN_TILE_SIZE = 8;		// IN PIXELS
const Uint32 DRAWN_TILE_RADIUS = 30;	// number of blocks to either side of centre
const Uint32 DRAWN_TILE_RADIUS_SQ = DRAWN_TILE_RADIUS * DRAWN_TILE_RADIUS;

// how fast we forget tiles, and how fast that rate increases
enum class ForgetRate { FORGET_SLOW = 4, FORGET_MED = 16, FORGET_FAST = 64, FORGET_FASTEST = 128 };
const Uint32 FORGET_TIME = 75;
const Uint32 FORGET_TIME_INCR = 5000;

// states for keyboard prompting/HUD display
enum class ControlState { FADEIN, WALK, LOOKPROMPT, PROPTALK, MARKTALK, MARKYES, MARKNO };

// types of riddles
enum class OddManProperty { NUM, SUB };


class Storyteller {
private:
	Storyteller();

	HUD* hud;
	SDL_Surface* drawingSurface;
	bool sceneIsDirty;

	Map* map;
	Player* player;
	Vector2Ui viewingCentre;

	Uint32 level;
	RiddleType currentLvlType;
	PropType propList[NUM_PROP_TYPES - 1];
	bool riddlePropSubmerged;
	int riddlePropNum;

	SDL_Surface* fadeSurface;
	int fadeLevel;
	bool fadeFromWhite;

	ControlState controlState;

	ForgetRate forgetRate;
	Uint32 lastForget;
	Uint32 lastForgetRateIncrease;

	// Sounds
	Mix_Chunk* insectLoop;
	int insectLoopChannel;
	Mix_Chunk* insectSpices[3];
	int insectSpiceChannel;
	Timer spiceTimer;
	Uint32 spiceTimeout;
	Mix_Chunk* bellChunk;
	int bellChannel;

	void CreateDrawingSurface();

	void RestartGame();
	void AdvanceLevel();

	void NewMap();
	void PopulateMap(Vector2Ui&);
	void PopulateFinalMap(Vector2Ui&);
	void FillCluster(Vector2Ui, PropType, int);
	void FillCluster(Vector2Ui, PropType, int, bool);
	bool ClusterHasEnoughType(Vector2Ui, TileType, int);
	void ClearMap();

	void RestartSpiceTimer();

	Vector2i GetPlayerHVDist();
	void RecenterPlayer();

	void ProcessWalk(Uint32, SDL_Event&);
	void ProcessLookPrompt(Uint32, SDL_Event&);
	void ProcessPropDisplay(Uint32, SDL_Event&);
	void ProcessMarkerDisplay(Uint32, SDL_Event&);
	void ProcessMarkerYes(Uint32, SDL_Event&);
	void ProcessMarkerNo(Uint32, SDL_Event&);

	void AnswerHowMany(SDLKey);
	void AnswerSubmerged(SDLKey);
	void AnswerOddMan(SDLKey);

protected:
public:
	virtual ~Storyteller();
	static Storyteller* Instance();

	void Init();

	void Process(Uint32, SDL_Event&);
	void Update(Uint32);

	SDL_Surface* DrawSceneSurface();
};

#endif /*STORYTELLER_H_*/
