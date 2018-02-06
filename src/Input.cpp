#include "stdafx.h"

#include "Input.h"

Input::Input()
{
    for (int i = 0; i < 2; ++i)
    {
        m_keyStates[i].resize(SDL_NUM_SCANCODES, 0);
    }

    m_current = 0;

    m_mouseButtons[0] = 0;
    m_mouseButtons[1] = 0;

    SDL_GetMouseState(&m_mouseX, &m_mouseY);
}

void Input::Update()
{
    m_current ^= 1;
    const Uint8* keyStates = SDL_GetKeyboardState(nullptr);
    copy(keyStates, keyStates + SDL_NUM_SCANCODES, m_keyStates[m_current].begin());
    m_mouseButtons[m_current] = SDL_GetMouseState(&m_mouseX, &m_mouseY);
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

bool Input::MouseButtonDown(int button) const
{
    return (m_mouseButtons[m_current] & SDL_BUTTON(button)) ? true : false;
}

bool Input::MouseButtonPressed(int button) const
{
    bool isDown = (m_mouseButtons[m_current] & SDL_BUTTON(button)) ? true : false;
    bool wasDown = (m_mouseButtons[m_current ^ 1] & SDL_BUTTON(button)) ? true : false;
    return isDown && !wasDown;
}

bool Input::MouseButtonReleased(int button) const
{
    bool isDown = (m_mouseButtons[m_current] & SDL_BUTTON(button)) ? true : false;
    bool wasDown = (m_mouseButtons[m_current ^ 1] & SDL_BUTTON(button)) ? true : false;
    return isDown && wasDown;
}

void Input::GetMousePosition(int* x, int* y) const
{
    if (x)
    {
        *x = m_mouseX;
    }

    if (y)
    {
        *y = m_mouseY;
    }
}
