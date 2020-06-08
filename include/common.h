/**
 * common.h
 *
 * Author: nrudzicz
 * Created on: 9-May-2009
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <cstdlib>
#include <SDL.h>


 /************************************************************************************************** CONSTANTS */

const int RETURN_ERROR = -1;
const int RETURN_SUCCESS = 0;

const bool DEBUG = false;

// *** some handy-dandy string literals ***
const char* const LOG_FLUFF = "OVERBOURN: ";

// *** video properties ***
const bool FULLSCREEN = true;
const int WINDOW_SIZE = 480;
const int SCREEN_BPP = 32;

// *** other properties ***
const int MAX_FRAME_RATE = 30;	//FPS


// *** masks for SDL_Surface's ***
#if SDL_BYTEORDER == SDL_BIGENDIAN
	const Uint32 RMASK = 0xff000000;
	const Uint32 GMASK = 0x00ff0000;
	const Uint32 BMASK = 0x0000ff00;
	const Uint32 AMASK = 0x000000ff;
#else
	const Uint32 RMASK = 0x000000ff;
	const Uint32 GMASK = 0x0000ff00;
	const Uint32 BMASK = 0x00ff0000;
	const Uint32 AMASK = 0xff000000;
#endif

// *** some common SDL colours ***
const SDL_Color SDL_CLR_WHITE = { 255, 255, 255 };
const SDL_Color SDL_CLR_BLACK = { 0, 0, 0 };
const SDL_Color SDL_CLR_RED = { 255, 0, 0 };
const SDL_Color SDL_CLR_GREEN = { 0, 255, 0 };
const SDL_Color SDL_CLR_BLUE = { 0, 0, 255 };

// *** game-specific commons ***
enum class RiddleType { HOWMANY, SUBMERGED, ODDMAN };


/************************************************************************************************** MATH FUNCTIONS */

/**
 * Determine if a number is a perfect power of two
 */
bool IsPowerOfTwo(int);


template<class T>
T Clamp(T val, T min, T max) {
	return (val < min) ? min : (val > max) ? max : val;
}


/************************************************************************************************** FUNCTIONS */

/**
 * Get a boolean with a certain probability of true-ness
 * @param t_prob Probability of a "true" being generated [0..1]
 *
 * Note: We assume random seed has already been set
 */
bool RandomTrueFalse(float t_prob);


/**
 * A bit of randomness specifically for the FY-shuffle, below
 */
int RandomInt(int n);


/**
 * Randomly shuffle a given array
 */
template<class T>
void FisherYatesShuffle(T* ar, int numElements) {
	int i, j;
	T tmp;

	for (i = numElements - 1; i > 0; --i) {
		j = RandomInt(i + 1);
		tmp = ar[j];
		ar[j] = ar[i];
		ar[i] = tmp;
	}

	return;
}

#endif /* COMMON_H_ */
