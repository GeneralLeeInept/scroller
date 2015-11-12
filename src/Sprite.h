#ifndef _SPRITE_H
#define _SPRITE_H

#include <SDL_rect.h>

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
		Frame* m_frames;
	};

	int m_numSequences;
	Sequence* m_sequences;
};

class Sprite
{
public:

	Sprite(SpriteDefinition& def);
	~Sprite();

	void PlaySequence(int sequence, bool immediate);

private:
	SpriteDefinition& m_def;
	int m_sequence;
	int m_frame;
	float m_frameTimer;
};

#endif