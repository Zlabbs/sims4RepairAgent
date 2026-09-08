#pragma once

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

namespace UIShared
{
	class UIE_ProgressBar : public stackExternal::UI::UIElement
	{
	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();

			//renders progress bar background
			SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_subColor.r, m_subColor.g, m_subColor.b, m_subColor.a);
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &renderRect);

			//render the how much of the progress mar forground is full
			SDL_Rect renderRectB = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], (int)(((float)bounds.offsetSize[0]) * m_barProgression), bounds.offsetSize[1] };
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_mainColor.r, m_mainColor.g, m_mainColor.b, m_mainColor.a);
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &renderRectB);
	
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  

			//progression
			m_barProgression = asset.GetSubObjectsByScope(0, "<barProgression>", "</barProgression>").GetContentsFloat();

			//color stuff //forground
			stackExternal::assets::AssetObjectContainer colorArray = asset.GetSubObjectsByScope(0, "<colorBar>", "</colorBar>");
			m_mainColor.r = colorArray.GetSubObjectByArrayElement(0).GetContentsInt();
			m_mainColor.g = colorArray.GetSubObjectByArrayElement(1).GetContentsInt();
			m_mainColor.b = colorArray.GetSubObjectByArrayElement(2).GetContentsInt();
			m_mainColor.a = colorArray.GetSubObjectByArrayElement(3).GetContentsInt();

			//color stuff part two //background
			stackExternal::assets::AssetObjectContainer colorArrayB = asset.GetSubObjectsByScope(0, "<colorBackground>", "</colorBackground>");
			m_subColor.r = colorArrayB.GetSubObjectByArrayElement(0).GetContentsInt();
			m_subColor.g = colorArrayB.GetSubObjectByArrayElement(1).GetContentsInt();
			m_subColor.b = colorArrayB.GetSubObjectByArrayElement(2).GetContentsInt();
			m_subColor.a = colorArrayB.GetSubObjectByArrayElement(3).GetContentsInt();
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);

			//bar fullness
			if (asset.GetSubObjectsByScope(0, "<barProgression>", "</barProgression>").IsContentsValid() == true)
			{
				m_barProgression = asset.GetSubObjectsByScope(0, "<barProgression>", "</barProgression>").GetContentsFloat();
				return;
			}

			//progress bar color
			stackExternal::assets::AssetObjectContainer colorArray = asset.GetSubObjectsByScope(0, "<colorBar>", "</colorBar>");
			if (colorArray.IsContentsValid() == true)
			{
				m_mainColor.r = colorArray.GetSubObjectByArrayElement(0).GetContentsInt();
				m_mainColor.g = colorArray.GetSubObjectByArrayElement(1).GetContentsInt();
				m_mainColor.b = colorArray.GetSubObjectByArrayElement(2).GetContentsInt();
				m_mainColor.a = colorArray.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}

			//progress bar background
			stackExternal::assets::AssetObjectContainer colorArrayB = asset.GetSubObjectsByScope(0, "<colorBackground>", "</colorBackground>");
			if (colorArrayB.IsContentsValid() == true)
			{
				m_mainColor.r = colorArrayB.GetSubObjectByArrayElement(0).GetContentsInt();
				m_mainColor.g = colorArrayB.GetSubObjectByArrayElement(1).GetContentsInt();
				m_mainColor.b = colorArrayB.GetSubObjectByArrayElement(2).GetContentsInt();
				m_mainColor.a = colorArrayB.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}
		}

	public:
		float m_barProgression = 0.75f;
		SDL_Color m_mainColor = SDL_Color();
		SDL_Color m_subColor = SDL_Color();
	};
}