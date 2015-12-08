#pragma once

class IGameState;
class System;

class GameController
{
public:
	enum GameState
	{
		kNone,
		kMainMenu,
		kNewGame,
		kMapEditor,
		kSpriteEditor
	};

	GameController(System& system);

	void Run();
	void Quit();

	void GotoState(GameState nextState);

private:
	void DoStateChange();

	System& m_system;
	shared_ptr<IGameState> m_currentState;
	GameState m_nextState = kMainMenu;
	bool m_quit = false;
};