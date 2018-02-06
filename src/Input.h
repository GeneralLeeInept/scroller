#pragma once

class Input
{
public:
    Input();

    void Update();

    // Menu controls
    bool PreviousItem() const;
    bool NextItem() const;
    bool SelectItem() const;

    // Game controls
    bool Left() const;
    bool Right() const;
    bool Jump() const;

    // Raw
    bool KeyDown(int scanCode) const;
    bool KeyPressed(int scanCode) const;
    bool KeyReleased(int scanCode) const;
    bool MouseButtonDown(int button) const;
    bool MouseButtonPressed(int button) const;
    bool MouseButtonReleased(int button) const;
    void GetMousePosition(int* x, int* y) const;

private:
    vector<Uint8> m_keyStates[2];
    int m_current;
    Uint32 m_mouseButtons[2];
    int m_mouseX;
    int m_mouseY;
};
