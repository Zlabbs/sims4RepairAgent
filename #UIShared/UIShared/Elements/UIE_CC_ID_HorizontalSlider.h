#pragma once

#pragma once

#include <string>
#include <math.h>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"
#include "StandardShared/OtherMaths/OtherMaths.h"

#include "UIComponent_ConnectionContext.h"
#include "UIComponent_InputData.h"

namespace UIShared
{
	class UIE_CC_ID_HorizontalSlider : public stackExternal::UI::UIElement, UIComponent_ConnectionContext, UIComponent_InputData
	{
	private:
		float m_draggerLocationPercent = 0;;
		int m_draggerLocationAsAOffset = 0;
		
		int m_dragModeOffsetX = 0;
		bool m_inDragMode = false;

	protected:

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			//get mouse pos
			std::array<int, 2> mousePosRaw = GetMousePos();
			SDL_Rect mousePos = SDL_Rect{ mousePosRaw[0], mousePosRaw[1], 1, 1 };


			//get bounds
			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();


			//calulate bar location
			int barHeight = (int)(((float)bounds.offsetSize[1]) * m_barThickPercent);
			int barPosY = (int)((((float)(bounds.offsetSize[1])) / 2.0f) - (((float)(barHeight)) / 2.0f));

			SDL_Rect barRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1] + barPosY, bounds.offsetSize[0], barHeight };


			//calulate dragger location
			int draggerHeight = (int)(((float)bounds.offsetSize[1]) * m_draggerThickPercent);
			int draggerWidth = (int)(((float)bounds.offsetSize[0]) * m_draggerLengthPercent);
			int draggerPosY = (int)((((float)(bounds.offsetSize[1])) / 2.0f) - (((float)(draggerHeight)) / 2.0f));
			int draggerPosX = m_draggerLocationAsAOffset;

			SDL_Rect draggerRect = SDL_Rect{ bounds.offsetPos[0] + draggerPosX, bounds.offsetPos[1] + draggerPosY, draggerWidth, draggerHeight };


			//rendering
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorBar.r, m_colorBar.g, m_colorBar.b, m_colorBar.a);
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &barRect);

			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorNorm.r, m_colorNorm.g, m_colorNorm.b, m_colorNorm.a);
			if (SDL_HasIntersection(&mousePos, &draggerRect) == true || m_inDragMode == true) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorHover.r, m_colorHover.g, m_colorHover.b, m_colorHover.a); }
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &draggerRect);


			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorOutline.r, m_colorOutline.g, m_colorOutline.b, m_colorOutline.a);
			for (int i = 0; i < m_lineThickness; i++)
			{
				SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &draggerRect);
				draggerRect.x = draggerRect.x + 1;
				draggerRect.y = draggerRect.y + 1;
				draggerRect.w = draggerRect.w - 2;
				draggerRect.h = draggerRect.h - 2;
			}


			//handles dragging motion
			if (m_inDragMode == true)
			{
				int barSpaceValue = std::min(std::max((GetMousePos()[0] + m_dragModeOffsetX) - bounds.offsetPos[0], 0), bounds.offsetSize[0] - draggerWidth);
				m_draggerLocationPercent = standardShared::otherMaths::Remap(barSpaceValue, 0.0f, bounds.offsetSize[0] - draggerWidth, 0.0f, 1.0f);

				//std::cout << barSpaceValue << "," << m_draggerLocationPercent << "\n";
			}

			m_draggerLocationAsAOffset = (bounds.offsetSize[0] - draggerWidth) * m_draggerLocationPercent;

			if (renderDebug == false) { return; }
			SDL_Rect rectDebug = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, 255, 0, 255, 255);
			SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &rectDebug);
		}

		virtual bool OnEvent(unsigned int eventId, void* eventData) override
		{
			if (eventId == UISHAR__UICMD__RESET)
			{
				m_inDragMode = false;

				try
				{
					m_draggerLocationPercent = stof(GetConVar(m_conVarName));
				}
				catch (...)
				{
					m_draggerLocationPercent = 0;
				}
			}


			if (eventId == UISHAR__UICMD__SDL_EVENT && GetAttachedTree()->m_shouldRender == true) //is an event from sdl
			{
				SDL_Event* event = (SDL_Event*)eventData;


				//get mouse location
				std::array<int, 2> mousePosRaw = GetMousePos();
				SDL_Rect mousePos = SDL_Rect{ mousePosRaw[0], mousePosRaw[1], 1, 1 };


				//get dragger rect
				stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();

				int draggerHeight = (int)(((float)bounds.offsetSize[1]) * m_draggerThickPercent);
				int draggerWidth = (int)(((float)bounds.offsetSize[0]) * m_draggerLengthPercent);
				int draggerPosY = (int)((((float)(bounds.offsetSize[1])) / 2.0f) - (((float)(draggerHeight)) / 2.0f));
				int draggerPosX = m_draggerLocationAsAOffset;

				SDL_Rect draggerRect = SDL_Rect{ bounds.offsetPos[0] + draggerPosX, bounds.offsetPos[1] + draggerPosY, draggerWidth, draggerHeight }; //get the location of where the drgger part of the slider is


				//dragger controls
				if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
				{
					if (SDL_HasIntersection(&mousePos, &draggerRect) == true) //you are dragger the dragger
					{
						if (UIComponent_SDLScissorStack::IsEmpty() == false) //sissor check just to make sure
						{
							SDL_Rect checkRect = UIComponent_SDLScissorStack::Top();
							if (SDL_HasIntersection(&draggerRect, &checkRect) == false) //oh no we cant handle the event
							{
								return false;
							}
						}

						m_dragModeOffsetX = (bounds.offsetPos[0] + m_draggerLocationAsAOffset) - mousePos.x;
						m_inDragMode = true;
						return true;
					}
				}

				if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT)
				{
					m_inDragMode = false;
					SetConVar(m_conVarName, std::to_string(m_draggerLocationPercent));
				}
			}

			return false;
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset stuff
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());
			

			//convar stuff
			m_conVarName = std::string(asset.GetSubObjectsByScope(0, "<conVarName>", "</conVarName>").GetContents(), asset.GetSubObjectsByScope(0, "<conVarName>", "</conVarName>").GetContentsLength());

			try
			{
				m_draggerLocationPercent = stof(GetConVar(m_conVarName));
			}
			catch (...)
			{
				m_draggerLocationPercent = 0;
			}


			//shape stuff
			m_barThickPercent = asset.GetSubObjectsByScope(0, "<barThickPercent>", "</barThickPercent>").GetContentsFloat();
			m_draggerThickPercent = asset.GetSubObjectsByScope(0, "<draggerThickPercent>", "</draggerThickPercent>").GetContentsFloat();
			m_draggerLengthPercent = asset.GetSubObjectsByScope(0, "<draggerLengthPercent>", "</draggerLengthPercent>").GetContentsFloat();


			//line thickness
			m_lineThickness = asset.GetSubObjectsByScope(0, "<lineThickness>", "</lineThickness>").GetContentsInt();


			//color stuff
			stackExternal::assets::AssetObjectContainer colorArrayO = asset.GetSubObjectsByScope(0, "<colorOutline>", "</colorOutline>");
			m_colorOutline.r = colorArrayO.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorOutline.g = colorArrayO.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorOutline.b = colorArrayO.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorOutline.a = colorArrayO.GetSubObjectByArrayElement(3).GetContentsInt();

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
			stackExternal::assets::AssetObjectContainer colorArrayB = asset.GetSubObjectsByScope(0, "<colorBar>", "</colorBar>");
			m_colorBar.r = colorArrayO.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorBar.g = colorArrayO.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorBar.b = colorArrayO.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorBar.a = colorArrayO.GetSubObjectByArrayElement(3).GetContentsInt();

		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);


			//convar
			stackExternal::assets::AssetObjectContainer conVarName = asset.GetSubObjectsByScope(0, "<conVarName>", "</conVarName>");
			if (conVarName.IsContentsValid() == true)
			{
				m_conVarName = std::string(conVarName.GetContents(), conVarName.GetContentsLength());

				try
				{
					m_draggerLocationPercent = stof(GetConVar(m_conVarName));
				}
				catch (...)
				{
					m_draggerLocationPercent = 0;
				}

				return;
			}


			//shape stuff
			stackExternal::assets::AssetObjectContainer lineThickness = asset.GetSubObjectsByScope(0, "<lineThickness>", "</lineThickness>");
			if (conVarName.IsContentsValid() == true)
			{
				m_lineThickness = lineThickness.GetContentsInt();
				return;
			}

			stackExternal::assets::AssetObjectContainer draggerThickPercent = asset.GetSubObjectsByScope(0, "<draggerThickPercent>", "</draggerThickPercent>");
			if (conVarName.IsContentsValid() == true)
			{
				m_draggerThickPercent = draggerThickPercent.GetContentsFloat();
				return;
			}

			stackExternal::assets::AssetObjectContainer draggerLengthPercent = asset.GetSubObjectsByScope(0, "<draggerLengthPercent>", "</draggerLengthPercent>");
			if (conVarName.IsContentsValid() == true)
			{
				m_draggerLengthPercent = draggerLengthPercent.GetContentsFloat();
				return;
			}

			stackExternal::assets::AssetObjectContainer barThickPercent = asset.GetSubObjectsByScope(0, "<barThickPercent>", "</barThickPercent>");
			if (conVarName.IsContentsValid() == true)
			{
				m_barThickPercent = barThickPercent.GetContentsFloat();
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
			stackExternal::assets::AssetObjectContainer colorArrayB = asset.GetSubObjectsByScope(0, "<colorBar>", "</colorBar>");
			if (colorArrayB.IsContentsValid() == true)
			{
				m_colorBar.r = colorArrayB.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorBar.g = colorArrayB.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorBar.b = colorArrayB.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorBar.a = colorArrayB.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}
		}

	public:
		std::string m_conVarName = "";
		int m_lineThickness = 1;

		float m_barThickPercent = 0.75f;
		float m_draggerThickPercent = 1.0f;
		float m_draggerLengthPercent = 0.02f;

		SDL_Color m_colorOutline = SDL_Color();
		SDL_Color m_colorHover = SDL_Color();
		SDL_Color m_colorNorm = SDL_Color();
		SDL_Color m_colorBar = SDL_Color();
	};
}