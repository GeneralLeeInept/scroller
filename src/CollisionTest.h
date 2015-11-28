#pragma once

#include "IGameState.h"

#include "Collision.h"
#include "GameMap.h"
#include "Texture.h"
#include "Vector2.h"

class Aabb;
class System;

class CollisionTest : public IGameState
{
public:
	CollisionTest(System& system);

	bool HandleEvent(SDL_Event& e);
	void Update(Uint32 ticks);
	void Draw(SDL_Renderer* renderer);

private:
	void CollisionCheck(Vector2& move, Vector2& posCorrect, float seconds);
	bool InternalEdge(const Collision::Hit& hit, int tileX, int tileY);

	vector<TexturePtr> m_tileTextures;
	TexturePtr m_backdrop;
	GameMap m_mapData;
	Vector2 m_position;
	Vector2 m_velocity;
	bool m_onGround = false;
	bool m_jump = true;
	bool m_left = false;
	bool m_right = false;
	bool m_update = false;
	bool m_diagnostics = false;
	int m_cameraX = 0;
	int m_cameraY = 0;
};