#include "SDLText.h"

std::map<std::string, TTF_Font*>  mediaShared::media::SDLText::m_s_fontCache = std::map<std::string, TTF_Font*>();
std::map<std::string, SDL_Texture*> mediaShared::media::SDLText::m_s_textureBuffer = std::map<std::string, SDL_Texture*>();

void mediaShared::media::SDLText::RecalulateAlignment()
{
	switch (m_transformInnerHorizontalLocationOfText)
	{
	case HLeft:
		m_transformInnerPosAndSize.x = m_transformOuterPosAndSize.x + m_innerOuterMargin[0]; //as left as possible
		break;
	case HCenter:
		m_transformInnerPosAndSize.x = (m_transformOuterPosAndSize.x + (m_transformOuterPosAndSize.w / 2)) - (m_transformInnerPosAndSize.w / 2); //in the center
		break;
	case HRight:
		m_transformInnerPosAndSize.x = (m_transformOuterPosAndSize.x + m_transformOuterPosAndSize.w) - (m_transformInnerPosAndSize.w + m_innerOuterMargin[0]); //as right as possible
		break;
	case HScrollLeft:
		m_transformInnerPosAndSize.x = m_transformOuterPosAndSize.x + m_innerOuterMargin[0]; // starts in left posistion but change to right one with exess text
		if (m_transformInnerPosAndSize.w > m_transformOuterPosAndSize.w) { m_transformInnerPosAndSize.x = (m_transformOuterPosAndSize.x + m_transformOuterPosAndSize.w) - (m_transformInnerPosAndSize.w); }
		break;
	case HScrollRight:
		m_transformInnerPosAndSize.x = (m_transformOuterPosAndSize.x + m_transformOuterPosAndSize.w) - (m_transformInnerPosAndSize.w + m_innerOuterMargin[0]); // starts in right posistion but change to left one with exess text
		if (m_transformInnerPosAndSize.w > m_transformOuterPosAndSize.w) { m_transformInnerPosAndSize.x = m_transformOuterPosAndSize.x; }
		break;

	default:
		break;
	}

	switch (m_transformInnerVerticalLocationOfText)
	{
	case VUpper:
		m_transformInnerPosAndSize.y = m_transformOuterPosAndSize.y + m_innerOuterMargin[1]; //as far up as possible
		break;
	case VCenter:
		m_transformInnerPosAndSize.y = (m_transformOuterPosAndSize.y + (m_transformOuterPosAndSize.h / 2)) - (m_transformInnerPosAndSize.h / 2); //in the center
		break;
	case VLower:
		m_transformInnerPosAndSize.y = (m_transformOuterPosAndSize.y + m_transformOuterPosAndSize.h) - (m_transformInnerPosAndSize.h + m_innerOuterMargin[1]); //as down as possible 
		break;
	case VScrollLower:
		m_transformInnerPosAndSize.y = m_transformOuterPosAndSize.y + m_innerOuterMargin[1]; //starts in upper posistion but change to lower one with exess lines
		if (m_transformInnerPosAndSize.h > m_transformOuterPosAndSize.h) { m_transformInnerPosAndSize.x = (m_transformOuterPosAndSize.y + m_transformOuterPosAndSize.h) - (m_transformInnerPosAndSize.h); }
		break;
	case VScrollUpper:
		m_transformInnerPosAndSize.x = (m_transformOuterPosAndSize.y + m_transformOuterPosAndSize.h) - (m_transformInnerPosAndSize.h + m_innerOuterMargin[1]); //starts in lower posistion but change to upper one with exess lines
		if (m_transformInnerPosAndSize.h > m_transformOuterPosAndSize.h) { m_transformInnerPosAndSize.y = m_transformOuterPosAndSize.y; }
		break;


	default:
		break;
	}
}

mediaShared::media::SDLText::SDLText()
{
}

mediaShared::media::SDLText::~SDLText()
{
	if (m_isFontUnique == true) { FreeFontUnique(); }
	FreeTextTexture();
}


void mediaShared::media::SDLText::FreeFontCache()
{
	for (std::map<std::string, TTF_Font*>::iterator i = m_s_fontCache.begin(); i != m_s_fontCache.end(); ++i) //finds all items in map
	{
		TTF_CloseFont(i->second);
	}
}

void mediaShared::media::SDLText::FreeTextureBuffer()
{
	for (std::map<std::string, SDL_Texture*>::iterator i = m_s_textureBuffer.begin(); i != m_s_textureBuffer.end(); ++i) //finds all items in map
	{
		SDL_DestroyTexture(i->second);
	}
}

void mediaShared::media::SDLText::FreeFontUnique()
{
	if (m_isFontUnique == true)
	{
		TTF_CloseFont(m_fontAsset);
		m_fontAsset = nullptr;
	}
	else
	{
		std::cout << "SDLF_FFU_IFU==F: UNABLE TO FREE FONT AS IT IS NOT UNIQUE" << std::endl;
	}
}

void mediaShared::media::SDLText::FreeTextTexture()
{
	SDL_DestroyTexture(m_textureOfText);
	m_textureOfText = nullptr;
}

void mediaShared::media::SDLText::FreeFont()
{
	if (m_isFontUnique == false)
	{
		m_fontAsset = nullptr; //the font dosent actually get freed as to do that you must call FreeTextureCache but this frees every non unique font
	}
	else
	{
		std::cout << "SDLT_FF_IFU==T: UNABLE TO FREE FONT AS IT IS UNIQUE" << std::endl;
	}
}

void mediaShared::media::SDLText::SetFontPack(std::string pathToFontFile, char* packItemData, unsigned int packItemSize, unsigned int size, bool isUnique)
{
	SDL_RWops* wopStream = SDL_RWFromMem(packItemData, packItemSize);
	if (wopStream == nullptr)
	{
		std::cout << "SDLT_SFP_WS==N: FAILED TO ALLOCATE STREAM DATA: ERROR DETAILS:" << SDL_GetError() << std::endl;
		return;
	}


	if (isUnique == false)
	{
		if (m_s_fontCache.count(pathToFontFile + std::to_string(size)) != 0)
		{
			m_isFontUnique = false;
			m_fontAsset = m_s_fontCache[pathToFontFile + std::to_string(size)];
		}
		else
		{
			int freeStreamWhen = 1;
			m_fontAsset = TTF_OpenFontRW(wopStream, freeStreamWhen, size);
			m_isFontUnique = false;

			if (m_fontAsset != nullptr)//to avoid caching invalid data
			{
				m_s_fontCache[pathToFontFile + std::to_string(size)] = m_fontAsset;
			}
			else
			{
				std::cout << "SDLT_SFP_FA==N: FAILED TO OPEN NON UNIQUE FONT: ERROR DETAILS:" << SDL_GetError() << std::endl;
			}
		}
	}
	else
	{
		int freeStreamWhen = 1;
		m_fontAsset = TTF_OpenFontRW(wopStream, freeStreamWhen, size);
		m_isFontUnique = true;

		if (m_fontAsset == nullptr)
		{
			std::cout << "SDLT_SFP_FA==N: FAILED TO OPEN UNIQUE FONT: ERROR DETAILS:" << SDL_GetError() << std::endl;
		}
	}

	//SDL_RWclose(wopStream);
}

void mediaShared::media::SDLText::SetFont(std::string pathToFontFile, unsigned int size, bool isUnique)
{
	if (isUnique == false)
	{
		if (m_s_fontCache.count(pathToFontFile + std::to_string(size)) != 0)
		{
			m_fontAsset = m_s_fontCache[pathToFontFile + std::to_string(size)];
			m_isFontUnique = false;
		}
		else
		{
			m_fontAsset = TTF_OpenFont(pathToFontFile.c_str(), size);

			if (m_fontAsset != nullptr)//to avoid caching invalid data
			{
				m_s_fontCache[pathToFontFile + std::to_string(size)] = m_fontAsset;
				m_isFontUnique = false;
			}
			else
			{
				std::cout << "SDLT_SF_FA==N: FAILED TO OPEN NON UNIQUE FONT: ERROR DETAILS:" << SDL_GetError() << std::endl;
			}
		}
	}
	else
	{
		m_fontAsset = TTF_OpenFont(pathToFontFile.c_str(), size);
		m_isFontUnique = true;

		if (m_fontAsset == nullptr)
		{
			std::cout << "SDLT_SF_FA==N: FAILED TO OPEN UNIQUE FONT: ERROR DETAILS:" << SDL_GetError() << std::endl;
		}
	}
}

void mediaShared::media::SDLText::SetText(SDL_Renderer* renderer, std::string text)
{
	if (m_fontAsset == nullptr)
	{
		return; //someting went wrong
	}

	if (m_textureOfText != nullptr)
	{
		SDL_DestroyTexture(m_textureOfText); //remove old text
	}

	TTF_SizeText(m_fontAsset, text.c_str(), &m_transformInnerPosAndSize.w, &m_transformInnerPosAndSize.h); //dose new text

	SDL_Surface* Surface = TTF_RenderText_Solid(m_fontAsset, text.c_str(), SDL_Color{ 255, 255, 255, 255 });
	m_textureOfText = SDL_CreateTextureFromSurface(renderer, Surface);
	SDL_FreeSurface(Surface);

	m_s_textureBuffer[reinterpret_cast<char*>(this)] = m_textureOfText;

	RecalulateAlignment(); //so it is in the same place as before
}

std::array<int, 2> mediaShared::media::SDLText::GetSizeOfIfTextWas(std::string text)
{
	std::array<int, 2> sizeArray = std::array<int, 2>();
	TTF_SizeText(m_fontAsset, text.c_str(), &sizeArray[0], &sizeArray[1]);

	return sizeArray;
}

void mediaShared::media::SDLText::RenderText(SDL_Renderer* renderer)
{
	Uint8 RecivedColorR, RecivedColorG, RecivedColorB, RecivedColorA;

	if (m_textureOfText == nullptr || m_fontAsset == nullptr)
	{
		return;
	}

	SDL_Rect parentClip = SDL_Rect();
	bool isParentUseingClipping = SDL_RenderIsClipEnabled(renderer);
	SDL_RenderGetClipRect(renderer, &parentClip);

	SDL_Rect renderRect = m_transformOuterPosAndSize;
	
	if (isParentUseingClipping == true)
	{
		if (SDL_IntersectRect(&parentClip, &m_transformOuterPosAndSize, &renderRect) == false)
		{
			return;
		}
	}

	SDL_RenderSetClipRect(renderer, &renderRect); //so text cant leave area

	if (SDL_GetRenderDrawColor(renderer, &RecivedColorR, &RecivedColorG, &RecivedColorB, &RecivedColorA) >= 0) //dose color support
	{
		SDL_SetTextureAlphaMod(m_textureOfText, RecivedColorA);
		SDL_SetTextureColorMod(m_textureOfText, RecivedColorR, RecivedColorG, RecivedColorB);
	}

	SDL_RenderCopyEx(renderer, m_textureOfText, NULL, &m_transformInnerPosAndSize, m_transformInnerRotation, NULL, m_transformInnerFlipState); //render

	SDL_SetTextureAlphaMod(m_textureOfText, 255);
	SDL_SetTextureColorMod(m_textureOfText, 255, 255, 255);

	SDL_RenderSetClipRect(renderer, &parentClip); //restores area to normal
	if (isParentUseingClipping == false) { SDL_RenderSetClipRect(renderer, NULL); }
}


int mediaShared::media::SDLText::GetTextWidth()
{
	return m_transformInnerPosAndSize.w;
}

int mediaShared::media::SDLText::GetTextHeight()
{
	return m_transformInnerPosAndSize.h;
}

void mediaShared::media::SDLText::SetPos(std::array<int, 2> pos) //outer >> recalulate/inner
{
	m_transformOuterPosAndSize.x = pos[0];
	m_transformOuterPosAndSize.y = pos[1];

	RecalulateAlignment();
}

void mediaShared::media::SDLText::SetSize(std::array<int, 2> size)  //outer >> recalulate
{
	m_transformOuterPosAndSize.w = size[0];
	m_transformOuterPosAndSize.h = size[1];

	RecalulateAlignment();
}

void mediaShared::media::SDLText::SetFlip(SDL_RendererFlip flip) //inner
{
	m_transformInnerFlipState = flip;
}

void mediaShared::media::SDLText::SetRotation(float rotation) //inner
{
	m_transformInnerRotation = rotation;
}

void mediaShared::media::SDLText::SetHorizontalAlignment(SDLText_HorizontalLocation location) //inner >> racalulate
{
	m_transformInnerHorizontalLocationOfText = location;

	RecalulateAlignment();
}

void mediaShared::media::SDLText::SetVerticalAlignment(SDLText_VerticalLocation location) //inner >> recalulate
{
	m_transformInnerVerticalLocationOfText = location;

	RecalulateAlignment();
}

void mediaShared::media::SDLText::SetMargins(std::array<int, 2> margins) //outer+ inner >> recalulate
{
	m_innerOuterMargin = margins;

	RecalulateAlignment();
}


std::array<int, 2> mediaShared::media::SDLText::GetPos() //outer
{
	return std::array<int, 2>{ m_transformOuterPosAndSize.x, m_transformOuterPosAndSize.y };
}

std::array<int, 2> mediaShared::media::SDLText::GetSize() //outer
{
	return std::array<int, 2>{ m_transformOuterPosAndSize.w, m_transformOuterPosAndSize.h };
}

SDL_RendererFlip mediaShared::media::SDLText::GetFlip() //inner
{
	return m_transformInnerFlipState;
}

float mediaShared::media::SDLText::GetRotation() //inner
{
	return m_transformInnerRotation;
}

mediaShared::media::SDLText_HorizontalLocation mediaShared::media::SDLText::GetHorizontalAlignment() //inner
{
	return m_transformInnerHorizontalLocationOfText;
}

mediaShared::media::SDLText_VerticalLocation mediaShared::media::SDLText::GetVerticalAlignment() //inner
{
	return m_transformInnerVerticalLocationOfText;
}

std::array<int, 2> mediaShared::media::SDLText::GetMargins() //outer + inner
{
	return m_innerOuterMargin;
}

TTF_Font* mediaShared::media::SDLText::GetUnderlyingFontAsset()
{
	return m_fontAsset;
}


bool mediaShared::media::SDLText::IsAssetsValid()
{
	return (m_fontAsset != nullptr && m_textureOfText != nullptr);
}

bool mediaShared::media::SDLText::IsFontUnique()
{
	return m_isFontUnique;
}
