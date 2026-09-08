#pragma once

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

namespace UIShared
{
	class UIE_ColorBoxOutline : public stackExternal::UI::UIElement
	{
	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			//gets render bounts and sets outline color
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_mainColor.r, m_mainColor.g, m_mainColor.b, m_mainColor.a);


			//render 1 pixel thicc boxes insided of each other untill line thiccness has been met
			for (int i = 0; i < m_lineThickness; i++)
			{
				SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0] + i, bounds.offsetPos[1] + i, bounds.offsetSize[0] - (i *2), bounds.offsetSize[1] - (i * 2)};
				SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &renderRect);
			}
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  

			//line stuff
			m_lineThickness = asset.GetSubObjectsByScope(0, "<lineThickness>", "</lineThickness>").GetContentsInt();

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

			//line stuff
			if (asset.GetSubObjectsByScope(0, "<lineThickness>", "</lineThickness>").IsContentsValid() == true)
			{
				m_lineThickness = asset.GetSubObjectsByScope(0, "<lineThickness>", "</lineThickness>").GetContentsInt();
				return;
			}

			//color
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
		int m_lineThickness = 2;
	};
}