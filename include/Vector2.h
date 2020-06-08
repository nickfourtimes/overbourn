#pragma once

#include <SDL.h>

class Vector2i {
private:
	int m_x = 0, m_y = 0;

public:
	Vector2i();
	Vector2i(int x, int y);
	virtual ~Vector2i();

	void SetPos(int x, int y);
	int X();
	int Y();
};


class Vector2Ui {
private:
	Uint32 m_x = 0, m_y = 0;

public:
	Vector2Ui();
	Vector2Ui(Uint32 x, Uint32 y);
	virtual ~Vector2Ui();

	void SetPos(Uint32 x, Uint32 y);
	Uint32 X();
	Uint32 Y();
};
