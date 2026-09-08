#pragma once

#include <string>

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"
#include "MediaShared/SDLImage/SDLImage.h"

#include "UIComponent_ConnectionContext.h"

namespace UIShared
{
	class UIE_CC_ImageBox : public stackExternal::UI::UIElement, UIComponent_ConnectionContext
	{
	protected:
		virtual void OnRemove() override
		{
			m_imageBaseObject.FreeTexture();
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			//sets up area to draw image
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
			m_imageBaseObject.SetPos({ bounds.offsetPos[0], bounds.offsetPos[1] });
			m_imageBaseObject.SetSize({ bounds.offsetSize[0], bounds.offsetSize[1] });


			//draws image
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_imageColor.r, m_imageColor.g, m_imageColor.b, m_imageColor.a);
			m_imageBaseObject.RenderImageWithColorModifcation((SDL_Renderer*)renderingContext);


			//debug rendering (incase no image can be found as missing texture support isnt in editor yet)
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

			//image stuff
			std::string imagePath = std::string(asset.GetSubObjectsByScope(0, "<imagePath>", "</imagePath>").GetContents(), asset.GetSubObjectsByScope(0, "<imagePath>", "</imagePath>").GetContentsLength());

			char* fileData = nullptr;
			unsigned int fileSize = 0;
			GetItemFromPack(imagePath, &fileData, &fileSize);

			if (fileSize > 0)
			{
				m_imageBaseObject.SetTexturePack((SDL_Renderer*)propertyContext, imagePath, fileData, fileSize);
			}

			//color stuff
			stackExternal::assets::AssetObjectContainer colorArray = asset.GetSubObjectsByScope(0, "<color>", "</color>");
			m_imageColor.r = colorArray.GetSubObjectByArrayElement(0).GetContentsInt();
			m_imageColor.g = colorArray.GetSubObjectByArrayElement(1).GetContentsInt();
			m_imageColor.b = colorArray.GetSubObjectByArrayElement(2).GetContentsInt();
			m_imageColor.a = colorArray.GetSubObjectByArrayElement(3).GetContentsInt();
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);

			//set color
			stackExternal::assets::AssetObjectContainer colorArray = asset.GetSubObjectsByScope(0, "<color>", "</color>");
			if (colorArray.IsContentsValid() == true)
			{
				m_imageColor.r = colorArray.GetSubObjectByArrayElement(0).GetContentsInt();
				m_imageColor.g = colorArray.GetSubObjectByArrayElement(1).GetContentsInt();
				m_imageColor.b = colorArray.GetSubObjectByArrayElement(2).GetContentsInt();
				m_imageColor.a = colorArray.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}

			//set image
			if (asset.GetSubObjectsByScope(0, "<imagePath>", "</imagePath>").IsContentsValid() == true)
			{
				std::string imagePath = std::string(asset.GetSubObjectsByScope(0, "<imagePath>", "</imagePath>").GetContents(), asset.GetSubObjectsByScope(0, "<imagePath>", "</imagePath>").GetContentsLength());

				if (IsItemInPack(imagePath) == false)
				{
					m_imageBaseObject.SetTexture((SDL_Renderer*)propertyContext, imagePath);
				}
				else
				{
					char* fileData = nullptr;
					unsigned int fileSize = 0;
					GetItemFromPack(imagePath, &fileData, &fileSize);

					if (fileSize > 0)
					{
						m_imageBaseObject.SetTexturePack((SDL_Renderer*)propertyContext, imagePath, fileData, fileSize);
					}
				}
				return;
			}
		}

	public:
		mediaShared::media::SDLImage m_imageBaseObject = mediaShared::media::SDLImage();
		SDL_Color m_imageColor = SDL_Color();
	};
}