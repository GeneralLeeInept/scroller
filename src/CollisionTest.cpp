#include "stdafx.h"

#include "Aabb.h"
#include "Collision.h"
#include "CollisionTest.h"
#include "System.h"

#define MAPWIDTH 20
#define MAPHEIGHT 11
#define TILESIZE 64
#define PLAYERWIDTH 56
#define PLAYERHEIGHT 112
#define MAXSPEEDY 1000.f
#define MAXSPEEDX 640.f
#define JUMPSPEED -500.f
#define GRAVITY 1300.f
#define HORIZONTAL_ACCELERATION 6400.f
#define HORIZONTAL_FRICTION 1920.f
#define HORIZONTAL_ACCELERATION_AIR 3200.f

CollisionTest::CollisionTest(System& system)
{
	m_mapData.resize(MAPWIDTH * MAPHEIGHT);
	m_mapData = vector < int >
	{
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	};

	m_tileTextures.push_back(system.LoadTexture("tiles/creepybricks.png"));
	m_tileTextures.push_back(system.LoadTexture("tiles/creepybrickstop.png"));
	m_backdrop = system.LoadTexture("backdrops/spooky.png");

	m_position.Set(128.0f, 192.0f);
}

bool CollisionTest::HandleEvent(SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
	{
		SDL_KeyboardEvent& ke = reinterpret_cast<SDL_KeyboardEvent&>(e);

		switch (ke.keysym.scancode)
		{
			case SDL_SCANCODE_R:
			{
				if (e.type == SDL_KEYDOWN)
				{
					m_position.Set(128.0f, 192.0f);
					//m_position.Set(128.0f, 224.0f);
					m_velocity.Set(0.0f, 0.0f);
					m_onGround = false;
					m_jump = false;
					m_left = m_right = false;
				}

				return true;
			}

			case SDL_SCANCODE_SPACE:
			{
				if (e.type == SDL_KEYDOWN)
				{
					m_jump = true;
				}

				return true;
			}

			case SDL_SCANCODE_LEFT:
			{
				m_left = (e.type == SDL_KEYDOWN);
				return true;
			}

			case SDL_SCANCODE_RIGHT:
			{
				m_right = (e.type == SDL_KEYDOWN);
				return true;
			}

			case SDL_SCANCODE_TAB:
			{
				m_update = (e.type == SDL_KEYDOWN) ? !m_update : m_update;
				return true;
			}

			case SDL_SCANCODE_D:
			{
				m_diagnostics = (e.type == SDL_KEYDOWN) ? !m_diagnostics : m_diagnostics;
				return true;
			}
		}
	}

	return false;
}

Aabb currentBounds;
Aabb newBounds;
Aabb collisionBox;
vector<Aabb> collideTiles;

void CollisionTest::Update(Uint32 ticks)
{
	float seconds = ticks / 1000.0f;

	if (m_update)
	{
		collideTiles.clear();

		if (m_onGround)
		{
			if (m_jump)
			{
				m_velocity.m_y = JUMPSPEED;
				m_onGround = false;
				//m_update = false;
			}

			if (m_left)
			{
				m_velocity.m_x -= HORIZONTAL_ACCELERATION * seconds;
			}

			if (m_right)
			{
				m_velocity.m_x += HORIZONTAL_ACCELERATION * seconds;
			}
		}
		else
		{
			if (m_left)
			{
				m_velocity.m_x -= HORIZONTAL_ACCELERATION_AIR * seconds;
			}

			if (m_right)
			{
				m_velocity.m_x += HORIZONTAL_ACCELERATION_AIR * seconds;
			}
		}

		m_velocity.m_x = max(-MAXSPEEDX, min(m_velocity.m_x, MAXSPEEDX));
		m_velocity.m_y += GRAVITY * seconds;
		m_velocity.m_y = max(-MAXSPEEDY, min(m_velocity.m_y, MAXSPEEDY));

		m_onGround = false;
		m_jump = false;
	}

	Vector2 move;
	move.Scale(m_velocity, seconds);

	// collision detection & response
	Vector2 posCorrect;
	CollisionCheck(move, posCorrect, seconds);

	if (m_update)
	{
		move.Add(m_velocity, posCorrect);
		move.Scale(seconds);
		m_position.Add(move);

		if (m_velocity.m_y < FLT_EPSILON && m_velocity.m_y > -FLT_EPSILON)
		{
			m_velocity.m_y = 0.0f;
			m_onGround = true;
		}

		if (m_onGround)
		{
			if (m_velocity.m_x > 64.f)
			{
				m_velocity.m_x -= HORIZONTAL_FRICTION * seconds;
			}
			else if (m_velocity.m_x < -64.f)
			{
				m_velocity.m_x += HORIZONTAL_FRICTION * seconds;
			}
			else
			{
				m_velocity.m_x = 0.f;
			}
		}
	}
}

void CollisionTest::Draw(SDL_Renderer* renderer)
{
	SDL_Rect screenRect = { 0, 0, 1280, 720 };
	SDL_RenderCopy(renderer, m_backdrop, nullptr, &screenRect);

	// Draw map
	SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255);

	for (int y = 0; y < MAPHEIGHT; ++y)
	{
		for (int x = 0; x < MAPWIDTH; ++x)
		{
			int tile = m_mapData.at(x + y * MAPWIDTH);

			if (tile)
			{
				SDL_Rect tileRect = { x * TILESIZE, y * TILESIZE, TILESIZE, TILESIZE };
				SDL_RenderCopy(renderer, m_tileTextures[tile - 1], nullptr, &tileRect);
			}
		}
	}

	// Draw player
	SDL_Rect player = { static_cast<int>(m_position.m_x - PLAYERWIDTH * 0.5f), static_cast<int>(m_position.m_y - PLAYERHEIGHT), PLAYERWIDTH, PLAYERHEIGHT };
	SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);
	SDL_RenderFillRect(renderer, &player);

	// Diagnostics
	if (m_diagnostics)
	{
		SDL_Rect drawRect;
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		currentBounds.ToRect(drawRect);
		SDL_RenderDrawRect(renderer, &drawRect);
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		newBounds.ToRect(drawRect);
		SDL_RenderDrawRect(renderer, &drawRect);
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		collisionBox.ToRect(drawRect);
		SDL_RenderDrawRect(renderer, &drawRect);

		int minTileX = static_cast<int>(collisionBox.MinX()) >> 6;
		int minTileY = static_cast<int>(collisionBox.MinY()) >> 6;
		int maxTileX = (static_cast<int>(collisionBox.MaxX()) + 63) >> 6;
		int maxTileY = (static_cast<int>(collisionBox.MaxY()) + 63) >> 6;
		SDL_Rect testTilesRect = { minTileX * 64, minTileY * 64, (maxTileX - minTileX) * 64, (maxTileY - minTileY) * 64 };
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 64);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_RenderFillRect(renderer, &testTilesRect);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

		for (auto tile : collideTiles)
		{
			SDL_Rect drawRect;
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
			tile.ToRect(drawRect);
			SDL_RenderFillRect(renderer, &drawRect);
		}
	}
}

void CollisionTest::CollisionCheck(Vector2& move, Vector2& posCorrect, float seconds)
{
	currentBounds.m_origin = m_position;
	currentBounds.m_origin.m_y -= (PLAYERHEIGHT * 0.5f);
	currentBounds.m_halfExtents.Set(PLAYERWIDTH * 0.5f, PLAYERHEIGHT * 0.5f);

	Aabb collisionBoxes[2];
	collisionBoxes[0].m_origin.Set(m_position.m_x, m_position.m_y - PLAYERHEIGHT * 0.25f);
	collisionBoxes[0].m_halfExtents.Mul(currentBounds.m_halfExtents, Vector2(1.0f, 0.5f));
	collisionBoxes[1].m_origin.Set(m_position.m_x, m_position.m_y - PLAYERHEIGHT * 0.75f);
	collisionBoxes[1].m_halfExtents.Mul(currentBounds.m_halfExtents, Vector2(1.0f, 0.5f));

	newBounds = currentBounds;
	newBounds.m_origin.Add(move);

	collisionBox = currentBounds;
	collisionBox.Union(newBounds);

	int minTileX = max(static_cast<int>(collisionBox.MinX()) >> 6, 0);
	int minTileY = max(static_cast<int>(collisionBox.MinY()) >> 6, 0);
	int maxTileX = min((static_cast<int>(collisionBox.MaxX()) + 63) >> 6, MAPWIDTH - 1);
	int maxTileY = min((static_cast<int>(collisionBox.MaxY()) + 63) >> 6, MAPHEIGHT - 1);

	for (int tileY = minTileY; tileY <= maxTileY; ++tileY)
	{
		for (int tileX = minTileX; tileX <= maxTileX; ++tileX)
		{
			int tile = m_mapData.at(tileY * MAPWIDTH + tileX);

			if (tile)
			{
				Aabb tileAabb;
				tileAabb.m_origin.Set((tileX + 0.5f) * TILESIZE, (tileY + 0.5f) * TILESIZE);
				tileAabb.m_halfExtents.Set(TILESIZE * 0.5f, TILESIZE * 0.5f);

				for (int box = 0; box < 2; ++box)
				{
					Collision::Hit hit;
					Collision::HitFilter hitFilter = bind(&CollisionTest::InternalEdge, this, placeholders::_1, tileX, tileY);

					if (Collision::AabbVsAabb(collisionBoxes[box], tileAabb, hitFilter, hit))
					{
						float separation = max(hit.m_distance, 0.0f);
						float penetration = min(hit.m_distance, 0.0f);
						float nv = m_velocity.Dot(hit.m_normal) + separation / seconds;

						Vector2 extrude(hit.m_normal);
						extrude.Scale(penetration / seconds);
						posCorrect.Sub(extrude);

						if (nv < 0)
						{
							collideTiles.push_back(collisionBoxes[box]);
							collideTiles.push_back(tileAabb);
							hit.m_normal.Scale(nv);
							m_velocity.Sub(hit.m_normal);
						}
					}
				}
			}
		}
	}
}

bool CollisionTest::InternalEdge(const Collision::Hit& hit, int tileX, int tileY)
{
	int neighbourX = static_cast<int>(tileX + hit.m_normal.m_x);
	int neighbourY = static_cast<int>(tileY + hit.m_normal.m_y);

	if (neighbourX >= MAPWIDTH || neighbourY >= MAPHEIGHT)
	{
		return false;
	}

	int tile = m_mapData.at(neighbourY * MAPWIDTH + neighbourX);
	return (tile != 0);
}
