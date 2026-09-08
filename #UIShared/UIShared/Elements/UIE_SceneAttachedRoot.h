#pragma once

#include <array>

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

namespace UIShared
{
	class UIE_SceneAttachedRoot : public stackExternal::UI::UIElement
	{
	private:
		static void* m_owner;

	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual stackExternal::UI::UITransformBounds GetWorldTransformBoundsForChild(UIElement* childInQuestion) override
		{
			//set up bounds
			stackExternal::UI::UITransformBounds parentBounds = stackExternal::UI::UITransformBounds();
			stackExternal::UI::UITransformBounds returnBounds = stackExternal::UI::UITransformBounds();

			//where it is in scene + its offset pos and size
			parentBounds.offsetPos[0] = m_rootBounds.offsetPos[0] + m_posInScene[0];
			parentBounds.offsetPos[1] = m_rootBounds.offsetPos[1] + m_posInScene[1];
			parentBounds.offsetSize[0] = m_rootBounds.offsetSize[0];
			parentBounds.offsetSize[1] = m_rootBounds.offsetSize[1];
			
			//child transform
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
			m_rootBounds.offsetPos[0] = posAddArray.GetSubObjectByArrayElement(0).GetContentsInt();
			m_rootBounds.offsetPos[1] = posAddArray.GetSubObjectByArrayElement(1).GetContentsInt();

			//size subtract
			stackExternal::assets::AssetObjectContainer sizeSubtract = asset.GetSubObjectsByScope(0, "<sizeAdd>", "</sizeAdd>");
			m_rootBounds.offsetSize[0] = -sizeSubtract.GetSubObjectByArrayElement(0).GetContentsInt();
			m_rootBounds.offsetSize[1] = -sizeSubtract.GetSubObjectByArrayElement(1).GetContentsInt();
		}

	public:
		std::array<int, 2> m_posInScene = std::array<int, 2>();
		stackExternal::UI::UITransformBounds m_rootBounds = stackExternal::UI::UITransformBounds();

		static void SetUiOwner(void* owner)
		{
			m_owner = owner;
		}

		static void* GetUiOwner()
		{
			return m_owner;
		}

	};

	void* UIE_SceneAttachedRoot::m_owner = nullptr;
}