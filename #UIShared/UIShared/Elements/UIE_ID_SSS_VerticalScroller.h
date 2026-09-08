#pragma once

#include <math.h>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"
#include "StandardShared/OtherMaths/OtherMaths.h"

#include "UIComponent_InputData.h"
#include "UIComponent_SDLScissorStack.h"

namespace UIShared
{
	class UIE_ID_SSS_VerticalScroller : public stackExternal::UI::UIElement, UIComponent_InputData, UIComponent_SDLScissorStack
	{
	private:
		float m_contentsOffsetY = 0; //a 0 to 1 repasentation of where the scroller really is
		int m_contentsHeightOld = 0;
		int m_contentsHeightCurrent = 0;

		int m_dragOffsetY = 0;
		bool m_inDragMode = false;

	protected:
		virtual stackExternal::UI::UITransformBounds GetWorldTransformBoundsForChild(UIElement* childInQuestion) override
		{
			stackExternal::UI::UITransformBounds parentBounds = stackExternal::UI::UITransformBounds();
			stackExternal::UI::UITransformBounds returnBounds = stackExternal::UI::UITransformBounds();

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

				float diffrance = (float)(std::max(m_contentsHeightOld - parentBounds.offsetSize[1], 0));
				returnBounds.offsetPos[1] = (returnBounds.offsetPos[1] + m_childrenVerticalPadding) -((int)(diffrance * m_contentsOffsetY));
			}

			int depthOfCurrentElement = ((returnBounds.offsetPos[1] + returnBounds.offsetSize[1]) - parentBounds.offsetPos[1]) + m_childrenVerticalPadding;
			if (depthOfCurrentElement > m_contentsHeightCurrent)
			{
				m_contentsHeightCurrent = depthOfCurrentElement;
			}
			 
			return returnBounds;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{	
			if (abs(m_contentsHeightCurrent - m_contentsHeightOld) >= 25)
			{
				m_contentsHeightOld = m_contentsHeightCurrent;
			}
			m_contentsHeightCurrent = 0;

			//gets the mouse pos
			std::array<int, 2> mousePosRaw = GetMousePos();
			SDL_Rect mousePos = SDL_Rect{ mousePosRaw[0], mousePosRaw[1], 1, 1 };

			//gets the bounds
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
			SDL_Rect contentsRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0] -(m_barThickness +1), bounds.offsetSize[1]};
			SDL_Rect barRect = SDL_Rect{ (bounds.offsetPos[0] + bounds.offsetSize[0]) - (m_barThickness + 1), bounds.offsetPos[1], (m_barThickness + 1), bounds.offsetSize[1] };

			//gets dragger rect
			int draggerSize = std::max(bounds.offsetSize[1] - std::max(m_contentsHeightOld - bounds.offsetSize[1], 0), m_minScrollerHeight);
			SDL_Rect draggerRect = SDL_Rect{ (bounds.offsetPos[0] + bounds.offsetSize[0]) - (m_barThickness + 1), bounds.offsetPos[1] + m_scrollerLocationY, m_barThickness + 1, draggerSize };


			if (m_hideBarIfUneeded == false || draggerSize < bounds.offsetSize[1])
			{
				//renders bar background
				SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorBackground.r, m_colorBackground.g, m_colorBackground.b, m_colorBackground.a);
				SDL_RenderFillRect((SDL_Renderer*)renderingContext, &barRect);

				//renders dragger
				SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorNorm.r, m_colorNorm.g, m_colorNorm.b, m_colorNorm.a);
				if (SDL_HasIntersection(&mousePos, &draggerRect) == true || m_inDragMode == true) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorHover.r, m_colorHover.g, m_colorHover.b, m_colorHover.a); }
				SDL_RenderFillRect((SDL_Renderer*)renderingContext, &draggerRect);


				//renders separator
				SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorSeparator.r, m_colorSeparator.g, m_colorSeparator.b, m_colorSeparator.a);
				SDL_RenderDrawLine((SDL_Renderer*)renderingContext, barRect.x, barRect.y, barRect.x, barRect.y + (barRect.h - 1));
			}

			if (m_inDragMode == true)
			{
				int scrollersNewYLocation = std::min(std::max(mousePos.y + m_dragOffsetY, bounds.offsetPos[1]), (bounds.offsetPos[1] + bounds.offsetSize[1]) - draggerSize) - bounds.offsetPos[1];
				m_scrollerLocationY = scrollersNewYLocation;
				m_contentsOffsetY = standardShared::otherMaths::Remap((float)(scrollersNewYLocation), 0, (float)((bounds.offsetSize[1]) - draggerSize), 0, 1);

				if (!(m_contentsOffsetY >= 0 && m_contentsOffsetY <= 1)) { m_contentsOffsetY = 0; }
			}
			else //incase the box the scroller is in expands or contracts
			{
				//scroller maths
				int scrollersNewYLocation = std::max(m_scrollerLocationY, 0);
				scrollersNewYLocation = std::min(scrollersNewYLocation, bounds.offsetSize[1] - draggerSize);
				m_scrollerLocationY = scrollersNewYLocation;

				m_contentsOffsetY = standardShared::otherMaths::Remap((float)(scrollersNewYLocation), 0, (float)((bounds.offsetSize[1]) - draggerSize), 0, 1);

				if (!(m_contentsOffsetY >= 0 && m_contentsOffsetY <= 1)) { m_contentsOffsetY = 0; }
			}


			//creates sissor area for the children
			contentsRect.y = contentsRect.y - m_viewerVerticalPadding;
			contentsRect.h = contentsRect.h + (m_viewerVerticalPadding * 2);

			if (renderDebug == true) //renders sissior
			{
				SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, 0, 255, 0, 255);
				SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &contentsRect);
			}

			//cuts sissor area
			PushToStack((SDL_Renderer*)renderingContext, contentsRect);
		}

		virtual bool OnEvent(unsigned int eventId, void* eventData) override
		{
			if (eventId == UISHAR__UICMD__RESET)
			{
				m_scrollerLocationY = 0;
				m_contentsOffsetY = 0;
				m_inDragMode = false;
			}

			if (eventId == UISHAR__UICMD__SDL_EVENT && GetAttachedTree()->m_shouldRender == true) //is an event from sdl
			{
				SDL_Event* event = (SDL_Event*)eventData;

				if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
				{
					stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
					int draggerSize = std::max(bounds.offsetSize[1] - std::max(m_contentsHeightOld - bounds.offsetSize[1], 0), m_minScrollerHeight);
					SDL_Rect draggerRect = SDL_Rect{ (bounds.offsetPos[0] + bounds.offsetSize[0]) - (m_barThickness + 1), bounds.offsetPos[1] + m_scrollerLocationY, m_barThickness + 1, draggerSize };

					if (m_hideBarIfUneeded == false || draggerSize < bounds.offsetSize[1])
					{
						SDL_Rect mousePos = SDL_Rect{ GetMousePos()[0], GetMousePos()[1], 1, 1 };

						if (SDL_HasIntersection(&mousePos, &draggerRect) == true)
						{
							m_inDragMode = true;
							m_dragOffsetY = (bounds.offsetPos[1] + m_scrollerLocationY) - mousePos.y;

							return true;
						}
					}
				}

				if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT)
				{
					m_inDragMode = false;
				}
			}

			return false;
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  

			//shape
			m_barThickness = asset.GetSubObjectsByScope(0, "<barThickness>", "</barThickness>").GetContentsInt();
			m_minScrollerHeight = asset.GetSubObjectsByScope(0, "<minScrollerHeight>", "</minScrollerHeight>").GetContentsInt();
			m_hideBarIfUneeded = asset.GetSubObjectsByScope(0, "<hideBarIfUneeded>", "</hideBarIfUneeded>").GetContentsBool();

			//padding
			m_viewerVerticalPadding = asset.GetSubObjectsByScope(0, "<viewerVerticalPadding>", "</viewerVerticalPadding>").GetContentsInt();
			m_childrenVerticalPadding = asset.GetSubObjectsByScope(0, "<childrenVerticalPadding>", "</childrenVerticalPadding>").GetContentsInt();


			//color stuff
			stackExternal::assets::AssetObjectContainer colorArrayH = asset.GetSubObjectsByScope(0, "<colorHover>", "</colorHover>");
			m_colorHover.r = colorArrayH.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorHover.g = colorArrayH.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorHover.b = colorArrayH.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorHover.a = colorArrayH.GetSubObjectByArrayElement(3).GetContentsInt();

			//color stuff
			stackExternal::assets::AssetObjectContainer colorArrayN = asset.GetSubObjectsByScope(0, "<colorNorm>", "</colorNorm>");
			m_colorNorm.r = colorArrayN.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorNorm.g = colorArrayN.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorNorm.b = colorArrayN.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorNorm.a = colorArrayN.GetSubObjectByArrayElement(3).GetContentsInt();

			//color stuff
			stackExternal::assets::AssetObjectContainer colorArrayB = asset.GetSubObjectsByScope(0, "<colorBackground>", "</colorBackground>");
			m_colorBackground.r = colorArrayB.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorBackground.g = colorArrayB.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorBackground.b = colorArrayB.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorBackground.a = colorArrayB.GetSubObjectByArrayElement(3).GetContentsInt();


			//color stuff
			stackExternal::assets::AssetObjectContainer colorArrayS = asset.GetSubObjectsByScope(0, "<colorSeparator>", "</colorSeparator>");
			m_colorSeparator.r = colorArrayS.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorSeparator.g = colorArrayS.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorSeparator.b = colorArrayS.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorSeparator.a = colorArrayS.GetSubObjectByArrayElement(3).GetContentsInt();
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);

			if (asset.GetSubObjectsByScope(0, "<barThickness>", "</barThickness>").IsContentsValid() == true)
			{
				m_barThickness = asset.GetSubObjectsByScope(0, "<barThickness>", "</barThickness>").GetContentsInt();
				return;
			}

			if (asset.GetSubObjectsByScope(0, "<barThickness>", "</barThickness>").IsContentsValid() == true)
			{
				m_minScrollerHeight = asset.GetSubObjectsByScope(0, "<minScrollerHeight>", "</minScrollerHeight>").GetContentsInt();
				return;
			}

			if (asset.GetSubObjectsByScope(0, "<barThickness>", "</barThickness>").IsContentsValid() == true)
			{
				m_hideBarIfUneeded = asset.GetSubObjectsByScope(0, "<hideBarIfUneeded>", "</hideBarIfUneeded>").GetContentsBool();
				return;
			}

			//color stuff
			stackExternal::assets::AssetObjectContainer colorArrayH = asset.GetSubObjectsByScope(0, "<colorHover>", "</colorHover>");
			if (colorArrayH.IsContentsValid() == true)
			{
				m_colorHover.r = colorArrayH.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorHover.g = colorArrayH.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorHover.b = colorArrayH.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorHover.a = colorArrayH.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}

			//color stuff
			stackExternal::assets::AssetObjectContainer colorArrayN = asset.GetSubObjectsByScope(0, "<colorNorm>", "</colorNorm>");
			if (colorArrayN.IsContentsValid() == true)
			{
				m_colorNorm.r = colorArrayN.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorNorm.g = colorArrayN.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorNorm.b = colorArrayN.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorNorm.a = colorArrayN.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}

			//color stuff
			stackExternal::assets::AssetObjectContainer colorArrayB = asset.GetSubObjectsByScope(0, "<colorBackground>", "</colorBackground>");
			if (colorArrayB.IsContentsValid() == true)
			{
				m_colorBackground.r = colorArrayB.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorBackground.g = colorArrayB.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorBackground.b = colorArrayB.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorBackground.a = colorArrayB.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}


			//color stuff
			stackExternal::assets::AssetObjectContainer colorArrayS = asset.GetSubObjectsByScope(0, "<colorSeparator>", "</colorSeparator>");
			if (colorArrayS.IsContentsValid() == true)
			{
				m_colorSeparator.r = colorArrayS.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorSeparator.g = colorArrayS.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorSeparator.b = colorArrayS.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorSeparator.a = colorArrayS.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}

		}

	public:
		void SetScrollerLocationCapped(int inputY)
		{
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
			int draggerSize = std::max(bounds.offsetSize[1] - std::max(m_contentsHeightOld - bounds.offsetSize[1], 0), m_minScrollerHeight);

			int scrollersNewYLocation = std::max(inputY, 0);
			scrollersNewYLocation = std::min(scrollersNewYLocation, bounds.offsetSize[1] - draggerSize);
			m_scrollerLocationY = scrollersNewYLocation;
		}

		bool m_hideBarIfUneeded = true;

		int m_scrollerLocationY = 0;
		int m_barThickness = 4;
		int m_minScrollerHeight = 40;

		int m_viewerVerticalPadding = 0;
		int m_childrenVerticalPadding = 0;

		SDL_Color m_colorHover = SDL_Color();
		SDL_Color m_colorNorm = SDL_Color();
		SDL_Color m_colorBackground = SDL_Color();
		SDL_Color m_colorSeparator = SDL_Color();
	};
}