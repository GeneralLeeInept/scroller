#pragma once

#include "Texture.h"

struct SpriteDefinition
{
	struct Frame
	{
		SDL_Rect m_dimensions;
		SDL_Point m_origin;
	};

	struct Sequence
	{
		int m_numFrames;
		int m_startFrame;
	};

	int m_numSequences;
	Sequence* m_sequences;
	TexturePtr m_texturePage;
};

class Sprite
{
public:

	Sprite(const SpriteDefinition& def);
	~Sprite();

	void PlaySequence(int sequence, bool immediate);

private:
	const SpriteDefinition& m_def;
	int m_sequence;
	int m_frame;
	Uint32 m_frameTimer;
};
