#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "System.h"

vector<string> System::GetFilesInFolder(const string& path) const
{
	vector<string> names;
	string searchPath = path + "*.*"; 
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(searchPath.c_str(), &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				names.push_back(path + fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));

		::FindClose(hFind);
	}

	return names;
}
