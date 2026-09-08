#pragma once

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

#include "UIComponent_InputData.h"

namespace UIShared
{
	class UIE_KeyEventRenderToggle : public stackExternal::UI::UIElement
	{
	private:

	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual bool OnEvent(unsigned int eventId, void* eventData) override
		{
			if (eventId == UISHAR__UICMD__SDL_EVENT) //is an event from sdl
			{
				SDL_Event* event = (SDL_Event*)eventData;
				stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();

				if (m_visibilityRequired == false || (bounds.offsetPos[0] + bounds.offsetSize[0] >= 0 && bounds.offsetPos[1] + bounds.offsetSize[1] >= 0 && GetAttachedTree()->m_shouldRender == true))
				{
					if (event->type == SDL_MOUSEBUTTONDOWN)
					{
						unsigned int key = (event->button.button);

						if (key == m_sdlMouseCode)
						{
							GetAttachedTree()->m_shouldRender = !GetAttachedTree()->m_shouldRender;
							if (GetAttachedTree()->m_shouldRender == false) { GetAttachedTree()->PassEventAsListUnhandleable(UISHAR__UICMD__RESET, nullptr); }

							return true;
						}
					}

					if (event->type == SDL_KEYDOWN)
					{
						unsigned int key = (unsigned int)(event->key.keysym.sym);

						if (key == m_sdlKeyCode)
						{
							GetAttachedTree()->m_shouldRender = !GetAttachedTree()->m_shouldRender;
							if (GetAttachedTree()->m_shouldRender == false) { GetAttachedTree()->PassEventAsListUnhandleable(UISHAR__UICMD__RESET, nullptr); }

							return true;
						}
					}
				}
			}

			return false;
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  

			m_visibilityRequired = asset.GetSubObjectsByScope(0, "<visibilityRequired>", "</visibilityRequired>").GetContentsBool(); //dose the event or the parent of the event I should say need to be visible
			m_sdlKeyCode = ((unsigned int)asset.GetSubObjectsByScope(0, "<sdlKeyCode>", "</sdlKeyCode>").GetContentsInt());
			m_sdlMouseCode = ((unsigned int)asset.GetSubObjectsByScope(0, "<sdlMouseCode>", "</sdlMouseCode>").GetContentsInt());
		}

	public:
		int m_visibilityRequired = true;
		unsigned int m_sdlKeyCode = 0;
		unsigned int m_sdlMouseCode = 0;

	};
}