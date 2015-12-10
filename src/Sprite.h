#pragma once

#include "Texture.h"

class System;

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

	vector<Frame> m_frames;
	vector<Sequence> m_sequences;
	TexturePtr m_texturePage;
	Uint16 m_ticksPerFrame;

	void Load(const string& path, const System& system);
	void Save(const string& path);
};

class Sprite
{
public:
	Sprite(const SpriteDefinition& def);

	SDL_Point& Position();
	void SetFlip(bool flip);

	void PlaySequence(int sequence);
	void Update(Uint32 delta);
	void Render(SDL_Renderer* renderer);

private:
	const SpriteDefinition& m_def;
	int m_sequence = -1;
	int m_frame = 0;
	Uint32 m_frameTimer = 0;
	SDL_Point m_position;
	bool m_flipped = false;
};
