#pragma once

#include "SDL.h"

#include <string>

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"
#include "MediaShared/SDLText/SDLText.h"

#include "UIComponent_InputData.h"
#include "UIComponent_ConnectionContext.h"

namespace UIShared
{
	class UIE_CC_ID_ReactiveTextElement : public stackExternal::UI::UIElement, UIComponent_ConnectionContext, UIComponent_InputData
	{
	protected:
		virtual void OnRemove() override
		{
			m_textBaseObject.FreeFont(); //frees text stuff
			m_textBaseObject.FreeTextTexture();
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			//set up text render bounds
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
			m_textBaseObject.SetPos({ bounds.offsetPos[0], bounds.offsetPos[1] });
			m_textBaseObject.SetSize({ bounds.offsetSize[0], bounds.offsetSize[1] });

			//rects
			SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
			SDL_Rect mousePos = SDL_Rect{ GetMousePos()[0], GetMousePos()[1], 1, 1 };

			//renders text
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorNorm.r, m_colorNorm.g, m_colorNorm.b, m_colorNorm.a);
			if (SDL_HasIntersection(&mousePos, &renderRect) == true) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorHover.r, m_colorHover.g, m_colorHover.b, m_colorHover.a); }
			m_textBaseObject.RenderText((SDL_Renderer*)renderingContext);


			//debug rendering (ncase text is invisible from missing font)
			if (renderDebug == false) { return; }
			SDL_Rect rectDebug = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, 255, 0, 255, 255);
			SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &rectDebug);
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  

			//text aligment
			m_textBaseObject.SetHorizontalAlignment((mediaShared::media::SDLText_HorizontalLocation)(asset.GetSubObjectsByScope(0, "<horizontalAlignment>", "</horizontalAlignment>").GetContentsInt()));
			m_textBaseObject.SetVerticalAlignment((mediaShared::media::SDLText_VerticalLocation)(asset.GetSubObjectsByScope(0, "<verticalAlignment>", "</verticalAlignment>").GetContentsInt()));

			//font
			int fontSize = asset.GetSubObjectsByScope(0, "<textSize>", "</textSize>").GetContentsInt();
			std::string font = std::string(asset.GetSubObjectsByScope(0, "<font>", "</font>").GetContents(), asset.GetSubObjectsByScope(0, "<font>", "</font>").GetContentsLength());

			char* fileData = nullptr;
			unsigned int fileSize = 0;
			GetItemFromPack(font, &fileData, &fileSize);

			if (fileSize > 0)
			{
				m_textBaseObject.SetFontPack(font, fileData, fileSize, fontSize);
			}

			//text
			std::string text = std::string(asset.GetSubObjectsByScope(0, "<text>", "</text>").GetContents(), asset.GetSubObjectsByScope(0, "<text>", "</text>").GetContentsLength());
			m_textBaseObject.SetText((SDL_Renderer*)propertyContext, text);

			//color stuff
			stackExternal::assets::AssetObjectContainer colorArrayN = asset.GetSubObjectsByScope(0, "<colorNorm>", "</colorNorm>");
			m_colorNorm.r = colorArrayN.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorNorm.g = colorArrayN.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorNorm.b = colorArrayN.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorNorm.a = colorArrayN.GetSubObjectByArrayElement(3).GetContentsInt();


			stackExternal::assets::AssetObjectContainer colorArrayH = asset.GetSubObjectsByScope(0, "<colorHover>", "</colorHover>");
			m_colorHover.r = colorArrayH.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorHover.g = colorArrayH.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorHover.b = colorArrayH.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorHover.a = colorArrayH.GetSubObjectByArrayElement(3).GetContentsInt();
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength); //settting font and text aligment imposible because I say so

			//set color
			stackExternal::assets::AssetObjectContainer colorArrayN = asset.GetSubObjectsByScope(0, "<colorNorm>", "</colorNorm>");
			if (colorArrayN.IsContentsValid() == true)
			{
				m_colorNorm.r = colorArrayN.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorNorm.g = colorArrayN.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorNorm.b = colorArrayN.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorNorm.a = colorArrayN.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}

			stackExternal::assets::AssetObjectContainer colorArrayH = asset.GetSubObjectsByScope(0, "<colorHover>", "</colorHover>");
			if (colorArrayH.IsContentsValid() == true)
			{
				m_colorHover.r = colorArrayH.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorHover.g = colorArrayH.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorHover.b = colorArrayH.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorHover.a = colorArrayH.GetSubObjectByArrayElement(3).GetContentsInt();
			}


			//set text
			if (asset.GetSubObjectsByScope(0, "<text>", "</text>").IsContentsValid() == true)
			{
				std::string text = std::string(asset.GetSubObjectsByScope(0, "<text>", "</text>").GetContents(), asset.GetSubObjectsByScope(0, "<text>", "</text>").GetContentsLength());
				m_textBaseObject.SetText((SDL_Renderer*)propertyContext, text);
				return;
			}
		}

	public:
		mediaShared::media::SDLText m_textBaseObject = mediaShared::media::SDLText();
		SDL_Color m_colorNorm = SDL_Color();
		SDL_Color m_colorHover = SDL_Color();
	};
}