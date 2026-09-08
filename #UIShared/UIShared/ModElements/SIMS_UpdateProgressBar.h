#pragma once

#pragma once

#include <chrono>
#include <fstream>
#include <string>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

#include "scriptManagment.h"

namespace UIShared
{
	class SIMS_UpdateProgressBar : public stackExternal::UI::UIElement
	{
	private:
		std::chrono::steady_clock::time_point m_progrssBarHideAnimationTimePoint = std::chrono::steady_clock::now();
		std::chrono::steady_clock::time_point m_progrssBarMainAnimationTimePoint = std::chrono::steady_clock::now();
		float m_progrssBarAnimationTime = 0;
		float m_hideTimeMax = 0.2f;

		std::array<float,2> GetIndeterminateProgressBarPoints(float progressTime, float timeMul, float detail, float minSize, float maxSize)
		{
			float progressBarProgress = ((float)(((int)(progressTime * timeMul)) % ((int)detail))) / detail;
			float width = 1.0f - (std::abs(progressBarProgress - 0.5f) / 0.5f);
			width = (minSize + width * (maxSize - minSize));

			return std::array<float, 2>{progressBarProgress, width};
		}

	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			if (sfra::scrman::IsUpdateInProgress() == true)
			{
				std::chrono::steady_clock::time_point timePointNow = std::chrono::steady_clock::now();
				m_progrssBarAnimationTime = std::chrono::duration<float>{ timePointNow - m_progrssBarMainAnimationTimePoint }.count();
			}


			//gets render bounds
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();


			if (sfra::scrman::IsUpdateInProgress() == false && sfra::scrman::DidUpdateWork() == true)
			{
				std::chrono::steady_clock::time_point timePointNow = std::chrono::steady_clock::now();
				float currentTimeOnScale = (std::chrono::duration<float>{ timePointNow - m_progrssBarHideAnimationTimePoint }.count() / m_hideTimeMax);

				bounds.offsetPos[1] = bounds.offsetPos[1] + (((float)(bounds.offsetSize[1])) * currentTimeOnScale);
				bounds.offsetSize[1] = (bounds.offsetSize[1] -(((float)(bounds.offsetSize[1])) * currentTimeOnScale)) +1;
			}
			else
			{
				m_progrssBarHideAnimationTimePoint = std::chrono::steady_clock::now();
			}

			//get bar bounds
			float time = m_progrssBarAnimationTime;
			float timeMul = 800.0f;
			float detailOverTime = 1000.0f;
			float barMinSize = 0.016f;
			float barMaxSize = 0.45f;
			std::array<float, 2> pos = GetIndeterminateProgressBarPoints(time, timeMul, detailOverTime, barMinSize, barMaxSize);


			SDL_Rect renderRectBg = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
			SDL_Rect renderRectFg = SDL_Rect{ bounds.offsetPos[0] +(int)(((float)bounds.offsetSize[0]) * pos[0]), bounds.offsetPos[1], (int)(((float)bounds.offsetSize[0]) * pos[1]), bounds.offsetSize[1]};


			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorWorkingBg.r, m_colorWorkingBg.g, m_colorWorkingBg.b, m_colorWorkingBg.a);
			if (sfra::scrman::DidUpdateWork() == false && sfra::scrman::IsUpdateInProgress() == false) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorFailureBg.r, m_colorFailureBg.g, m_colorFailureBg.b, m_colorFailureBg.a); }
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &renderRectBg);


			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorWorkingFg.r, m_colorWorkingFg.g, m_colorWorkingFg.b, m_colorWorkingFg.a);
			if (sfra::scrman::DidUpdateWork() == false && sfra::scrman::IsUpdateInProgress() == false) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorFailureFg.r, m_colorFailureFg.g, m_colorFailureFg.b, m_colorFailureFg.a); }
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &renderRectFg);
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
		
		}

	public:
		SDL_Color m_colorWorkingBg = SDL_Color{ 0, 150, 0, 255 };
		SDL_Color m_colorWorkingFg = SDL_Color{ 150, 255, 0, 255 };

		SDL_Color m_colorFailureBg = SDL_Color{200, 0, 0, 255,};
		SDL_Color m_colorFailureFg = SDL_Color{ 255, 50, 50, 255 };

	};
}