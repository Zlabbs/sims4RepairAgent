#pragma once

#include <array>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

#include "UIComponent_InputData.h"

#define UISHAR__HIDEBOX_STATE_CHANGE_EVENT 1212

namespace UIShared
{
	class UIE_HideableBox : public stackExternal::UI::UIElement
	{
	private:
		bool m_isHidden = false;
		bool m_isHiddenIntialState = false;

	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			//debug rendering
			if (renderDebug == false) { return; }
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
			SDL_Rect rectDebug = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, 255, 80, 0, 255);
			SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &rectDebug);
		}

		virtual bool OnEvent(unsigned int eventId, void* eventData) override
		{
			if (eventId == UISHAR__UICMD__RESET && m_ingoreResetEvents == false)
			{
				m_isHidden = m_isHiddenIntialState;
				GetAttachedTree()->PassEventAsListUnhandleable(UISHAR__HIDEBOX_STATE_CHANGE_EVENT, this);
			}

			return false;
		}

		virtual stackExternal::UI::UITransformBounds GetWorldTransformBoundsForChild(UIElement* childInQuestion) override
		{
			stackExternal::UI::UITransformBounds parentBounds = stackExternal::UI::UITransformBounds();
			stackExternal::UI::UITransformBounds returnBounds = stackExternal::UI::UITransformBounds();

			if (m_isHidden == true) //moves box off screeen and all of it
			{
				returnBounds.offsetPos[0] = -8048;
				returnBounds.offsetPos[1] = 0;

				returnBounds.offsetSize[0] = 100;
				returnBounds.offsetSize[1] = 100;
				return returnBounds;
			}

			UIElement* parent = GetParent();
			stackExternal::UI::UITransform childTransform = childInQuestion->GetLocalTransform();

			if (parent != nullptr)
			{
				parentBounds = parent->GetWorldTransformBoundsForChild(this);

				//child{w,h} = child{w,h} + (parent{w,h} * child{psw,psh});
				returnBounds.offsetSize[0] = childTransform.bounds.offsetSize[0] + (int)((float)(parentBounds.offsetSize[0]) * childTransform.percentageSize[0]);
				returnBounds.offsetSize[1] = childTransform.bounds.offsetSize[1] + (int)((float)(parentBounds.offsetSize[1]) * childTransform.percentageSize[1]);

				//child{x,y} = child{x,y} + parent{x,y} +(parent{w,h} * child{apx,apy});
				returnBounds.offsetPos[0] = childTransform.bounds.offsetPos[0] + parentBounds.offsetPos[0] + (int)((float)(parentBounds.offsetSize[0]) * childTransform.anchorPos[0]);
				returnBounds.offsetPos[1] = childTransform.bounds.offsetPos[1] + parentBounds.offsetPos[1] + (int)((float)(parentBounds.offsetSize[1]) * childTransform.anchorPos[1]);

				//child{x,y} = child{x,y} -(child{w,h} * child{pp,pp});
				returnBounds.offsetPos[0] = returnBounds.offsetPos[0] - (int)((float)(returnBounds.offsetSize[0]) * childTransform.pivotPos[0]);
				returnBounds.offsetPos[1] = returnBounds.offsetPos[1] - (int)((float)(returnBounds.offsetSize[1]) * childTransform.pivotPos[1]);
			}

			return returnBounds;
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  

			//hide
			m_isHidden = asset.GetSubObjectsByScope(0, "<isHidden>", "</isHidden>").GetContentsBool();
			m_isHiddenIntialState = m_isHidden;

			//event ingore
			m_ingoreResetEvents = asset.GetSubObjectsByScope(0, "<ingoreResetEvents>", "</ingoreResetEvents>").GetContentsBool();
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);

			stackExternal::assets::AssetObjectContainer isHidden = asset.GetSubObjectsByScope(0, "<isHidden>", "</isHidden>");
			if (isHidden.IsContentsValid() == true)
			{
				m_isHidden = isHidden.GetContentsBool();
				GetAttachedTree()->PassEventAsListUnhandleable(UISHAR__HIDEBOX_STATE_CHANGE_EVENT, this);
			}


			stackExternal::assets::AssetObjectContainer ingoreResetEvents = asset.GetSubObjectsByScope(0, "<ingoreResetEvents>", "</ingoreResetEvents>");
			if (ingoreResetEvents.IsContentsValid() == true)
			{
				m_ingoreResetEvents = ingoreResetEvents.GetContentsBool();
			}
		}

	public:
		bool m_ingoreResetEvents = false;

		void SetHideState(bool state)
		{
			m_isHidden = state;
			GetAttachedTree()->PassEventAsListUnhandleable(UISHAR__HIDEBOX_STATE_CHANGE_EVENT, this);
		}

		bool GetHiddenState()
		{
			return m_isHidden;
		}
		
	};
}