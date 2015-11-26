#pragma once

#include "SdlResourcePtr.h"

class TexturePtr : public SdlResourcePtr < SDL_Texture >
{
public:
	TexturePtr() = default;
	TexturePtr(SDL_Texture* texture);
	TexturePtr(SDL_Texture* texture, const string& path);

	const string& GetPath() const;

private:
	string m_path;
};
