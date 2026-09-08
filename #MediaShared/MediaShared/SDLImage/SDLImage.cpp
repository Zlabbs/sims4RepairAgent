#include "SDLImage.h"

std::string mediaShared::media::SDLImage::m_s_missingTexturePath = "";
std::map<std::string, SDL_Texture*> mediaShared::media::SDLImage::m_s_textureCache = std::map<std::string, SDL_Texture*>();

SDL_Texture* mediaShared::media::SDLImage::CreateTexture(SDL_Renderer* renderer, std::string texturePath)
{
	SDL_Surface* pSurface = IMG_Load(texturePath.c_str());
	if (pSurface == nullptr)
	{
		std::cout << "SDLI_CT_PS==N: FAILED TO ALLOCATE SURFACE DATA: ERROR DETAILS:" << SDL_GetError() << std::endl;
		return nullptr;
	}

	SDL_Texture* textureObject = SDL_CreateTextureFromSurface(renderer, pSurface);
	if (textureObject == nullptr)
	{
		std::cout << "SDLI_CT_TO==N: FAILED TO ALLOCATE TEXTURE DATA: ERROR DETAILS:" << SDL_GetError() << std::endl;
	}

	SDL_FreeSurface(pSurface);
	return textureObject;
}

SDL_Texture* mediaShared::media::SDLImage::CreateTexturePack(SDL_Renderer* renderer, char* packItemData, unsigned int size)
{
	SDL_RWops* wopStream = SDL_RWFromMem(packItemData, size);
	if (wopStream == nullptr)
	{
		std::cout << "SDLI_CTP_WS==N: FAILED TO ALLOCATE STREAM DATA: ERROR DETAILS:" << SDL_GetError() << std::endl;
		return nullptr;
	}

	SDL_Surface* pSurface = IMG_Load_RW(wopStream, 0);
	if (pSurface == nullptr)
	{
		std::cout << "SDLI_CTP_PS==N: FAILED TO ALLOCATE SURFACE DATA: ERROR DETAILS:" << SDL_GetError() << std::endl;
		return nullptr;
	}

	SDL_Texture* textureObject = SDL_CreateTextureFromSurface(renderer, pSurface);
	if (textureObject == nullptr)
	{
		std::cout << "SDLI_CTP_TO==N: FAILED TO ALLOCATE IMAGE DATA: ERROR DETAILS:" << SDL_GetError() << std::endl;
	}

	SDL_RWclose(wopStream);
	SDL_FreeSurface(pSurface);
	return textureObject;
}


SDL_Texture* mediaShared::media::SDLImage::GetTextureForRendering(SDL_Renderer* renderer, SDL_Texture* orignal)
{
	if (orignal != nullptr)
	{
		return orignal;
	}
	else
	{
		if (m_s_textureCache.count(m_s_missingTexturePath + reinterpret_cast<char*>(renderer)) > 0)
		{
			return m_s_textureCache[m_s_missingTexturePath + reinterpret_cast<char*>(renderer)];
		}

		return nullptr;
	}
}


mediaShared::media::SDLImage::SDLImage()
{

}

mediaShared::media::SDLImage::~SDLImage()
{
	if (m_isTextureUnique == true) { FreeTextureUnique(); }
}


void mediaShared::media::SDLImage::FreeTextureCache()
{
	for (std::map<std::string, SDL_Texture*>::iterator i = m_s_textureCache.begin(); i != m_s_textureCache.end(); ++i) //finds all items in map
	{
		SDL_DestroyTexture(i->second);
	}

	m_s_textureCache.clear();
	m_s_missingTexturePath = "";
}


void mediaShared::media::SDLImage::SetMissingTexture(SDL_Renderer* renderer, std::string texturePath)
{
	SDL_Texture* texture = CreateTexture(renderer, texturePath);

	if (m_s_textureCache.count(texturePath + reinterpret_cast<char*>(renderer)) == 0)
	{
		m_s_textureCache[texturePath + reinterpret_cast<char*>(renderer)] = texture;
	}
	else
	{
		SDL_DestroyTexture(m_s_textureCache[texturePath + reinterpret_cast<char*>(renderer)]);
		m_s_textureCache[texturePath + reinterpret_cast<char*>(renderer)] = texture;
	}

	m_s_missingTexturePath = texturePath;
}

void mediaShared::media::SDLImage::SetMissingTexturePack(SDL_Renderer* renderer, std::string texturePath, char* packItemData, unsigned int size)
{
	SDL_Texture* texture = CreateTexturePack(renderer, packItemData, size);
	if (m_s_textureCache.count(texturePath + reinterpret_cast<char*>(renderer)) == 0)
	{
		m_s_textureCache[texturePath + reinterpret_cast<char*>(renderer)] = texture;
	}
	else
	{
		SDL_DestroyTexture(m_s_textureCache[texturePath + reinterpret_cast<char*>(renderer)]);
		m_s_textureCache[texturePath + reinterpret_cast<char*>(renderer)] = texture;
	}

	m_s_missingTexturePath = texturePath;
}


void mediaShared::media::SDLImage::SetPos(std::array<int, 2> pos)
{
	m_transformPosAndSize.x = pos[0];
	m_transformPosAndSize.y = pos[1];
}

void mediaShared::media::SDLImage::SetSize(std::array<int, 2> size)
{
	m_transformPosAndSize.w = size[0];
	m_transformPosAndSize.h = size[1];
}

void mediaShared::media::SDLImage::SetRotation(float rotation)
{
	m_transformRotation = rotation;
}

void mediaShared::media::SDLImage::SetFlip(SDL_RendererFlip flip)
{
	m_transformFlipState = flip;
}

std::array<int, 2> mediaShared::media::SDLImage::GetPos()
{
	return std::array<int, 2>{m_transformPosAndSize.x, m_transformPosAndSize.y};
}

std::array<int, 2> mediaShared::media::SDLImage::GetSize()
{
	return std::array<int, 2>{m_transformPosAndSize.w, m_transformPosAndSize.h};
}

float mediaShared::media::SDLImage::GetRotation()
{
	return m_transformRotation;
}

SDL_RendererFlip mediaShared::media::SDLImage::GetFlip()
{
	return m_transformFlipState;
}

std::array<int, 2> mediaShared::media::SDLImage::GetTextureOrignalSize()
{
	std::array<int, 2> array = std::array<int, 2>();
	if (m_myTexture == nullptr) { return array; }

	SDL_QueryTexture(m_myTexture, NULL, NULL, &array[0], &array[1]);
	return array;
}


void mediaShared::media::SDLImage::RenderImageWithBlendAndColorModifcation(SDL_Renderer* renderer, SDL_BlendMode mode)
{
	Uint8 r, g, b, a;
	SDL_Texture* texture = GetTextureForRendering(renderer, m_myTexture);
	if (texture == nullptr) { return; }

	if (SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a) >= 0)
	{
		SDL_SetTextureAlphaMod(texture, a);
		SDL_SetTextureColorMod(texture, r, g, a);
	}

	SDL_SetTextureBlendMode(texture, mode);
	SDL_RenderCopyEx(renderer, texture, NULL, &m_transformPosAndSize, m_transformRotation, NULL, m_transformFlipState);
	SDL_SetTextureBlendMode(texture, SDL_BlendMode::SDL_BLENDMODE_BLEND);

	SDL_SetTextureAlphaMod(texture, 255);
	SDL_SetTextureColorMod(texture, 255, 255, 255);
}

void mediaShared::media::SDLImage::RenderImageWithColorModifcation(SDL_Renderer* renderer)
{
	Uint8 r, g, b, a;
	SDL_Texture* texture = GetTextureForRendering(renderer, m_myTexture);
	if (texture == nullptr) { return; }

	if (SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a) >= 0)
	{
		SDL_SetTextureAlphaMod(texture, a);
		SDL_SetTextureColorMod(texture, r, g, a);
	}

	SDL_RenderCopyEx(renderer, texture, NULL, &m_transformPosAndSize, m_transformRotation, NULL, m_transformFlipState);

	SDL_SetTextureAlphaMod(texture, 255);
	SDL_SetTextureColorMod(texture, 255, 255, 255);
}

void mediaShared::media::SDLImage::RenderImageWithBlendModifcation(SDL_Renderer* renderer, SDL_BlendMode mode)
{
	SDL_Texture* texture = GetTextureForRendering(renderer, m_myTexture);
	if (texture == nullptr) { return; }

	SDL_SetTextureBlendMode(texture, mode);
	SDL_RenderCopyEx(renderer, texture, NULL, &m_transformPosAndSize, m_transformRotation, NULL, m_transformFlipState);
	SDL_SetTextureBlendMode(texture, SDL_BlendMode::SDL_BLENDMODE_BLEND);

}

void mediaShared::media::SDLImage::RenderImage(SDL_Renderer* renderer)
{
	SDL_Texture* texture = m_myTexture;

	if (texture == nullptr)
	{
		if (m_s_textureCache.count(m_s_missingTexturePath + reinterpret_cast<char*>(renderer)) > 0)
		{
			texture = m_s_textureCache[m_s_missingTexturePath + reinterpret_cast<char*>(renderer)];
		}
		else
		{
			return;
		}
	}

	SDL_RenderCopyEx(renderer, texture, NULL, &m_transformPosAndSize , m_transformRotation, NULL, m_transformFlipState);
}


void mediaShared::media::SDLImage::FreeTexture()
{
	if (m_isTextureUnique == false)
	{
		m_myTexture = nullptr; //the texture dosent actually get freed as to do that you must call FreeTextureCache but this frees every non unique texture
	}
	else
	{
		std::cout << "SDLI_FT_ITU==T: UNABLE TO FREE TEXTURE AS IT IS UNIQUE" << std::endl;
	}
}

void mediaShared::media::SDLImage::SetTexture(SDL_Renderer* renderer, std::string texturePath)
{
	if (m_s_textureCache.count(texturePath + reinterpret_cast<char*>(renderer)) != 0)
	{
		m_myTexture = m_s_textureCache[texturePath + reinterpret_cast<char*>(renderer)];
		m_isTextureUnique = false;
	}
	else
	{
		SDL_Texture* texture = CreateTexture(renderer, texturePath);
		if (texture == nullptr) { return; }

		m_s_textureCache[texturePath + reinterpret_cast<char*>(renderer)] = texture;
		m_myTexture = texture;
		m_isTextureUnique = false;
	}
}

void mediaShared::media::SDLImage::SetTexturePack(SDL_Renderer* renderer, std::string texturePath, char* packItemData, unsigned int size)
{
	if (m_s_textureCache.count(texturePath + reinterpret_cast<char*>(renderer)) != 0)
	{
		m_myTexture = m_s_textureCache[texturePath + reinterpret_cast<char*>(renderer)];
		m_isTextureUnique = false;
	}
	else
	{
		SDL_Texture* texture = CreateTexturePack(renderer, packItemData, size);
		if (texture == nullptr) { return; }

		m_s_textureCache[texturePath + reinterpret_cast<char*>(renderer)] = texture;
		m_myTexture = texture;
		m_isTextureUnique = false;
	}
}


void mediaShared::media::SDLImage::FreeTextureUnique()
{
	if (m_isTextureUnique == true)
	{
		SDL_DestroyTexture(m_myTexture);
		m_myTexture = nullptr;
	}
	else
	{
		std::cout << "SDLI_FTU_ITU==F: UNABLE TO FREE TEXTURE AS IT IS NOT UNIQUE" << std::endl;
	}
}

void mediaShared::media::SDLImage::SetTextureUnique(SDL_Renderer* renderer, std::string texturePath)
{
	SDL_Texture* texture = CreateTexture(renderer, texturePath);
	if (texture == nullptr) { return; }

	m_myTexture = texture;
	m_isTextureUnique = true;
}

void mediaShared::media::SDLImage::SetTexturePackUnique(SDL_Renderer* renderer, std::string texturePath, char* packItemData, unsigned int size)
{
	SDL_Texture* texture = CreateTexturePack(renderer, packItemData, size);
	if (texture == nullptr) { return; }

	m_myTexture = texture;
	m_isTextureUnique = true;
}


bool mediaShared::media::SDLImage::IsTextureValid()
{
	return (m_myTexture != nullptr);
}

bool mediaShared::media::SDLImage::IsTextureUnique()
{
	return m_isTextureUnique;
}

