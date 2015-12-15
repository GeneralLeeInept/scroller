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

private:
	vector<Uint8> m_keyStates[2];
	int m_current;
};