#include "stdafx.h"

#include "Input.h"

Input::Input()
{
	for (int i = 0; i < 2; ++i)
	{
		m_keyStates[i].resize(SDL_NUM_SCANCODES, 0);
	}

	m_current = 0;
}

void Input::Update()
{
	m_current ^= 1;
	const Uint8* keyStates = SDL_GetKeyboardState(nullptr);
	copy(keyStates, keyStates + SDL_NUM_SCANCODES, m_keyStates[m_current].begin());
}

bool Input::PreviousItem() const
{
	return KeyPressed(SDL_SCANCODE_UP);
}

bool Input::NextItem() const
{
	return KeyPressed(SDL_SCANCODE_DOWN);
}

bool Input::SelectItem() const
{
	return KeyReleased(SDL_SCANCODE_SPACE);
}

bool Input::Left() const
{
	return KeyDown(SDL_SCANCODE_LEFT);
}

bool Input::Right() const
{
	return KeyDown(SDL_SCANCODE_RIGHT);
}

bool Input::Jump() const
{
	return KeyDown(SDL_SCANCODE_SPACE);
}

bool Input::KeyDown(int scanCode) const
{
	return m_keyStates[m_current][scanCode] ? true : false;
}

bool Input::KeyPressed(int scanCode) const
{
	bool isDown = m_keyStates[m_current][scanCode] ? true : false;
	bool wasDown = m_keyStates[m_current ^ 1][scanCode] ? true : false;
	return isDown && !wasDown;
}

bool Input::KeyReleased(int scanCode) const
{
	bool isDown = m_keyStates[m_current][scanCode] ? true : false;
	bool wasDown = m_keyStates[m_current ^ 1][scanCode] ? true : false;
	return !isDown && wasDown;
}
