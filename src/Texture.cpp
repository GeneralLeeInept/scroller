#include "stdafx.h"

#include "Texture.h"

template <>
void DestroySDLResource<SDL_Texture>(SDL_Texture* resource)
{
    SDL_DestroyTexture(resource);
}

TexturePtr::TexturePtr(SDL_Texture* texture)
    : TexturePtr(texture, "")
{
}

TexturePtr::TexturePtr(SDL_Texture* texture, const string& path)
    : SdlResourcePtr<SDL_Texture>(texture)
    , m_path(path)
{
}

const string& TexturePtr::GetPath() const
{
    return m_path;
}
