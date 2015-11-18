#pragma once

template<typename T>
void DestroySDLResource(T* resource);

template<typename T>
class SDLResourcePtr : public shared_ptr < T >
{
public:
	SDLResourcePtr(T* resource)
	{
		reset(resource, DestroySDLResource<T>);
	}

	SDLResourcePtr() : SDLResourcePtr<T>(nullptr)
	{

	}

	operator T*() { return get(); }
};