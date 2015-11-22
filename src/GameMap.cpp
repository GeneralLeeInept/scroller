#include "stdafx.h"

#include "GameMap.h"

/*

Structure on disk

+---------+-------------------------------+
| 4 bytes | 'G','M','A','P'               |
+---------+-------------------------------+
| 4 bytes | Map width (in tiles)          |
+---------+-------------------------------+
| 4 bytes | Map height (in tiles)         |
+---------+-------------------------------+
*/
void GameMap::load(const string& path)
{
	SDL_RWops* file = SDL_RWFromFile(path.c_str(), "rb");

	if (!file)
	{
		throw exception(SDL_GetError());
	}

//	SDL_ReadLE32()
}