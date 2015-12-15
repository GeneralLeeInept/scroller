#pragma once

#include "IGameState.h"

#include "Collision.h"
#include "GameMap.h"
#include "Texture.h"
#include "Vector2.h"

class Aabb;
class GameController;
class Input;
class System;

typedef shared_ptr<class Sprite> SpritePtr;

class CollisionTest : public IGameState
{
public:
	CollisionTest(System& system, GameController& gameController, Input& input);

	void Update(Uint32 ticks);
	void Draw(SDL_Renderer* renderer);

private:
	void CollisionCheck(Vector2& move, Vector2& posCorrect, float seconds);
	bool InternalEdge(const Collision::Hit& hit, int tileX, int tileY);

	GameController& m_gameController;
	Input& m_input;
	vector<TexturePtr> m_tileTextures;
	TexturePtr m_backdrop;
	GameMap m_mapData;
	SpritePtr m_sprite;
	Vector2 m_position;
	Vector2 m_velocity;
	bool m_onGround = false;
	bool m_update = true;
	bool m_diagnostics = false;
	int m_cameraX = 0;
	int m_cameraY = 0;
};