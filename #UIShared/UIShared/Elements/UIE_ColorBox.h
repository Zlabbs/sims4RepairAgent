#pragma once

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

namespace UIShared
{
	class UIE_ColorBox : public stackExternal::UI::UIElement
	{
	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			//gets render bounds
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
			SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };

			//renders box
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_mainColor.r, m_mainColor.g, m_mainColor.b, m_mainColor.a);
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &renderRect);
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  

			//color stuff
			stackExternal::assets::AssetObjectContainer colorArray = asset.GetSubObjectsByScope(0, "<color>", "</color>");
			m_mainColor.r = colorArray.GetSubObjectByArrayElement(0).GetContentsInt();
			m_mainColor.g = colorArray.GetSubObjectByArrayElement(1).GetContentsInt();
			m_mainColor.b = colorArray.GetSubObjectByArrayElement(2).GetContentsInt();
			m_mainColor.a = colorArray.GetSubObjectByArrayElement(3).GetContentsInt();
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);

			//set color
			stackExternal::assets::AssetObjectContainer colorArray = asset.GetSubObjectsByScope(0, "<color>", "</color>");
			if (colorArray.IsContentsValid() == true)
			{
				m_mainColor.r = colorArray.GetSubObjectByArrayElement(0).GetContentsInt();
				m_mainColor.g = colorArray.GetSubObjectByArrayElement(1).GetContentsInt();
				m_mainColor.b = colorArray.GetSubObjectByArrayElement(2).GetContentsInt();
				m_mainColor.a = colorArray.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}
		}

	public:
		SDL_Color m_mainColor = SDL_Color();
	};
}