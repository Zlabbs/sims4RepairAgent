#pragma once

#include <map>
#include <string>

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

#include "MasterList.h"

namespace UIShared
{
	stackExternal::UI::UITransform DecodeTransform(stackExternal::assets::AssetObjectContainer* asset)
	{
		unsigned int jumpPoint = 0;
		stackExternal::UI::UITransform returnTransform = stackExternal::UI::UITransform();

		stackExternal::assets::AssetObjectContainer offsetPosArray = asset->GetSubObjectsByScope(0, "<offsetPos>", "</offsetPos>");
		if (offsetPosArray.IsContentsValid() == false) { return returnTransform; } //object dosent have a normal transform

		jumpPoint = 0;
		returnTransform.bounds.offsetPos[0] = offsetPosArray.GetSubObjectByArrayElement(0, ",", &jumpPoint).GetContentsInt();
		returnTransform.bounds.offsetPos[1] = offsetPosArray.GetSubObjectByArrayElement(0, ",", &jumpPoint).GetContentsInt();

		jumpPoint = 0;
		stackExternal::assets::AssetObjectContainer offsetSizeArray = asset->GetSubObjectsByScope(0, "<offsetSize>", "</offsetSize>");
		returnTransform.bounds.offsetSize[0] = offsetSizeArray.GetSubObjectByArrayElement(0, ",", &jumpPoint).GetContentsInt();
		returnTransform.bounds.offsetSize[1] = offsetSizeArray.GetSubObjectByArrayElement(0, ",", &jumpPoint).GetContentsInt();

		jumpPoint = 0;
		stackExternal::assets::AssetObjectContainer percentageSizeArray = asset->GetSubObjectsByScope(0, "<percentageSize>", "</percentageSize>");
		returnTransform.percentageSize[0] = percentageSizeArray.GetSubObjectByArrayElement(0, ",", &jumpPoint).GetContentsFloat();
		returnTransform.percentageSize[1] = percentageSizeArray.GetSubObjectByArrayElement(0, ",", &jumpPoint).GetContentsFloat();

		jumpPoint = 0;
		stackExternal::assets::AssetObjectContainer anchorPosArray = asset->GetSubObjectsByScope(0, "<anchorPos>", "</anchorPos>");
		returnTransform.anchorPos[0] = anchorPosArray.GetSubObjectByArrayElement(0, ",", &jumpPoint).GetContentsFloat();
		returnTransform.anchorPos[1] = anchorPosArray.GetSubObjectByArrayElement(0, ",", &jumpPoint).GetContentsFloat();

		jumpPoint = 0;
		stackExternal::assets::AssetObjectContainer pivotPosArray = asset->GetSubObjectsByScope(0, "<pivotPos>", "</pivotPos>");
		returnTransform.pivotPos[0] = pivotPosArray.GetSubObjectByArrayElement(0, ",", &jumpPoint).GetContentsFloat();
		returnTransform.pivotPos[1] = pivotPosArray.GetSubObjectByArrayElement(0, ",", &jumpPoint).GetContentsFloat();

		return returnTransform;
	}

	void DecodeAssestToUiElement(void* propertyContext, stackExternal::UI::UITreeContiner* tree, stackExternal::assets::AssetObjectContainer* asset, bool createDebugUiElements)
	{
		stackExternal::UI::UIElement* baseElement = nullptr;
		bool createdObsoleteEntity = false;

		//decode asset class here
		std::string classType = std::string(asset->GetSubObjectsByScope(0, "<class>", "</class>").GetContents(), asset->GetSubObjectsByScope(0, "<class>", "</class>").GetContentsLength());
		if (classType == "obsolete")
		{
			if (createDebugUiElements == true)
			{
				stackExternal::UI::UIElement* debugElement = new UIE_hidden_DebugBox();
				debugElement->SetLocalTransform(DecodeTransform(asset));
				tree->AddElement(debugElement);
			}

			createdObsoleteEntity = true;
			classType = std::string(asset->GetSubObjectsByScope(0, "<cclass>", "</cclass>").GetContents(), asset->GetSubObjectsByScope(0, "<cclass>", "</cclass>").GetContentsLength());
		}

		if (classType == "SIMS_ID_DownloadButton") { baseElement = new SIMS_ID_DownloadButton(); }
		else if (classType == "SIMS_ID_DonateButton") { baseElement = new SIMS_ID_DonateButton(); }
		else if (classType == "SIMS_CC_ID_SearchBox") { baseElement = new SIMS_CC_ID_SearchBox(); }
		else if (classType == "SIMS_CC_ID_ExecPath") { baseElement = new SIMS_CC_ID_ExecPath(); }
		else if (classType == "SIMS_UpdateProgressBar") { baseElement = new SIMS_UpdateProgressBar(); }
		else if (classType == "SIMS_CC_ID_AutoFixes") { baseElement = new SIMS_CC_ID_AutoFixes(); }
		else if (classType == "SIMS_CC_ID_MasterList") { baseElement = new SIMS_CC_ID_MasterList(); }

		else if (classType == "UIE_CC_ImageBox") { baseElement = new UIE_CC_ImageBox(); } //I know this sucks but thats life
		else if (classType == "UIE_CC_TextElement") { baseElement = new UIE_CC_TextElement(); }
		else if (classType == "UIE_ColorBox") { baseElement = new UIE_ColorBox(); }
		else if (classType == "UIE_ColorBoxOutline") { baseElement = new UIE_ColorBoxOutline(); }
		else if (classType == "UIE_CollapseToContentsBox") { baseElement = new UIE_CollapseToContentsBox(); }
		else if (classType == "UIE_HideableBox") { baseElement = new UIE_HideableBox(); }
		else if (classType == "UIE_ProgressBar") { baseElement = new UIE_ProgressBar(); }
		else if (classType == "UIE_SceneAttachedRoot") { baseElement = new UIE_SceneAttachedRoot(); }
		else if (classType == "UIE_ScreenRoot") { baseElement = new UIE_ScreenRoot(); }

		else if (classType == "UIE_VerticalList") { baseElement = new UIE_VerticalList(); }
		else if (classType == "UIE_HorizontalList") { baseElement = new UIE_HorizontalList(); }

		else if (classType == "UIE_ID_Window") { baseElement = new UIE_ID_Window(); }
		else if (classType == "UIE_CC_ID_Button") { baseElement = new UIE_CC_ID_Button(); }
		else if (classType == "UIE_ID_HideButton") { baseElement = new UIE_ID_HideButton(); }
		else if (classType == "UIE_ID_BlockRenderButton") { baseElement = new UIE_ID_BlockRenderButton(); }
		else if (classType == "UIE_CC_ID_CheckBox") { baseElement = new UIE_CC_ID_CheckBox(); }
		else if (classType == "UIE_ID_ScrollerButton") { baseElement = new UIE_ID_ScrollerButton(); }
		else if (classType == "UIE_CC_ID_HorizontalSlider") { baseElement = new UIE_CC_ID_HorizontalSlider(); }
		else if (classType == "UIE_CC_ID_ReactiveTextElement") { baseElement = new UIE_CC_ID_ReactiveTextElement(); }
		else if (classType == "UIE_CC_ID_KeyButton") {baseElement = new UIE_CC_ID_KeyButton(); }
		else if (classType == "UIE_CC_KeyEvent") { baseElement = new UIE_CC_KeyEvent(); }
		else if (classType == "UIE_KeyEventRenderToggle") { baseElement = new UIE_KeyEventRenderToggle(); }

		else if (classType == "UIE_ID_SSS_VerticalScroller") { baseElement = new UIE_ID_SSS_VerticalScroller(); }
		else if (classType == "UIE_SSS_ScissorModifyer") { baseElement = new UIE_SSS_ScissorModifyer(); }

		

		if (baseElement != nullptr)
		{
			//decode children
			tree->PushElementDomain(baseElement);

			//decode setting
			baseElement->LoadPropities(propertyContext, asset->GetContents(), asset->GetContentsLength());

			//decode transform
			baseElement->SetLocalTransform(DecodeTransform(asset));

			stackExternal::assets::AssetObjectContainer children = asset->GetSubObjectsByScopeExpand(0, "<children>", "</children>");
			unsigned int jumpPoint = 0;
			for (unsigned int i = 0; i < children.GetSubObjectsByScopeExpandLength("<uiObject>", "</uiObject>"); i++)
			{
				stackExternal::assets::AssetObjectContainer child = asset->GetSubObjectsByScopeExpand(0, "<uiObject>", "</uiObject>", &jumpPoint);
				DecodeAssestToUiElement(propertyContext, tree, &child, createDebugUiElements);
			}

			tree->PopElementDomain();
		}
	}

	stackExternal::UI::UITreeContiner* DecodeAssestToUiTree(void* propertyContext, stackExternal::assets::AssetObjectContainer* asset, bool createDebugUiElements)
	{
		stackExternal::UI::UITreeContiner* returnTree = new stackExternal::UI::UITreeContiner();

		//decode container settings tagName
		returnTree->SetTagName(std::string(asset->GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset->GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength()).c_str());
		returnTree->m_shouldReciveEvents = asset->GetSubObjectsByScope(0, "<shouldReciveEvents>", "</shouldReciveEvents>").GetContentsBool();
		returnTree->m_shouldDebugRender = asset->GetSubObjectsByScope(0, "<shouldDebugRender>", "</shouldDebugRender>").GetContentsBool();
		returnTree->m_shouldRender = asset->GetSubObjectsByScope(0, "<shouldRender>", "</shouldRender>").GetContentsBool();

		stackExternal::assets::AssetObjectContainer children = asset->GetSubObjectsByScopeExpand(0, "<children>", "</children>");
		
		if (createDebugUiElements == false)
		{
			returnTree->m_shouldDebugRender = false;
		}

		//decode children
		unsigned int jumpPoint = 0;
		for (unsigned int i = 0; i < children.GetSubObjectsByScopeExpandLength("<uiObject>", "</uiObject>"); i++)
		{
			stackExternal::assets::AssetObjectContainer child = children.GetSubObjectsByScopeExpand(0, "<uiObject>", "</uiObject>", &jumpPoint);
			DecodeAssestToUiElement(propertyContext, returnTree, &child, createDebugUiElements);
		}

		returnTree->FlushElementDomainStack();

		return returnTree;
	}
}