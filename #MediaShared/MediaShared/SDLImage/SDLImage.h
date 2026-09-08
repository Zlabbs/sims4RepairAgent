#pragma once

#include <map>
#include <string>
#include <array>
#include <iostream>

#include "SDL.h"
#include "SDL_image.h"

namespace mediaShared
{
	namespace media
	{
		class SDLImage
		{
		private:
			static std::string m_s_missingTexturePath;
			static std::map<std::string, SDL_Texture*> m_s_textureCache;

			SDL_Texture* m_myTexture = nullptr;
			bool m_isTextureUnique = false;

			SDL_Rect m_transformPosAndSize  = SDL_Rect();
			SDL_RendererFlip m_transformFlipState = SDL_RendererFlip::SDL_FLIP_NONE;
			float m_transformRotation = 0;

			static SDL_Texture* CreateTexture(SDL_Renderer* renderer, std::string texturePath);
			static SDL_Texture* CreateTexturePack(SDL_Renderer* renderer, char* packItemData, unsigned int size);

			SDL_Texture* GetTextureForRendering(SDL_Renderer* renderer, SDL_Texture* orignal);

		public:
			SDLImage();
			~SDLImage();
			SDLImage& operator=(const SDLImage& t) = delete;

			static void FreeTextureCache();

			static void SetMissingTexture(SDL_Renderer* renderer, std::string texturePath);
			static void SetMissingTexturePack(SDL_Renderer* renderer, std::string texturePath, char* packItemData, unsigned int size);

			void SetPos(std::array<int, 2> pos);
			void SetSize(std::array<int, 2> size);
			void SetRotation(float rotation);
			void SetFlip(SDL_RendererFlip flip);

			std::array<int, 2> GetPos();
			std::array<int, 2> GetSize();
			float GetRotation();
			SDL_RendererFlip GetFlip();

			std::array<int, 2> GetTextureOrignalSize();

			void RenderImageWithBlendAndColorModifcation(SDL_Renderer* renderer, SDL_BlendMode mode);
			void RenderImageWithColorModifcation(SDL_Renderer* renderer);
			void RenderImageWithBlendModifcation(SDL_Renderer* renderer, SDL_BlendMode mode);
			void RenderImage(SDL_Renderer* renderer);

			void FreeTexture();
			void SetTexture(SDL_Renderer* renderer, std::string texturePath);
			void SetTexturePack(SDL_Renderer* renderer, std::string texturePath, char* packItemData, unsigned int size);

			void FreeTextureUnique();
			void SetTextureUnique(SDL_Renderer* renderer, std::string texturePath);
			void SetTexturePackUnique(SDL_Renderer* renderer, std::string texturePath, char* packItemData, unsigned int size);

			bool IsTextureValid();
			bool IsTextureUnique();

		};
	}
}

