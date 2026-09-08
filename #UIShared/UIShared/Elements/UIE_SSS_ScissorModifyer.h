
#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

#include "UIComponent_SDLScissorStack.h"

namespace UIShared
{
	class UIE_SSS_ScissorModifyer : public stackExternal::UI::UIElement, UIComponent_SDLScissorStack
	{
	protected:
		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			if (m_isEnd == false)
			{
				stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
				SDL_Rect boundsRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };

				if (renderDebug == true)
				{
					SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, 0, 255, 0, 255);
					SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &boundsRect);
				}

				PushToStack((SDL_Renderer*)renderingContext, boundsRect);
			}
			else
			{
				PopFromStack((SDL_Renderer*)renderingContext);
			}
		}

	public:
		bool m_isEnd = false;

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  
			m_isEnd = asset.GetSubObjectsByScope(0, "<isEnd>", "</isEnd>").GetContentsBool();
		}

	};
}