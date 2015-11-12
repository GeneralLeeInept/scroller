#ifndef _SYSTEM_H
#define _SYSTEM_H

#include <string>

using namespace std;

struct SDL_Renderer;
struct SDL_Window;

class System
{
public:
	System(const string& title);
	~System();

	string GetError() const;

private:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
};

#endif
