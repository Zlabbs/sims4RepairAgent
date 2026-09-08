#pragma once

#include <map>
#include <string>
#include <array>
#include <iostream>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

namespace mediaShared
{
	namespace media
	{
		enum SDLText_HorizontalLocation
		{
			HLeft,
			HCenter,
			HRight,
			HScrollLeft,
			HScrollRight,
		};

		enum SDLText_VerticalLocation
		{
			VUpper,
			VCenter,
			VLower,
			VScrollUpper,
			VScrollLower,
		};

		class SDLText
		{
		private:
			static std::map<std::string, TTF_Font*> m_s_fontCache;
			static std::map<std::string, SDL_Texture*> m_s_textureBuffer;

			TTF_Font* m_fontAsset = nullptr;
			SDL_Texture* m_textureOfText = nullptr;
			bool m_isFontUnique = false;


			SDL_Rect m_transformOuterPosAndSize = SDL_Rect(); //the area the text can be

			SDLText_HorizontalLocation m_transformInnerHorizontalLocationOfText = SDLText_HorizontalLocation::HLeft;
			SDLText_VerticalLocation m_transformInnerVerticalLocationOfText = SDLText_VerticalLocation::VUpper;
			SDL_Rect m_transformInnerPosAndSize = SDL_Rect(); //where the text acually is
			SDL_RendererFlip m_transformInnerFlipState = SDL_RendererFlip::SDL_FLIP_NONE;
			std::array<int, 2> m_innerOuterMargin = { 0, 0 };
			float m_transformInnerRotation = 0;

			void RecalulateAlignment();

		public:
			SDLText();
			~SDLText();
			SDLText& operator=(const SDLText& t) = delete;

			static void FreeFontCache();
			static void FreeTextureBuffer();
			void FreeFontUnique();
			void FreeTextTexture();
			void FreeFont();

			void SetFontPack(std::string pathToFontFile, char* packItemData, unsigned int packItemSize, unsigned int size, bool isUnique = false);
			void SetFont(std::string pathToFontFile, unsigned int size, bool isUnique = false);
			void SetText(SDL_Renderer* renderer, std::string text);

			std::array<int, 2> GetSizeOfIfTextWas(std::string text);

			void RenderText(SDL_Renderer* renderer);

			int GetTextWidth();
			int GetTextHeight();

			void SetPos(std::array<int, 2> pos);
			void SetSize(std::array<int, 2> size);
			void SetFlip(SDL_RendererFlip flip);
			void SetRotation(float rotation);
			void SetHorizontalAlignment(SDLText_HorizontalLocation location); //imagin text aligment on windows
			void SetVerticalAlignment(SDLText_VerticalLocation location);
			void SetMargins(std::array<int, 2> margins);

			std::array<int, 2> GetPos();
			std::array<int, 2> GetSize();
			SDL_RendererFlip GetFlip();
			float GetRotation();
			SDLText_HorizontalLocation GetHorizontalAlignment();
			SDLText_VerticalLocation GetVerticalAlignment();
			std::array<int, 2> GetMargins();

			TTF_Font* GetUnderlyingFontAsset();

			bool IsAssetsValid();
			bool IsFontUnique();
		};
	}
}

