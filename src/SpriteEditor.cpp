#include "stdafx.h"

#include "GameController.h"
#include "Input.h"
#include "Sprite.h"
#include "SpriteEditor.h"
#include "System.h"

static SpriteDefinition testSpriteDef;

SpriteEditor::SpriteEditor(const System& system, GameController& gameController, Input& input)
    : m_system(system)
    , m_gameController(gameController)
    , m_input(input)
{
    MakeTestSprite(system);
}

void SpriteEditor::update_input()
{
    if (m_input.KeyReleased(SDL_SCANCODE_X))
    {
        m_gameController.GotoState(GameController::kMainMenu);
    }

    if (m_input.KeyReleased(SDL_SCANCODE_S))
    {
        testSpriteDef.Save("sprites/superryan.spr");
    }

    m_left = m_input.KeyDown(SDL_SCANCODE_LEFT);
    m_right = m_input.KeyDown(SDL_SCANCODE_RIGHT);
}

void SpriteEditor::Update(Uint32 ms)
{
    update_input();

    m_sprite->Update(ms);

    SDL_Point& pos = m_sprite->Position();
    int prevX = pos.x;

    if (m_left)
    {
        pos.x -= 3;
    }

    if (m_right)
    {
        pos.x += 3;
    }

    if (pos.x != prevX)
    {
        m_sprite->SetFlip(prevX > pos.x);
    }
}

void SpriteEditor::Draw(SDL_Renderer* renderer)
{
    //Clear screen
    SDL_SetRenderDrawColor(renderer, 84, 200, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    //Draw sprite
    m_sprite->Draw(renderer);
}

void SpriteEditor::MakeTestSprite(const System& system)
{
    testSpriteDef.m_frames.resize(22);

    for (int frame = 0; frame < 22; ++frame)
    {
        SDL_Rect& dimensions = testSpriteDef.m_frames[frame].m_dimensions;
        dimensions.x = (frame & 0xf) << 7;
        dimensions.y = (frame >> 4) << 8;
        dimensions.w = 128;
        dimensions.h = 256;

        SDL_Point& origin = testSpriteDef.m_frames[frame].m_origin;
        origin.x = 75;
        origin.y = 234;
    }

    testSpriteDef.m_sequences.resize(1);
    testSpriteDef.m_sequences[0].m_startFrame = 0;
    testSpriteDef.m_sequences[0].m_numFrames = 22;

    testSpriteDef.m_ticksPerFrame = 33;
    testSpriteDef.m_texturePage = system.LoadTexture("sprites/superryan.png");

    m_sprite = make_unique<Sprite>(testSpriteDef);
    m_sprite->Position().x = 640;
    m_sprite->Position().y = 360;
    m_sprite->PlaySequence(0);
}
