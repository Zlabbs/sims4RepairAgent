#pragma once

#include <vector>
#include <string>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

#include "UIE_HideableBox.h"

#include "UIComponent_InputData.h"
#include "UIComponent_SDLScissorStack.h"

namespace UIShared
{
	class UIE_ID_HideButton : public stackExternal::UI::UIElement, UIComponent_InputData
	{
	private:
		bool m_isVisible = false;
		bool m_initalStateCache = false;

	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			if (m_initalStateCache == false)
			{
				UIE_HideableBox* tempAccessToBox = ((UIE_HideableBox*)GetAttachedTree()->GetElementByTagName(m_tagNameOfBoxToRender.c_str()));
				if (tempAccessToBox != nullptr)
				{
					m_isVisible = !(tempAccessToBox->GetHiddenState());
				}

				m_initalStateCache = true;
			}

			//gets render bounds
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
			SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
			SDL_Rect mousePos = SDL_Rect{ GetMousePos()[0], GetMousePos()[1], 1, 1 };

			//renders box fill
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorNorm.r, m_colorNorm.g, m_colorNorm.b, m_colorNorm.a);
			if (m_isVisible == true) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorNormVis.r, m_colorNormVis.g, m_colorNormVis.b, m_colorNormVis.a); }
			if (SDL_HasIntersection(&mousePos, &renderRect) == true) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorHover.r, m_colorHover.g, m_colorHover.b, m_colorHover.a); }
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &renderRect);


			//render box outline
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorOutline.r, m_colorOutline.g, m_colorOutline.b, m_colorOutline.a);
			if (m_isVisible == true) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorOutlineVis.r, m_colorOutlineVis.g, m_colorOutlineVis.b, m_colorOutlineVis.a); }
			for (int i = 0; i < m_lineThickness; i++)
			{
				SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &renderRect);
				renderRect.x = renderRect.x + 1;
				renderRect.y = renderRect.y + 1;
				renderRect.w = renderRect.w - 2;
				renderRect.h = renderRect.h - 2;
			}
		}

		virtual bool OnEvent(unsigned int eventId, void* eventData) override
		{
			if (eventId == UISHAR__HIDEBOX_STATE_CHANGE_EVENT)
			{
				UIE_HideableBox* box = (UIE_HideableBox*)eventData;
				if (std::string(box->GetTagName()) == m_tagNameOfBoxToRender)
				{
					m_isVisible = !box->GetHiddenState();
				}
			}

			if (eventId == UISHAR__UICMD__SDL_EVENT && GetAttachedTree()->m_shouldRender == true) //is an event from sdl
			{
				SDL_Event* event = (SDL_Event*)eventData;

				if (event->type == SDL_MOUSEBUTTONDOWN)
				{
					stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
					SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
					SDL_Rect mousePos = SDL_Rect{ GetMousePos()[0], GetMousePos()[1], 1, 1 };



					if (SDL_HasIntersection(&mousePos, &renderRect) == true)
					{
						if (UIComponent_SDLScissorStack::IsEmpty() == false) //sissor check just to make sure
						{
							SDL_Rect checkRect = UIComponent_SDLScissorStack::Top();
							if(SDL_HasIntersection(&renderRect, &checkRect) == false) //oh no we cant handle the event
							{
								return false;
							}
						}

						UIE_HideableBox* tempAccessToBox = ((UIE_HideableBox*)GetAttachedTree()->GetElementByTagName(m_tagNameOfBoxToRender.c_str()));
						if (tempAccessToBox != nullptr)
						{
							GetAttachedTree()->PassEventAsListUnhandleable(UISHAR__UICMD__RESET, nullptr); //so everything is reset between switching tabs
							tempAccessToBox->SetHideState(false);
						}

						for (size_t i = 0; i < m_tagNamesOfBoxesToHide.size(); i++)
						{
							UIE_HideableBox* tempAccessToBoxThatNeedsHiding = ((UIE_HideableBox*)GetAttachedTree()->GetElementByTagName(m_tagNamesOfBoxesToHide[i].c_str()));
							if (tempAccessToBoxThatNeedsHiding != nullptr)
							{
								 tempAccessToBoxThatNeedsHiding->SetHideState(true);
							}
						}

						return true;
					}
				}
			}

			return false;
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  

			//main box
			m_tagNameOfBoxToRender = std::string(asset.GetSubObjectsByScope(0, "<tagNameBoxToRender>", "</tagNameBoxToRender>").GetContents(), asset.GetSubObjectsByScope(0, "<tagNameBoxToRender>", "</tagNameBoxToRender>").GetContentsLength());


			//other boxes
			stackExternal::assets::AssetObjectContainer hideBoxesArray = asset.GetSubObjectsByScope(0, "<tagNamesOfBoxesToHide>", "</tagNamesOfBoxesToHide>");
			for (unsigned int i = 0; i < hideBoxesArray.GetSubObjectByArrayElementLength(); i++)
			{
				m_tagNamesOfBoxesToHide.push_back(std::string(hideBoxesArray.GetSubObjectByArrayElement(i).GetContents(), hideBoxesArray.GetSubObjectByArrayElement(i).GetContentsLength()));
			}


			//line stuff
			m_lineThickness = asset.GetSubObjectsByScope(0, "<lineThickness>", "</lineThickness>").GetContentsInt();

			//color stuff colorHover
			stackExternal::assets::AssetObjectContainer colorArrayH = asset.GetSubObjectsByScope(0, "<colorHover>", "</colorHover>");
			m_colorHover.r = colorArrayH.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorHover.g = colorArrayH.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorHover.b = colorArrayH.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorHover.a = colorArrayH.GetSubObjectByArrayElement(3).GetContentsInt();


			//color stuff colorNorm
			stackExternal::assets::AssetObjectContainer colorArrayN = asset.GetSubObjectsByScope(0, "<colorNorm>", "</colorNorm>");
			m_colorNorm.r = colorArrayN.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorNorm.g = colorArrayN.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorNorm.b = colorArrayN.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorNorm.a = colorArrayN.GetSubObjectByArrayElement(3).GetContentsInt();

			//color stuff colorOuline
			stackExternal::assets::AssetObjectContainer colorArrayO = asset.GetSubObjectsByScope(0, "<colorOutline>", "</colorOutline>");
			m_colorOutline.r = colorArrayO.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorOutline.g = colorArrayO.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorOutline.b = colorArrayO.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorOutline.a = colorArrayO.GetSubObjectByArrayElement(3).GetContentsInt();

			//set color normal
			stackExternal::assets::AssetObjectContainer colorArrayNB = asset.GetSubObjectsByScope(0, "<colorNormBoxVis>", "</colorNormBoxVis>");
			m_colorNormVis.r = colorArrayNB.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorNormVis.g = colorArrayNB.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorNormVis.b = colorArrayNB.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorNormVis.a = colorArrayNB.GetSubObjectByArrayElement(3).GetContentsInt();


			//set color outline
			stackExternal::assets::AssetObjectContainer colorArrayOB = asset.GetSubObjectsByScope(0, "<colorOutlineBoxVis>", "</colorOutlineBoxVis>");
			m_colorOutlineVis.r = colorArrayOB.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorOutlineVis.g = colorArrayOB.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorOutlineVis.b = colorArrayOB.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorOutlineVis.a = colorArrayOB.GetSubObjectByArrayElement(3).GetContentsInt();
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);


			//line stuff
			stackExternal::assets::AssetObjectContainer lineThickness = asset.GetSubObjectsByScope(0, "<lineThickness>", "</lineThickness>");
			if (lineThickness.IsContentsValid() == true)
			{
				m_lineThickness = lineThickness.GetContentsInt();
				return;
			}


			//set color hover
			stackExternal::assets::AssetObjectContainer colorArrayH = asset.GetSubObjectsByScope(0, "<colorHover>", "</colorHover>");
			if (colorArrayH.IsContentsValid() == true)
			{
				m_colorHover.r = colorArrayH.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorHover.g = colorArrayH.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorHover.b = colorArrayH.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorHover.a = colorArrayH.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}


			//set color normal
			stackExternal::assets::AssetObjectContainer colorArrayN = asset.GetSubObjectsByScope(0, "<colorNorm>", "</colorNorm>");
			if (colorArrayN.IsContentsValid() == true)
			{
				m_colorNorm.r = colorArrayN.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorNorm.g = colorArrayN.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorNorm.b = colorArrayN.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorNorm.a = colorArrayN.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}


			//set color outline
			stackExternal::assets::AssetObjectContainer colorArrayO = asset.GetSubObjectsByScope(0, "<colorOutline>", "</colorOutline>");
			if (colorArrayO.IsContentsValid() == true)
			{
				m_colorOutline.r = colorArrayO.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorOutline.g = colorArrayO.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorOutline.b = colorArrayO.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorOutline.a = colorArrayO.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}

			//set color normal
			stackExternal::assets::AssetObjectContainer colorArrayNB = asset.GetSubObjectsByScope(0, "<colorNormBoxVis>", "</colorNormBoxVis>");
			if (colorArrayNB.IsContentsValid() == true)
			{
				m_colorNormVis.r = colorArrayNB.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorNormVis.g = colorArrayNB.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorNormVis.b = colorArrayNB.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorNormVis.a = colorArrayNB.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}


			//set color outline
			stackExternal::assets::AssetObjectContainer colorArrayOB = asset.GetSubObjectsByScope(0, "<colorOutlineBoxVis>", "</colorOutlineBoxVis>");
			if (colorArrayO.IsContentsValid() == true)
			{
				m_colorOutlineVis.r = colorArrayOB.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorOutlineVis.g = colorArrayOB.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorOutlineVis.b = colorArrayOB.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorOutlineVis.a = colorArrayOB.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}
		}

	public:
		int m_lineThickness = 2;

		std::string m_tagNameOfBoxToRender = "";
		std::vector<std::string> m_tagNamesOfBoxesToHide = std::vector<std::string>();

		SDL_Color m_colorHover = SDL_Color();
		SDL_Color m_colorNorm = SDL_Color();
		SDL_Color m_colorOutline = SDL_Color();
		SDL_Color m_colorOutlineVis = SDL_Color();
		SDL_Color m_colorNormVis = SDL_Color();
	};
}