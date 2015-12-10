#pragma once

template<typename T>
void DestroySDLResource(T* resource);

template<typename T>
class SdlResourcePtr : public shared_ptr < T >
{
public:
	SdlResourcePtr(T* resource)
	{
		reset(resource, DestroySDLResource<T>);
	}

	SdlResourcePtr() : SdlResourcePtr<T>(nullptr)
	{

	}

	operator T* ()
	{
		return get();
	}

	operator T*() const
	{
		return const_cast<T*>(get());
	}
};