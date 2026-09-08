#pragma once

#include <map>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

namespace UIShared
{
	class UIE_CollapseToContentsBox : public stackExternal::UI::UIElement
	{
	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			stackExternal::UI::UITransform trans = GetLocalTransform();
			trans.bounds.offsetSize[0] = 0;
			trans.bounds.offsetSize[1] = 0;
			trans.percentageSize[0] = 0;
			trans.percentageSize[1] = 0;
			SetLocalTransform(trans);

			int collapseWidth = 0;
			int collapseHeight = 0;

			for (unsigned int i = 0; i < GetChildLength(); i++)
			{
				stackExternal::UI::UIElement* element = GetChild(i);
				stackExternal::UI::UITransformBounds elementBounds = element->GetWorldTransformBounds();
				stackExternal::UI::UITransformBounds parentBounds = GetWorldTransformBounds();

				int elementWidth = (elementBounds.offsetPos[0] - parentBounds.offsetPos[0]) + elementBounds.offsetSize[0];
				int elementHeight = (elementBounds.offsetPos[1] - parentBounds.offsetPos[1]) + elementBounds.offsetSize[1];

				if (elementWidth > collapseWidth)
				{
					collapseWidth = elementWidth;
				}

				if (elementHeight > collapseHeight)
				{
					collapseHeight = elementHeight;
				}
			}

			trans.bounds.offsetSize[0] = collapseWidth + m_addedSize[0];
			trans.bounds.offsetSize[1] = collapseHeight + m_addedSize[1];
			trans.percentageSize[0] = 0;
			trans.percentageSize[1] = 0;

			SetLocalTransform(trans);
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  

			stackExternal::assets::AssetObjectContainer colorArray = asset.GetSubObjectsByScope(0, "<addedSize>", "</addedSize>");
			m_addedSize[0] = colorArray.GetSubObjectByArrayElement(0).GetContentsInt();
			m_addedSize[1] = colorArray.GetSubObjectByArrayElement(1).GetContentsInt();
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{

		}

	public:
		int m_addedSize[2] = {};
	};
}