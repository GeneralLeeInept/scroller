#pragma once

#include "Texture.h"

class System
{
public:
	System(const string& title);
	~System();

	SDL_Renderer* GetRenderer() const;

	TexturePtr LoadTexture(const string& path) const;
	vector<string> GetFilesInFolder(const string& path) const;

	string GetError() const;
	
private:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
};
