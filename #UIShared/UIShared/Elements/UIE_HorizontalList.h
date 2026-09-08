#pragma once

#include <map>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

namespace UIShared
{
	class UIE_HorizontalList : public stackExternal::UI::UIElement
	{
	private:
		int m_currentItemOffset = 0;
		std::map<stackExternal::UI::UIElement*, stackExternal::UI::UITransformBounds> m_cacheMap = std::map<stackExternal::UI::UIElement*, stackExternal::UI::UITransformBounds>();

	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			//reset this to 0 before we render any children
			m_currentItemOffset = 0;
			m_cacheMap.clear();

			//debug rendering
			if (renderDebug == false) { return; }
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
			SDL_Rect rectDebug = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, 0, 150, 255, 255);
			SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &rectDebug);
		}

		virtual stackExternal::UI::UITransformBounds GetWorldTransformBoundsForChild(UIElement* childInQuestion) override
		{
			if (m_cacheMap.count(childInQuestion) != 0) { return m_cacheMap[childInQuestion]; }

			stackExternal::UI::UITransformBounds parentBounds = stackExternal::UI::UITransformBounds(); //bounds store
			stackExternal::UI::UITransformBounds returnBounds = stackExternal::UI::UITransformBounds();

			UIElement* parent = GetParent();
			stackExternal::UI::UITransform childTransform = childInQuestion->GetLocalTransform();

			if (parent != nullptr)
			{
				parentBounds = parent->GetWorldTransformBoundsForChild(this);

				returnBounds.offsetSize[0] = childTransform.bounds.offsetSize[0]; //set width to width of list element
				returnBounds.offsetSize[1] = parentBounds.offsetSize[1]; // set height to height of list

				if (m_compact == false)
				{
					returnBounds.offsetSize[0] = (parentBounds.offsetSize[0] / GetChildLength()) - m_spacing; //if compact spacing is false it will force the elements to fill the space of the list as much as possible
					if (GetChild(GetChildLength() - 1) == childInQuestion) { returnBounds.offsetSize[0] = returnBounds.offsetSize[0] + m_spacing; }
				}

				returnBounds.offsetPos[0] = parentBounds.offsetPos[0] + m_currentItemOffset; //pos setting
				returnBounds.offsetPos[1] = parentBounds.offsetPos[1];

				m_currentItemOffset = m_currentItemOffset + returnBounds.offsetSize[0] + m_spacing; 
			}

			//colaps it to the contents
			if (m_collapseToContents == true && m_compact == true)
			{
				stackExternal::UI::UITransform trans = GetLocalTransform();
				trans.percentageSize[0] = 0;
				trans.bounds.offsetSize[0] = m_currentItemOffset - m_spacing;

				SetLocalTransform(trans);
			}

			m_cacheMap[childInQuestion] = returnBounds;
			return returnBounds;
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength()); 

			m_compact = asset.GetSubObjectsByScope(0, "<compact>", "</compact>").GetContentsBool();
			m_collapseToContents = asset.GetSubObjectsByScope(0, "<collapseToContents>", "</collapseToContents>").GetContentsBool();
			m_spacing = asset.GetSubObjectsByScope(0, "<spacing>", "</spacing>").GetContentsInt();
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			
			//compact
			if (asset.GetSubObjectsByScope(0, "<compact>", "</compact>").IsContentsValid() == true)
			{
				m_compact = asset.GetSubObjectsByScope(0, "<compact>", "</compact>").GetContentsBool();
				return;
			}

			if (asset.GetSubObjectsByScope(0, "<collapseToContents>", "</collapseToContents>").IsContentsValid() == true)
			{
				m_collapseToContents = asset.GetSubObjectsByScope(0, "<collapseToContents>", "</collapseToContents>").GetContentsBool();
				return;
			}

			//spacing
			if (asset.GetSubObjectsByScope(0, "<spacing>", "</spacing>").IsContentsValid() == true)
			{
				m_compact = asset.GetSubObjectsByScope(0, "<spacing>", "</spacing>").GetContentsInt();
				return;
			}
		}

	public:
		bool m_collapseToContents = true;
		bool m_compact = true;
		int m_spacing = 2;
	};
}