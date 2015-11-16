#include "stdafx.h"

#include "Texture.h"

template<>
void DestroySDLResource<SDL_Texture>(SDL_Texture* resource)
{
	SDL_DestroyTexture(resource);
}