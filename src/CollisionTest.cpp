#include "stdafx.h"

#include "AABB.h"
#include "CollisionTest.h"
#include "System.h"

#define MAPWIDTH 20
#define MAPHEIGHT 11
#define TILESIZE 64
#define PLAYERWIDTH 64
#define PLAYERHEIGHT 128
#define MAXSPEEDY 1000.f
#define MAXSPEEDX 640.f
#define JUMPSPEED -500.f
#define GRAVITY 1300.f
#define HORIZONTAL_ACCELERATION 6400.f
#define HORIZONTAL_FRICTION 1920.f

CollisionTest::CollisionTest(System& system)
{
	m_mapData.resize(MAPWIDTH * MAPHEIGHT);
	m_mapData = vector < int >
	{
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	};

	m_tileTexture = system.LoadTexture("tiles/creepybricks.png");
	m_backdrop = system.LoadTexture("backdrops/spooky.png");

	m_position.Set(128.0f, 224.0f);
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
					m_position.Set(128.0f, 224.0f);
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
		}
	}

	return false;
}

AABB currentBounds;
AABB newBounds;
AABB collisionBox;

void CollisionTest::Update(Uint32 ticks)
{
	float seconds = ticks / 1000.0f;

	if (m_update)
	{

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

			m_velocity.m_x = max(-MAXSPEEDX, min(m_velocity.m_x, MAXSPEEDX));

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

		m_velocity.m_y += GRAVITY * seconds;
		m_velocity.m_y = min(m_velocity.m_y, MAXSPEEDY);

		m_onGround = false;
		m_jump = false;
	}

	Vector2 move(m_velocity);
	move.Scale(seconds);

	// collision detection & response
	currentBounds.m_origin = m_position;
	currentBounds.m_origin.m_y -= (PLAYERHEIGHT * 0.5f);
	currentBounds.m_halfExtents.Set(PLAYERWIDTH * 0.5f, PLAYERHEIGHT * 0.5f);

	newBounds = currentBounds;
	newBounds.m_origin.Add(move);

	collisionBox = currentBounds;
	collisionBox.Union(newBounds);

	if (m_update)
	{
		m_position.Add(move);

		if (m_position.m_y > 640)
		{
			m_position.m_y = 640;
			m_onGround = true;
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
				SDL_RenderCopy(renderer, m_tileTexture, nullptr, &tileRect);
			}
		}
	}

	// Draw player
	SDL_Rect player = { static_cast<int>(m_position.m_x - PLAYERWIDTH * 0.5f), static_cast<int>(m_position.m_y - PLAYERHEIGHT), PLAYERWIDTH, PLAYERHEIGHT };
	SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);
	SDL_RenderFillRect(renderer, &player);

	// Diagnostics
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
}
