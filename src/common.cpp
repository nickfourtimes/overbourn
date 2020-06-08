/**
 * common.cpp
 *
 * Author: nrudzicz
 * Created on: 10-May-2009
 */

#include <stdlib.h>

#include "common.h"


/************************************************************************************************** PUBLIC FUNCTIONS */

bool IsPowerOfTwo(int i) {
	return (i & (i - 1)) == 0;
}


bool RandomTrueFalse(float t_prob) {
	return (rand() % 1001) < t_prob * 1000.0f;
}


int RandomInt(int n) {
	int limit = RAND_MAX - RAND_MAX % n;
	int rnd;

	do {
		rnd = rand();
	} while (rnd >= limit);

	return rnd % n;
}
