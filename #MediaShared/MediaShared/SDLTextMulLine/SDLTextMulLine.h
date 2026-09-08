#pragma once

#include <vector>

#include "../SDLText/SDLText.h"

namespace mediaShared
{
	namespace media
	{
		class SDLTextMulLine
		{
		private:

			std::string m_font = "";
			std::string m_text = "";
			int m_fontSize = 0;

			SDLText_VerticalLocation m_transformInnerVerticalLocationOfText = SDLText_VerticalLocation::VUpper;
			SDLText_HorizontalLocation m_transformInnerHorizontalLocationOfText = SDLText_HorizontalLocation::HLeft;
			std::array<int, 2> m_pos = std::array<int, 2>();
			int m_width = 256;
			int m_lineSpacing = 2;

			std::vector<std::string> m_newLineChars = std::vector<std::string>{"\n","\r","\\n","\\r"};
			std::vector<SDLText> m_allTextElements = std::vector<SDLText>();

			void CalulateMulLineText(SDL_Renderer* renderer, std::string text, std::vector<std::string> newLineChars, std::string font, int fontSize, std::array<int, 2> pos, int width, int spacing, SDLText_VerticalLocation vpos, SDLText_HorizontalLocation hpos);

		public:

			void SetNewLineSeparators(SDL_Renderer* renderer, std::vector<std::string> newLines);
			void SetVerticalAlignment(SDL_Renderer* renderer, SDLText_VerticalLocation location);
			void SetHorizontalAlignment(SDL_Renderer* renderer, SDLText_HorizontalLocation location);
			void SetPos(SDL_Renderer* renderer, std::array<int, 2> pos);
			void SetWidth(SDL_Renderer* renderer, int width);
			void SetLineSpacing(SDL_Renderer* renderer, int space);

			std::vector<std::string> GetNewLineSeparators();
			SDLText_VerticalLocation GetVerticalAlignment();
			SDLText_HorizontalLocation GetHorizontalAlignment();
			std::array<int, 2> GetPos();
			int GetWidth();
			int SetLineSpacing();


			void SetFontPack(std::string pathToFontFile, char* packItemData, unsigned int packItemSize, unsigned int size);
			void SetFont(std::string pathToFontFile, unsigned int size);
			void SetText(SDL_Renderer* renderer, std::string text);

			void RenderText(SDL_Renderer* renderer);
		};

	}
}
