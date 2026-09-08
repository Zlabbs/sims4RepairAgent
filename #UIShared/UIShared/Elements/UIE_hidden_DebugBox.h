#pragma once

#include "SDL.h"

#include "UISource/UISource.h"

namespace UIShared
{
	class UIE_hidden_DebugBox : public stackExternal::UI::UIElement
	{
	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			if (renderDebug == false) { return; }

			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, 255, 232, 80, 255);

			SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
			SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &renderRect);
		}
	};
}