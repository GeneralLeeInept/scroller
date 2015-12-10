#pragma once

#define FOURCC(x) ((x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3])

class FileException : public runtime_error
{
public:
	FileException(const string& path, const string& what)
		: runtime_error("FileException: '" + path + "' - " + what)
	{
	}
};

class BadDataException : public runtime_error
{
public:
	BadDataException(const string& path, const string& what)
		: runtime_error("Could not load data file '" + path + "' - " + what)
	{
	}
};
class FileHandle
{
public:
	FileHandle(const string& path, const char* mode)
	{
		m_rwops = SDL_RWFromFile(path.c_str(), mode);

		if (!m_rwops)
		{
			throw FileException(path, string("failed to open [") + mode + "]");
		}
	}

	~FileHandle()
	{
		SDL_RWclose(m_rwops);
	}

	operator SDL_RWops* ()
	{
		return m_rwops;
	}

	SDL_RWops* operator->()
	{
		return m_rwops;
	}

private:
	SDL_RWops* m_rwops = nullptr;
};
