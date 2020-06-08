#pragma once

#include <SDL.h>

class Timer {
private:
	Uint32 m_startTime;

protected:
public:
	void Start();
	Uint32 GetTicks();
	void Stop();
};
