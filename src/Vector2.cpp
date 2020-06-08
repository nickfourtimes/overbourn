#include "Vector2.h"


/************************************************************************************************** {CON|DE}STRUCTORS */

Vector2i::Vector2i() {
	m_x = m_y = 0;
	return;
}


Vector2i::Vector2i(int x, int y) {
	x = x;
	y = y;
	return;
}


Vector2i::~Vector2i() {
	return;
}


Vector2Ui::Vector2Ui() {
	m_x = m_y = 0;
	return;
}


Vector2Ui::Vector2Ui(Uint32 x, Uint32 y) {
	m_x = x;
	m_y = y;
	return;
}


Vector2Ui::~Vector2Ui() {
	return;
}


/************************************************************************************************** METHODS */

void Vector2i::SetPos(int x, int y) {
	m_x = x;
	m_y = y;
	return;
}


int Vector2i::X() {
	return m_x;
}


int Vector2i::Y() {
	return m_y;
}


void Vector2Ui::SetPos(Uint32 x, Uint32 y) {
	m_x = x;
	m_y = y;
	return;
}


Uint32 Vector2Ui::X() {
	return m_x;
}


Uint32 Vector2Ui::Y() {
	return m_y;
}
