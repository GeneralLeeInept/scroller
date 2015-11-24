#pragma once

class IGameState;
class System;

class GameController
{
public:
	GameController(System& system);

	void Run();
	void Quit();

	void GotoState(shared_ptr<IGameState> nextState);

private:
	System& m_system;
	shared_ptr<IGameState> m_currentState;
	shared_ptr<IGameState> m_nextState;
	bool m_quit = false;
};