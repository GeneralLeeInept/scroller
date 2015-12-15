#pragma once

class IGameState
{
public:
	virtual ~IGameState() = default;

	virtual void Update(Uint32 ms) = 0;
	virtual void Draw(SDL_Renderer* renderer) = 0;
};