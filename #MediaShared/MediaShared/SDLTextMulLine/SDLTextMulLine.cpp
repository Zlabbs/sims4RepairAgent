#include "SDLTextMulLine.h"

void mediaShared::media::SDLTextMulLine::CalulateMulLineText(SDL_Renderer* renderer, std::string text, std::vector<std::string> newLineChars, std::string font, int fontSize, std::array<int, 2> pos, int width, int spacing, SDLText_VerticalLocation vpos, SDLText_HorizontalLocation hpos)
{
	m_allTextElements.clear();

	if (renderer == nullptr || m_text == "" || m_font == "" || m_fontSize == 0)
	{
		return;
	}

	SDLText createFontInCache = SDLText();
	createFontInCache.SetFont(font, fontSize);

	std::string textLineBuffer = "";
	int currentTextHeightOffset = 0;

	for (size_t i = 0; i < text.length(); i++)
	{
		int width = 0;
		int height = 0;

		textLineBuffer = textLineBuffer + text[i];
		TTF_SizeText(createFontInCache.GetUnderlyingFontAsset(), textLineBuffer.c_str(), &width, &height);

		int findLength = 0;

		for (size_t o = 0; o < newLineChars.size(); o++)
		{
			if (textLineBuffer.substr(textLineBuffer.length() - newLineChars[o].length(), std::string::npos) == newLineChars[o]);
			{
				findLength = newLineChars[o].length();
				break;
			}
		}

		if (width >= m_width || findLength > 0)
		{
			SDLText mainText = SDLText();
			mainText.SetFont(font, fontSize);
			mainText.SetPos({ pos[0], pos[1] + currentTextHeightOffset });
			mainText.SetSize({ width, height });
			mainText.SetText(renderer, textLineBuffer.substr(0, textLineBuffer.size() - std::max(findLength,1)));
			mainText.SetVerticalAlignment(vpos);
			mainText.SetHorizontalAlignment(hpos);

			currentTextHeightOffset = currentTextHeightOffset + height + spacing;
			textLineBuffer = "";

			m_allTextElements.push_back(mainText);
		}
	}
}


void mediaShared::media::SDLTextMulLine::SetNewLineSeparators(SDL_Renderer* renderer, std::vector<std::string> newLines)
{
	m_newLineChars = newLines;
	CalulateMulLineText(renderer, m_text, m_newLineChars, m_font, m_fontSize, m_pos, m_width, m_lineSpacing, m_transformInnerVerticalLocationOfText, m_transformInnerHorizontalLocationOfText);
}

void mediaShared::media::SDLTextMulLine::SetVerticalAlignment(SDL_Renderer* renderer, SDLText_VerticalLocation location)
{
	m_transformInnerVerticalLocationOfText = location;
	CalulateMulLineText(renderer, m_text, m_newLineChars, m_font, m_fontSize, m_pos, m_width, m_lineSpacing, m_transformInnerVerticalLocationOfText, m_transformInnerHorizontalLocationOfText);
}

void mediaShared::media::SDLTextMulLine::SetHorizontalAlignment(SDL_Renderer* renderer, SDLText_HorizontalLocation location)
{
	m_transformInnerHorizontalLocationOfText = location;
	CalulateMulLineText(renderer, m_text, m_newLineChars, m_font, m_fontSize, m_pos, m_width, m_lineSpacing, m_transformInnerVerticalLocationOfText, m_transformInnerHorizontalLocationOfText);
}

void mediaShared::media::SDLTextMulLine::SetPos(SDL_Renderer* renderer, std::array<int, 2> pos)
{
	m_pos = pos;
	CalulateMulLineText(renderer, m_text, m_newLineChars, m_font, m_fontSize, m_pos, m_width, m_lineSpacing, m_transformInnerVerticalLocationOfText, m_transformInnerHorizontalLocationOfText);
}

void mediaShared::media::SDLTextMulLine::SetWidth(SDL_Renderer* renderer, int width)
{
	m_width = width;
	CalulateMulLineText(renderer, m_text, m_newLineChars, m_font, m_fontSize, m_pos, m_width, m_lineSpacing, m_transformInnerVerticalLocationOfText, m_transformInnerHorizontalLocationOfText);
}

void mediaShared::media::SDLTextMulLine::SetLineSpacing(SDL_Renderer* renderer, int space)
{
	m_lineSpacing = space;
	CalulateMulLineText(renderer, m_text, m_newLineChars, m_font, m_fontSize, m_pos, m_width, m_lineSpacing, m_transformInnerVerticalLocationOfText, m_transformInnerHorizontalLocationOfText);
}


std::vector<std::string> mediaShared::media::SDLTextMulLine::GetNewLineSeparators()
{
	return m_newLineChars;
}

mediaShared::media::SDLText_VerticalLocation mediaShared::media::SDLTextMulLine::GetVerticalAlignment()
{
	return m_transformInnerVerticalLocationOfText;
}

mediaShared::media::SDLText_HorizontalLocation mediaShared::media::SDLTextMulLine::GetHorizontalAlignment()
{
	return m_transformInnerHorizontalLocationOfText;
}

std::array<int, 2> mediaShared::media::SDLTextMulLine::GetPos()
{
	return m_pos;
}

int mediaShared::media::SDLTextMulLine::GetWidth()
{
	return m_width;
}

int mediaShared::media::SDLTextMulLine::SetLineSpacing()
{
	return m_lineSpacing;
}


void mediaShared::media::SDLTextMulLine::SetFontPack(std::string pathToFontFile, char* packItemData, unsigned int packItemSize, unsigned int size)
{
	m_font = pathToFontFile;
	m_fontSize = size;

	SDLText createFontInCache = SDLText();
	createFontInCache.SetFontPack(pathToFontFile, packItemData, packItemSize, size);
}

void mediaShared::media::SDLTextMulLine::SetFont(std::string pathToFontFile, unsigned int size)
{
	m_font = pathToFontFile;
	m_fontSize = size;

	SDLText createFontInCache = SDLText();
	createFontInCache.SetFont(pathToFontFile, size);
}

void mediaShared::media::SDLTextMulLine::SetText(SDL_Renderer* renderer, std::string text)
{
	m_text = text;
	CalulateMulLineText(renderer, m_text, m_newLineChars, m_font, m_fontSize, m_pos, m_width, m_lineSpacing, m_transformInnerVerticalLocationOfText, m_transformInnerHorizontalLocationOfText);
}

void mediaShared::media::SDLTextMulLine::RenderText(SDL_Renderer* renderer)
{
	for (size_t i = 0; i < m_allTextElements.size(); i++)
	{
		m_allTextElements[i].RenderText(renderer);
	}
}