#pragma once

#include <array>
#include <string>
#include <iostream>

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"

namespace mediaShared
{
	namespace program
	{
		class SDLProgramClass
		{
		private:
			static void(*m_removeEvent)();
			static bool m_programIsValid;

			static bool SetUpEverything();
			static bool SetUpImages();
			static bool SetUpText();
			static bool SetUpWindow(std::string title, unsigned int windowW, unsigned int windowH);
			static bool SetUpRenderer();

			static void OnRemove();
			static void RemoveImageFontAndMixerStorage();
			static void RemoveWindowAndRenderer();

		public:
			static void SetPreRemoveEvent(void(*func)());

			static std::array<int, 2> m_windowCurrentSize;
			static std::array<int, 2> m_windowCurrentPos;

			static SDL_Window* m_windowObject;
			static SDL_Renderer* m_rendererObject;

			static bool Start(std::string title, unsigned int windowW, unsigned int windowH);
			static bool Remove();
			static void Update(SDL_Event& event);

			static bool IsValid(); //is our instance of the program valid (will be invalid if startup failed or removal is requested)
		};
	}
}

