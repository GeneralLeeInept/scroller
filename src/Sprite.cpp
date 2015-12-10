#include "stdafx.h"

#include "FileHandle.h"
#include "Sprite.h"
#include "System.h"

#define SPRITEMAGIC FOURCC("SPRT")

struct SpriteHeader
{
	Uint32 m_magic;	// 'S','P','R','T'
	Uint16 m_numFrames;
	Uint16 m_numSequences;
	Uint16 m_ticksPerFrame;
	Uint16 m_texturePathLength;
};

void SpriteDefinition::Load(const string& path, const System& system)
{
	FileHandle file(path, "rb");

	SpriteHeader sh;

	if (SDL_RWread(file, &sh, sizeof(SpriteHeader), 1) != 1)
	{
		throw BadDataException(path, "failed to read sprite header");
	}

	if (sh.m_magic != SPRITEMAGIC)
	{
		throw BadDataException(path, "invalid sprite file");
	}

	m_frames.resize(sh.m_numFrames);

	if (SDL_RWread(file, &m_frames[0], sizeof(Frame), sh.m_numFrames) != sh.m_numFrames)
	{
		throw BadDataException(path, "failed to read sprite frames");
	}

	m_sequences.resize(sh.m_numSequences);

	if (SDL_RWread(file, &m_sequences[0], sizeof(Sequence), sh.m_numSequences) != sh.m_numSequences)
	{
		throw BadDataException(path, "failed to read sprite sequences");
	}

	m_ticksPerFrame = sh.m_ticksPerFrame;

	string texturePath;
	texturePath.reserve(sh.m_texturePathLength);
	if (SDL_RWread(file, &texturePath[0], 1, sh.m_texturePathLength) != sh.m_texturePathLength)
	{
		throw BadDataException(path, "failed to read sprite texture path");
	}

	m_texturePage = system.LoadTexture(texturePath);
}

void SpriteDefinition::Save(const string& path)
{
	FileHandle file(path, "wb");

	SpriteHeader sh;

	sh.m_magic = SPRITEMAGIC;
	sh.m_numFrames = m_frames.size();
	sh.m_numSequences = m_sequences.size();
	sh.m_ticksPerFrame = m_ticksPerFrame;
	sh.m_texturePathLength = m_texturePage.GetPath().length();

	if (SDL_RWwrite(file, &sh, sizeof(SpriteHeader), 1) != 1)
	{
		throw FileException(path, "failed to write sprite header");
	}

	if (SDL_RWwrite(file, &m_frames[0], sizeof(Frame), sh.m_numFrames) != sh.m_numFrames)
	{
		throw FileException(path, "failed to write sprite frames");
	}

	if (SDL_RWwrite(file, &m_sequences[0], sizeof(Sequence), sh.m_numSequences) != sh.m_numSequences)
	{
		throw FileException(path, "failed to write sprite sequences");
	}

	if (SDL_RWwrite(file, &m_texturePage.GetPath()[0], 1, sh.m_texturePathLength) != sh.m_texturePathLength)
	{
		throw FileException(path, "failed to write sprite texture path");
	}
}

Sprite::Sprite(const SpriteDefinition& def)
	: m_def(def)
{
}


SDL_Point& Sprite::Position()
{
	return m_position;
}

void Sprite::SetFlip(bool flip)
{
	m_flipped = flip;
}

void Sprite::PlaySequence(int sequence)
{
	m_sequence = sequence;
	m_frame = 0;
	m_frameTimer = 0;
}

void Sprite::Update(Uint32 delta)
{
	if (m_sequence >= 0)
	{
		m_frameTimer += delta;

		while (m_frameTimer >= m_def.m_ticksPerFrame)
		{
			m_frameTimer -= m_def.m_ticksPerFrame;
			++m_frame;
		}

		while (m_frame >= m_def.m_sequences[m_sequence].m_numFrames)
		{
			m_frame -= m_def.m_sequences[m_sequence].m_numFrames;
		}
	}
}

void Sprite::Render(SDL_Renderer* renderer)
{
	if (m_sequence >= 0)
	{
		SDL_Rect destRect;
		int frameIndex = m_def.m_sequences[m_sequence].m_startFrame + m_frame;
		const SpriteDefinition::Frame& frame = m_def.m_frames.at(frameIndex);

		destRect.x = m_position.x - frame.m_origin.x;
		destRect.y = m_position.y - frame.m_origin.y;
		destRect.w = frame.m_dimensions.w;
		destRect.h = frame.m_dimensions.h;
		SDL_RenderCopyEx(renderer, m_def.m_texturePage, &frame.m_dimensions, &destRect, 0.0, nullptr, m_flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}
}
