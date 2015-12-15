#include "stdafx.h"

#include "Aabb.h"
#include "Collision.h"
#include "CollisionTest.h"
#include "GameController.h"
#include "Input.h"
#include "Sprite.h"
#include "System.h"

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
#define MINCAMERAX ((1280 / 2) + (16 * TILESIZE))
#define MAXCAMERAX (MINCAMERAX + (160 - 32) * TILESIZE)

static SpriteDefinition spriteDef;

CollisionTest::CollisionTest(System& system, GameController& gameController, Input& input)
	: m_gameController(gameController)
	, m_input(input)
{
	m_mapData.Load("maps/test.map", system);
	m_position.Set(19.5f * 64.0f, 18.0f * 64.0f);
	spriteDef.Load("sprites/superryan.spr", system);
	m_sprite = make_unique<Sprite>(spriteDef);
	m_sprite->PlaySequence(0);
}
#if 0
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
					m_position.Set(19.5f * 64.0f, 18.0f * 64.0f);
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
				//m_update = (e.type == SDL_KEYDOWN) ? !m_update : m_update;
				return true;
			}

			case SDL_SCANCODE_D:
			{
				m_diagnostics = (e.type == SDL_KEYDOWN) ? !m_diagnostics : m_diagnostics;
				return true;
			}

			case SDL_SCANCODE_X:
			{
				if (e.type == SDL_KEYUP)
				{
					m_gameController.GotoState(GameController::kMainMenu);
				}

				return true;
			}
		}
	}

	return false;
}
#endif

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
			if (m_input.Jump())
			{
				m_velocity.m_y = JUMPSPEED;
				m_onGround = false;
			}

			if (m_input.Left())
			{
				m_velocity.m_x -= HORIZONTAL_ACCELERATION * seconds;
			}

			if (m_input.Right())
			{
				m_velocity.m_x += HORIZONTAL_ACCELERATION * seconds;
			}
		}
		else
		{
			if (m_input.Left())
			{
				m_velocity.m_x -= HORIZONTAL_ACCELERATION_AIR * seconds;
			}

			if (m_input.Right())
			{
				m_velocity.m_x += HORIZONTAL_ACCELERATION_AIR * seconds;
			}
		}

		m_velocity.m_x = max(-MAXSPEEDX, min(m_velocity.m_x, MAXSPEEDX));
		m_velocity.m_y += GRAVITY * seconds;
		m_velocity.m_y = max(-MAXSPEEDY, min(m_velocity.m_y, MAXSPEEDY));

		m_onGround = false;
	}

	Vector2 move;
	move.Scale(m_velocity, seconds);

	// collision detection & response
	Vector2 posCorrect;
	CollisionCheck(move, posCorrect, seconds);
	
	Vector2 prevPosition = m_position;

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

	m_cameraX = static_cast<int>(m_position.m_x) - 640;// max(MINCAMERAX, min(MAXCAMERAX, static_cast<int>(m_position.m_x)));
	m_cameraY = static_cast<int>(m_position.m_y) - 360;

	SDL_Point& pos = m_sprite->Position();
	int prevX = pos.x;

	pos.x = static_cast<int>(m_position.m_x) - m_cameraX;
	pos.y = static_cast<int>(m_position.m_y) - m_cameraY;

	if (m_input.Left())
	{
		m_sprite->SetFlip(true);
	}
	else if (m_input.Right())
	{
		m_sprite->SetFlip(false);
	}

	m_sprite->Update(ticks);
}

void CollisionTest::Draw(SDL_Renderer* renderer)
{
	// Draw map
	m_mapData.Draw(renderer, m_cameraX, m_cameraY);

	// Draw player
	m_sprite->Draw(renderer);

	// Diagnostics
	if (m_diagnostics)
	{
		SDL_Rect playerRect = { static_cast<int>(m_position.m_x - PLAYERWIDTH * 0.5f), static_cast<int>(m_position.m_y - PLAYERHEIGHT), PLAYERWIDTH, PLAYERHEIGHT };
		playerRect.x -= m_cameraX;
		playerRect.y -= m_cameraY;
		SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);
		SDL_RenderFillRect(renderer, &playerRect);

		SDL_Rect drawRect;
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		currentBounds.ToRect(drawRect);
		drawRect.x -= m_cameraX;
		drawRect.y -= m_cameraY;
		SDL_RenderDrawRect(renderer, &drawRect);
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		newBounds.ToRect(drawRect);
		drawRect.x -= m_cameraX;
		drawRect.y -= m_cameraY;
		SDL_RenderDrawRect(renderer, &drawRect);
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
		collisionBox.ToRect(drawRect);
		drawRect.x -= m_cameraX;
		drawRect.y -= m_cameraY;
		SDL_RenderDrawRect(renderer, &drawRect);

		int minTileX = static_cast<int>(collisionBox.MinX()) >> 6;
		int minTileY = static_cast<int>(collisionBox.MinY()) >> 6;
		int maxTileX = (static_cast<int>(collisionBox.MaxX()) + 63) >> 6;
		int maxTileY = (static_cast<int>(collisionBox.MaxY()) + 63) >> 6;
		SDL_Rect testTilesRect = { minTileX * 64, minTileY * 64, (maxTileX - minTileX) * 64, (maxTileY - minTileY) * 64 };
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 64);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		testTilesRect.x -= m_cameraX;
		testTilesRect.y -= m_cameraY;
		SDL_RenderFillRect(renderer, &testTilesRect);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

		for (auto tile : collideTiles)
		{
			SDL_Rect drawRect;
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
			tile.ToRect(drawRect);
			drawRect.x -= m_cameraX;
			drawRect.y -= m_cameraY;
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
	int maxTileX = min((static_cast<int>(collisionBox.MaxX()) + 63) >> 6, m_mapData.GetWidth() - 1);
	int maxTileY = min((static_cast<int>(collisionBox.MaxY()) + 63) >> 6, m_mapData.GetHeight() - 1);

	for (int tileY = minTileY; tileY <= maxTileY; ++tileY)
	{
		for (int tileX = minTileX; tileX <= maxTileX; ++tileX)
		{
			int tile = m_mapData.GetTile(GameMap::kPlayground, tileX, tileY);

			if (tile != GameMap::kEmptyTile)
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

	if (neighbourX <= 0 || neighbourX >= m_mapData.GetWidth() || neighbourY <= 0 || neighbourY >= m_mapData.GetHeight())
	{
		return false;
	}

	int tile = m_mapData.GetTile(GameMap::kPlayground, neighbourX, neighbourY);
	return (tile != GameMap::kEmptyTile);
}
