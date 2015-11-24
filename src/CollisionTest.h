#pragma once

#include "Collision.h"
#include "Texture.h"
#include "Vector2.h"

class Aabb;
class System;

class CollisionTest
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
	vector<int> m_mapData;
	Vector2 m_position;
	Vector2 m_velocity;
	bool m_onGround = false;
	bool m_jump = true;
	bool m_left = false;
	bool m_right = false;
	bool m_update = false;
	bool m_diagnostics = false;
};