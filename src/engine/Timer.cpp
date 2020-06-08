#include "engine/Timer.h"


void Timer::Start() {
	m_startTime = SDL_GetTicks();
	return;
}


Uint32 Timer::GetTicks() {
	return SDL_GetTicks() - m_startTime;
}


void Timer::Stop() {
	// actually i'm not sure this is even needed (or at least, not presently used)
	return;
}
