#include "SDLProgramClass.h"

void(*mediaShared::program::SDLProgramClass::m_removeEvent)() = nullptr;
bool mediaShared::program::SDLProgramClass::m_programIsValid = false;

std::array<int, 2> mediaShared::program::SDLProgramClass::m_windowCurrentSize = std::array<int, 2>{ 200, 100 };
std::array<int, 2> mediaShared::program::SDLProgramClass::m_windowCurrentPos = std::array<int, 2>{ 0, 0 };

SDL_Window* mediaShared::program::SDLProgramClass::m_windowObject = nullptr;
SDL_Renderer* mediaShared::program::SDLProgramClass::m_rendererObject = nullptr;

bool mediaShared::program::SDLProgramClass::SetUpEverything()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) //everything startup error check
	{
		std::cout << "CRITCAL ERROR: SDLPC_SUE_E<0: SDL_INIT_EVERYTHING failed applcation can not start" << std::endl;
		return false; //error has occoured //abort exacution
	}

	std::cout << "sucessfully exacuted: SDLPC_SUE" << std::endl;

	return true; //true means it worked and we can continue oparation
}

bool mediaShared::program::SDLProgramClass::SetUpImages()
{
	if (!IMG_Init(IMG_INIT_PNG) && IMG_INIT_PNG) //image start up error check
	{
		std::cout << "ERROR: SDLPC_SUI_!I&I: IMG_INIT_PNG failed textures are non oparational" << std::endl;
		return false;
	}

	std::cout << "sucessfully exacuted: SDLPC_SUI" << std::endl;

	return true;
}

bool mediaShared::program::SDLProgramClass::SetUpText()
{
	if (TTF_Init() < 0) //text start up error check
	{
		std::cout << "ERROR: SDLPC_SUT_T<0: TTF_Init failed text can not be rendered" << std::endl;
		return false;
	}

	std::cout << "sucessfully exacuted: SDLPC_SUT" << std::endl;

	return true;
}

bool mediaShared::program::SDLProgramClass::SetUpWindow(std::string title, unsigned int windowW, unsigned int windowH)
{
	//SDL_SetHint("SDL_BORDERLESS_WINDOWED_STYLE", "1"); //window settings
	//SDL_SetHint("SDL_BORDERLESS_RESIZABLE_STYLE", "1");

	m_windowCurrentSize[0] = windowW;
	m_windowCurrentSize[1] = windowH;

	m_windowObject = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowCurrentSize[0], m_windowCurrentSize[1], SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE); //SDL_WINDOW_FULLSCREEN

	if (m_windowObject == nullptr) //we have no window how odd
	{
		std::cout << "ERROR: SDLPC_SUW_WO==N: failed to start window" << std::endl;
		return false; //error has occoured
	}

	try
	{
		SDL_GetWindowPosition(m_windowObject, &m_windowCurrentPos[0], &m_windowCurrentPos[1]); //get win pos
	}
	catch (...)
	{
		//failed to get win pos but it aint the end of the world
	}

	std::cout << "sucessfully exacuted: SDLPC_SUW" << std::endl;

	return true;
}

bool mediaShared::program::SDLProgramClass::SetUpRenderer()
{
	m_rendererObject = SDL_CreateRenderer(m_windowObject, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(m_rendererObject, SDL_BLENDMODE_BLEND);

	if (m_rendererObject == nullptr) //we have no renderer how oddddd
	{
		std::cout << "ERROR: SDLPC_SUR_RO==N: failed to start renderer" << std::endl;
		return false; //error has occoured
	}

	std::cout << "sucessfully exacuted: SDLPC_SUR" << std::endl;

	return true;
}

void mediaShared::program::SDLProgramClass::OnRemove()
{
	if (m_removeEvent != nullptr) { m_removeEvent(); }
	RemoveImageFontAndMixerStorage();
	RemoveWindowAndRenderer();

	std::cout << "deleting: program bye bye" << std::endl;
}

void mediaShared::program::SDLProgramClass::RemoveImageFontAndMixerStorage()
{
}


void mediaShared::program::SDLProgramClass::RemoveWindowAndRenderer()
{
	std::cout << "deleting: window" << std::endl;
	if (m_windowObject != nullptr)
	{
		SDL_DestroyWindow(m_windowObject); //deleteing
		m_windowObject = nullptr;
	}

	std::cout << "deleting: render" << std::endl;
	if (m_rendererObject != nullptr)
	{
		SDL_DestroyRenderer(m_rendererObject); //deleteing
		m_rendererObject = nullptr;
	}
}

void mediaShared::program::SDLProgramClass::SetPreRemoveEvent(void(*func)())
{
	m_removeEvent = func;
}

bool mediaShared::program::SDLProgramClass::Start(std::string title, unsigned int windowW, unsigned int windowH)
{
	if (SetUpEverything() == false) { return false; };
	if (SetUpImages() == false) { return false; };
	if (SetUpText() == false) { return false; };

	if (SetUpWindow(title, windowW, windowH) == false) { return false; };
	if (SetUpRenderer() == false) { return false; };

	m_programIsValid = true; //the program is now valid

	std::cout << "sucessfully exacuted: SDLPC_S: program class startup procedure complete, the program is now valid" << std::endl;

	return true;
}

bool mediaShared::program::SDLProgramClass::Remove()
{
	m_programIsValid = false; //the program is no longer valid

	OnRemove();

	return true;
}

void mediaShared::program::SDLProgramClass::Update(SDL_Event& event)
{
	if (m_programIsValid == false) { return; }

	//events program needs to handle here
	switch (event.type)
	{
	case SDL_QUIT:
		std::cout << std::endl << "SDLPC_U: A request to exit the applcation was made: exacuteing request" << std::endl << std::endl;
		Remove(); //in quit we just have to remove the program
		return; //exits faster

	case SDL_WINDOWEVENT:
		if (event.window.event == SDL_WINDOWEVENT_RESIZED && SDL_GetWindowFromID(event.window.windowID) == m_windowObject)
		{
			SDL_Surface* WindowSuface = SDL_GetWindowSurface(m_windowObject); //all this attemps to pervent rendering issues if draging border
			uint32_t black = SDL_MapRGBA(WindowSuface->format, 0, 0, 0, 255);
			SDL_FillRect(WindowSuface, NULL, black);  //clear the screen
			SDL_UpdateWindowSurface(m_windowObject);

			SDL_GetWindowSizeInPixels(m_windowObject, &m_windowCurrentSize[0], &m_windowCurrentSize[1]); //stores our new size		}

			SDL_GetWindowPosition(m_windowObject, &m_windowCurrentPos[0], &m_windowCurrentPos[1]); //stores our new pos

			break;
		}
	}
}

bool mediaShared::program::SDLProgramClass::IsValid()
{
	return m_programIsValid;
}
