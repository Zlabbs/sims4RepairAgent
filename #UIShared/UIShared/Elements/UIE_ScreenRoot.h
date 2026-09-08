#pragma once

#include <array>

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

namespace UIShared
{
	class UIE_ScreenRoot : public stackExternal::UI::UIElement
	{
	private:
		static stackExternal::UI::UITransformBounds m_windowSize;

	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual stackExternal::UI::UITransformBounds GetWorldTransformBoundsForChild(UIElement* childInQuestion) override
		{
			stackExternal::UI::UITransformBounds parentBounds = stackExternal::UI::UITransformBounds();
			parentBounds.offsetPos[0] = m_windowSize.offsetPos[0] + m_screenBounds.offsetPos[0];
			parentBounds.offsetPos[1] = m_windowSize.offsetPos[1] + m_screenBounds.offsetPos[1];
			parentBounds.offsetSize[0] = m_windowSize.offsetSize[0] +m_screenBounds.offsetSize[0];
			parentBounds.offsetSize[1] = m_windowSize.offsetSize[1] +m_screenBounds.offsetSize[1];
			
			stackExternal::UI::UITransformBounds returnBounds = stackExternal::UI::UITransformBounds();
			stackExternal::UI::UITransform childTransform = childInQuestion->GetLocalTransform();

			//child{w,h} = child{w,h} + (parent{w,h} * child{psw,psh});
			returnBounds.offsetSize[0] = childTransform.bounds.offsetSize[0] + (int)((float)(parentBounds.offsetSize[0]) * childTransform.percentageSize[0]);
			returnBounds.offsetSize[1] = childTransform.bounds.offsetSize[1] + (int)((float)(parentBounds.offsetSize[1]) * childTransform.percentageSize[1]);

			//child{x,y} = child{x,y} + parent{x,y} +(parent{w,h} * child{apx,apy});
			returnBounds.offsetPos[0] = childTransform.bounds.offsetPos[0] + parentBounds.offsetPos[0] + (int)((float)(parentBounds.offsetSize[0]) * childTransform.anchorPos[0]);
			returnBounds.offsetPos[1] = childTransform.bounds.offsetPos[1] + parentBounds.offsetPos[1] + (int)((float)(parentBounds.offsetSize[1]) * childTransform.anchorPos[1]);

			//child{x,y} = child{x,y} -(child{w,h} * child{pp,pp});
			returnBounds.offsetPos[0] = returnBounds.offsetPos[0] - (int)((float)(returnBounds.offsetSize[0]) * childTransform.pivotPos[0]);
			returnBounds.offsetPos[1] = returnBounds.offsetPos[1] - (int)((float)(returnBounds.offsetSize[1]) * childTransform.pivotPos[1]);

			return returnBounds;
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);

			//pos add
			stackExternal::assets::AssetObjectContainer posAddArray = asset.GetSubObjectsByScope(0, "<posAdd>", "</posAdd>");
			m_screenBounds.offsetPos[0] = posAddArray.GetSubObjectByArrayElement(0).GetContentsInt();
			m_screenBounds.offsetPos[1] = posAddArray.GetSubObjectByArrayElement(1).GetContentsInt();

			//size subtract
			stackExternal::assets::AssetObjectContainer sizeSubtract = asset.GetSubObjectsByScope(0, "<sizeSubtract>", "</sizeSubtract>");
			m_screenBounds.offsetSize[0] = -sizeSubtract.GetSubObjectByArrayElement(0).GetContentsInt();
			m_screenBounds.offsetSize[1] = -sizeSubtract.GetSubObjectByArrayElement(1).GetContentsInt();
		}

	public:
		stackExternal::UI::UITransformBounds m_screenBounds = stackExternal::UI::UITransformBounds();

		static void SetScreenSize(std::array<int, 2> size)
		{
			m_windowSize.offsetSize[0] = size[0];
			m_windowSize.offsetSize[1] = size[1];
		}

		static void SetScreenOffset(std::array<int, 2> offset)
		{
			m_windowSize.offsetPos[0] = offset[0];
			m_windowSize.offsetPos[1] = offset[1];
		}
	};


	stackExternal::UI::UITransformBounds UIE_ScreenRoot::m_windowSize = stackExternal::UI::UITransformBounds();
}