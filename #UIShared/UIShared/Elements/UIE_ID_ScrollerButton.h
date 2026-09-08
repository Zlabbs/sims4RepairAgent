#pragma once

#include <chrono>
#include <string>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"
#include "MediaShared/SDLImage/SDLImage.h"

#include "UIComponent_InputData.h"
#include "UIComponent_SDLScissorStack.h"

#include "UIE_ID_SSS_VerticalScroller.h"

#define UISHAR__SCROLLERBUTTON__SCROLLER_TICK_RATE 0.025

namespace UIShared
{
	class UIE_ID_ScrollerButton : public stackExternal::UI::UIElement, UIComponent_InputData
	{
	private:
		std::chrono::steady_clock::time_point m_timeBetweenScrolls = std::chrono::steady_clock::now();
		bool m_inUseMode = false;

		void MinulateScrollerToMoveInDirection(std::string name, int dir, int amount, std::chrono::steady_clock::time_point* tp, double timeAmount)
		{
			UIE_ID_SSS_VerticalScroller* scroller = (UIE_ID_SSS_VerticalScroller*)GetAttachedTree()->GetElementByTagName(name.c_str());

			std::chrono::steady_clock::time_point timePointNow = std::chrono::steady_clock::now();
			double lengthOfElapsedTime = std::chrono::duration<double>{ timePointNow - (*tp) }.count();
			if (lengthOfElapsedTime > timeAmount)
			{
				(*tp) = timePointNow;

				switch (dir)
				{
				case 0:
					scroller->SetScrollerLocationCapped(scroller->m_scrollerLocationY - amount);
					break;

				case 1:
					scroller->SetScrollerLocationCapped(scroller->m_scrollerLocationY + amount);
					break;

				default:
					break;
				}
			}
		}


	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			//gets render bounds
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
			SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
			SDL_Rect mousePos = SDL_Rect{ GetMousePos()[0], GetMousePos()[1], 1, 1 };

			//renders box fill
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorNorm.r, m_colorNorm.g, m_colorNorm.b, m_colorNorm.a);
			if (SDL_HasIntersection(&mousePos, &renderRect) == true) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorHover.r, m_colorHover.g, m_colorHover.b, m_colorHover.a); }
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &renderRect);


			//render separator
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorSeparator.r, m_colorSeparator.g, m_colorSeparator.b, m_colorSeparator.a);
			SDL_RenderDrawLine((SDL_Renderer*)renderingContext, renderRect.x, renderRect.y, renderRect.x, renderRect.y + (renderRect.h - 1));


			renderRect.x = renderRect.x + (m_imageSpacing + 1);
			renderRect.y = renderRect.y + (m_imageSpacing + 1);
			renderRect.w = renderRect.w - ((m_imageSpacing + 1) *2);
			renderRect.h = renderRect.h - ((m_imageSpacing + 1) *2);

			//render arrow
			int arrowVerticesLength = 3;
			SDL_Vertex arrowVertices[3] = {};

			arrowVertices[0].color = m_colorArrow;
			arrowVertices[1].color = m_colorArrow;
			arrowVertices[2].color = m_colorArrow;
			arrowVertices[0].tex_coord.x = 1;
			arrowVertices[1].tex_coord.x = 1;
			arrowVertices[2].tex_coord.x = 1;
			arrowVertices[0].tex_coord.y = 1;
			arrowVertices[1].tex_coord.y = 1;
			arrowVertices[2].tex_coord.y = 1;

			switch (m_scrollDirection)
			{
			case 0:
				arrowVertices[0].position.x = (float)renderRect.x;
				arrowVertices[0].position.y = (float)(renderRect.y + renderRect.h);
				arrowVertices[2].position.x = (float)(renderRect.x + renderRect.w);
				arrowVertices[2].position.y = (float)(renderRect.y + renderRect.h);

				arrowVertices[1].position.x = (float)(((float)(renderRect.x)) + (((float)(renderRect.w)) /2.0f));
				arrowVertices[1].position.y = (float)(renderRect.y);

				break;

			case 1:
				arrowVertices[0].position.x = (float)renderRect.x;
				arrowVertices[0].position.y = (float)(renderRect.y);
				arrowVertices[2].position.x = (float)(renderRect.x + renderRect.w);
				arrowVertices[2].position.y = (float)(renderRect.y);

				arrowVertices[1].position.x = (float)(((float)(renderRect.x)) + (((float)(renderRect.w)) / 2.0f));
				arrowVertices[1].position.y = (float)(renderRect.y + renderRect.h);
				break;

			default:
				break;
			}

			SDL_RenderGeometry((SDL_Renderer*)renderingContext, NULL, arrowVertices, arrowVerticesLength, NULL, NULL);

			if (m_inUseMode == true)
			{
				MinulateScrollerToMoveInDirection(m_scrollersName, m_scrollDirection, m_scrollAmount, &m_timeBetweenScrolls, UISHAR__SCROLLERBUTTON__SCROLLER_TICK_RATE);
			}
		}

		virtual bool OnEvent(unsigned int eventId, void* eventData) override
		{
			if (eventId == UISHAR__UICMD__SDL_EVENT && GetAttachedTree()->m_shouldRender == true) //is an event from sdl
			{
				SDL_Event* event = (SDL_Event*)eventData;

				stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
				SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
				SDL_Rect mousePos = SDL_Rect{ GetMousePos()[0], GetMousePos()[1], 1, 1 };

				if (event->type == SDL_MOUSEWHEEL && m_takeWheelInput == true && renderRect.x > 0 && renderRect.y > 0) //mouse wheel confing
				{
					if (event->wheel.y > 0 && m_scrollDirection == 0)
					{
						MinulateScrollerToMoveInDirection(m_scrollersName, m_scrollDirection, m_scrollAmount, &m_timeBetweenScrolls, 0);
						return true;
					}

					if (event->wheel.y < 0 && m_scrollDirection == 1)
					{
						MinulateScrollerToMoveInDirection(m_scrollersName, m_scrollDirection, m_scrollAmount, &m_timeBetweenScrolls, 0);
						return true;
					}
				}

				if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
				{
					if (SDL_HasIntersection(&mousePos, &renderRect) == true)
					{
						if (UIComponent_SDLScissorStack::IsEmpty() == false) //sissor check just to make sure
						{
							SDL_Rect checkRect = UIComponent_SDLScissorStack::Top();
							if (SDL_HasIntersection(&renderRect, &checkRect) == false) //oh no we cant handle the event
							{
								return false;
							}
						}

						m_inUseMode = true;
						return true;
					}
				}

				if (event->type == SDL_MOUSEBUTTONUP)
				{
					m_inUseMode = false;
				}
			}

			return false;
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  


			//scroll stuff
			m_scrollersName = std::string(asset.GetSubObjectsByScope(0, "<scrollersName>", "</scrollersName>").GetContents(), asset.GetSubObjectsByScope(0, "<scrollersName>", "</scrollersName>").GetContentsLength());
			m_scrollDirection = asset.GetSubObjectsByScope(0, "<scrollDirection>", "</scrollDirection>").GetContentsInt();
			m_scrollAmount = asset.GetSubObjectsByScope(0, "<scrollAmount>", "</scrollAmount>").GetContentsInt();
			m_takeWheelInput = asset.GetSubObjectsByScope(0, "<takeWheelInput>", "</takeWheelInput>").GetContentsBool();



			//line stuff
			m_imageSpacing = asset.GetSubObjectsByScope(0, "<imageSpacing>", "</imageSpacing>").GetContentsInt();


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
			stackExternal::assets::AssetObjectContainer colorArrayO = asset.GetSubObjectsByScope(0, "<colorSeparator>", "</colorSeparator>");
			m_colorSeparator.r = colorArrayO.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorSeparator.g = colorArrayO.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorSeparator.b = colorArrayO.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorSeparator.a = colorArrayO.GetSubObjectByArrayElement(3).GetContentsInt();

			//color stuff colorArrow
			stackExternal::assets::AssetObjectContainer colorArrayS = asset.GetSubObjectsByScope(0, "<colorArrow>", "</colorArrow>");
			m_colorArrow.r = colorArrayS.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorArrow.g = colorArrayS.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorArrow.b = colorArrayS.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorArrow.a = colorArrayS.GetSubObjectByArrayElement(3).GetContentsInt();
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			

			//scroll dir
			stackExternal::assets::AssetObjectContainer scrollDirection = asset.GetSubObjectsByScope(0, "<scrollDirection>", "</scrollDirection>");
			if (scrollDirection.IsContentsValid() == true)
			{
				m_scrollDirection = scrollDirection.GetContentsInt();
				return;
			}

			//spacing
			stackExternal::assets::AssetObjectContainer imageSpacing = asset.GetSubObjectsByScope(0, "<imageSpacing>", "</imageSpacing>");
			if (imageSpacing.IsContentsValid() == true)
			{
				m_imageSpacing = imageSpacing.GetContentsInt();
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
			stackExternal::assets::AssetObjectContainer colorArrayO = asset.GetSubObjectsByScope(0, "<colorSeparator>", "</colorSeparator>");
			if (colorArrayO.IsContentsValid() == true)
			{
				m_colorSeparator.r = colorArrayO.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorSeparator.g = colorArrayO.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorSeparator.b = colorArrayO.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorSeparator.a = colorArrayO.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}

			//set color arror
			stackExternal::assets::AssetObjectContainer colorArrayS = asset.GetSubObjectsByScope(0, "<colorArrow>", "</colorArrow>");
			if (colorArrayS.IsContentsValid() == true)
			{
				m_colorArrow.r = colorArrayS.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorArrow.g = colorArrayS.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorArrow.b = colorArrayS.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorArrow.a = colorArrayS.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}
		}

	public:
		std::string m_scrollersName = "";

		int m_scrollDirection = 0;
		int m_scrollAmount = 8;
		bool m_takeWheelInput = true;

		int m_imageSpacing = 4;

		SDL_Color m_colorHover = SDL_Color();
		SDL_Color m_colorNorm = SDL_Color();
		SDL_Color m_colorSeparator = SDL_Color();
		SDL_Color m_colorArrow = SDL_Color();
	};
}