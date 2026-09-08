#pragma once

#include <string>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"
#include "MediaShared/SDLImage/SDLImage.h"

#include "UIComponent_InputData.h"
#include "UIComponent_ConnectionContext.h"
#include "UIComponent_SDLScissorStack.h"

namespace UIShared
{
	class UIE_CC_ID_CheckBox : public stackExternal::UI::UIElement, UIComponent_ConnectionContext, UIComponent_InputData
	{
	private:
		bool m_inCheck = false;

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
			if (m_inCheck == true) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorSelect.r, m_colorSelect.g, m_colorSelect.b, m_colorSelect.a); }
			if (SDL_HasIntersection(&mousePos, &renderRect) == true) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorHover.r, m_colorHover.g, m_colorHover.b, m_colorHover.a); }
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &renderRect);


			//render box outline
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorOutline.r, m_colorOutline.g, m_colorOutline.b, m_colorOutline.a);
			for (int i = 0; i < m_lineThickness; i++)
			{
				SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &renderRect);
				renderRect.x = renderRect.x + 1;
				renderRect.y = renderRect.y + 1;
				renderRect.w = renderRect.w - 2;
				renderRect.h = renderRect.h - 2;
			}

			//reder check mark
			if (m_inCheck == true)
			{
				renderRect.x = renderRect.x + m_imageSpacing;
				renderRect.y = renderRect.y + m_imageSpacing;
				renderRect.w = renderRect.w - (m_imageSpacing *2);
				renderRect.h = renderRect.h - (m_imageSpacing *2);

				SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorImgBg.r, m_colorImgBg.g, m_colorImgBg.b, m_colorImgBg.a);
				SDL_RenderFillRect((SDL_Renderer*)renderingContext, &renderRect);

				m_imageBaseObject.SetPos({ renderRect.x, renderRect.y });
				m_imageBaseObject.SetSize({ renderRect.w, renderRect.h });
				m_imageBaseObject.RenderImage((SDL_Renderer*)renderingContext);
			}
		}

		virtual bool OnEvent(unsigned int eventId, void* eventData) override
		{
			if(eventId == UISHAR__UICMD__RESET)
			{
				try
				{
					m_inCheck = ((bool)stoi(GetConVar(m_conVarName)));
				}
				catch (...)
				{
					m_inCheck = false;
				}
			}

			if (eventId == UISHAR__UICMD__SDL_EVENT && GetAttachedTree()->m_shouldRender == true) //is an event from sdl
			{
				SDL_Event* event = (SDL_Event*)eventData;

				if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
				{
					stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
					SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
					SDL_Rect mousePos = SDL_Rect{ GetMousePos()[0], GetMousePos()[1], 1, 1 };

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

						m_inCheck = !m_inCheck;
						SetConVar(m_conVarName, std::to_string(m_inCheck));
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


			//convar stuff
			m_conVarName = std::string(asset.GetSubObjectsByScope(0, "<conVarName>", "</conVarName>").GetContents(), asset.GetSubObjectsByScope(0, "<conVarName>", "</conVarName>").GetContentsLength());

			try
			{
				m_inCheck = ((bool)stoi(GetConVar(m_conVarName)));
			}
			catch (...)
			{
				m_inCheck = false;
			}


			//image stuff
			std::string imagePath = std::string(asset.GetSubObjectsByScope(0, "<imagePath>", "</imagePath>").GetContents(), asset.GetSubObjectsByScope(0, "<imagePath>", "</imagePath>").GetContentsLength());
			if (IsItemInPack(imagePath) == false)
			{
				m_imageBaseObject.SetTexture((SDL_Renderer*)propertyContext, imagePath);
			}
			else
			{
				char* fileData = nullptr;
				unsigned int fileSize = 0;
				GetItemFromPack(imagePath, &fileData, &fileSize);

				if (fileSize > 0)
				{
					m_imageBaseObject.SetTexturePack((SDL_Renderer*)propertyContext, imagePath, fileData, fileSize);
				}
			}


			//line stuff
			m_lineThickness = asset.GetSubObjectsByScope(0, "<lineThickness>", "</lineThickness>").GetContentsInt();
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
			stackExternal::assets::AssetObjectContainer colorArrayO = asset.GetSubObjectsByScope(0, "<colorOutline>", "</colorOutline>");
			m_colorOutline.r = colorArrayO.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorOutline.g = colorArrayO.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorOutline.b = colorArrayO.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorOutline.a = colorArrayO.GetSubObjectByArrayElement(3).GetContentsInt();

			//color stuff colorOuline
			stackExternal::assets::AssetObjectContainer colorArrayS = asset.GetSubObjectsByScope(0, "<colorSelect>", "</colorSelect>");
			m_colorSelect.r = colorArrayS.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorSelect.g = colorArrayS.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorSelect.b = colorArrayS.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorSelect.a = colorArrayS.GetSubObjectByArrayElement(3).GetContentsInt();

			//color stuff colorOuline
			stackExternal::assets::AssetObjectContainer colorArrayI = asset.GetSubObjectsByScope(0, "<colorImgBg>", "</colorImgBg>");
			m_colorImgBg.r = colorArrayI.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorImgBg.g = colorArrayI.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorImgBg.b = colorArrayI.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorImgBg.a = colorArrayI.GetSubObjectByArrayElement(3).GetContentsInt();
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);


			stackExternal::assets::AssetObjectContainer conVarName = asset.GetSubObjectsByScope(0, "<conVarName>", "</conVarName>");
			if (conVarName.IsContentsValid() == true)
			{
				m_conVarName = std::string(conVarName.GetContents(), conVarName.GetContentsLength());
				
				try
				{
					m_inCheck = ((bool)stoi(GetConVar(m_conVarName)));
				}
				catch (...)
				{
					m_inCheck = false;
				}

				return;
			}


			//image stuff
			if (asset.GetSubObjectsByScope(0, "<imagePath>", "</imagePath>").IsContentsValid() == true)
			{
				std::string imagePath = std::string(asset.GetSubObjectsByScope(0, "<imagePath>", "</imagePath>").GetContents(), asset.GetSubObjectsByScope(0, "<imagePath>", "</imagePath>").GetContentsLength());

				if (IsItemInPack(imagePath) == false)
				{
					m_imageBaseObject.SetTexture((SDL_Renderer*)propertyContext, imagePath);
				}
				else
				{
					char* fileData = nullptr;
					unsigned int fileSize = 0;
					GetItemFromPack(imagePath, &fileData, &fileSize);

					if (fileSize > 0)
					{
						m_imageBaseObject.SetTexturePack((SDL_Renderer*)propertyContext, imagePath, fileData, fileSize);
					}
				}
				return;
			}


			//line stuff
			stackExternal::assets::AssetObjectContainer lineThickness = asset.GetSubObjectsByScope(0, "<lineThickness>", "</lineThickness>");
			if (lineThickness.IsContentsValid() == true)
			{
				m_lineThickness = lineThickness.GetContentsInt();
				return;
			}

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
			stackExternal::assets::AssetObjectContainer colorArrayO = asset.GetSubObjectsByScope(0, "<colorOutline>", "</colorOutline>");
			if (colorArrayO.IsContentsValid() == true)
			{
				m_colorOutline.r = colorArrayO.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorOutline.g = colorArrayO.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorOutline.b = colorArrayO.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorOutline.a = colorArrayO.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}

			//set color select
			stackExternal::assets::AssetObjectContainer colorArrayS = asset.GetSubObjectsByScope(0, "<colorSelect>", "</colorSelect>");
			if (colorArrayS.IsContentsValid() == true)
			{
				m_colorSelect.r = colorArrayS.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorSelect.g = colorArrayS.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorSelect.b = colorArrayS.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorSelect.a = colorArrayS.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}

			//set color img
			stackExternal::assets::AssetObjectContainer colorArrayI = asset.GetSubObjectsByScope(0, "<colorImgBg>", "</colorImgBg>");
			if (colorArrayI.IsContentsValid() == true)
			{
				m_colorImgBg.r = colorArrayI.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorImgBg.g = colorArrayI.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorImgBg.b = colorArrayI.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorImgBg.a = colorArrayI.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}
		}

	public:
		std::string m_conVarName = "";

		int m_lineThickness = 1;
		int m_imageSpacing = 4;

		mediaShared::media::SDLImage m_imageBaseObject = mediaShared::media::SDLImage();

		SDL_Color m_colorHover = SDL_Color();
		SDL_Color m_colorNorm = SDL_Color();
		SDL_Color m_colorOutline = SDL_Color();
		SDL_Color m_colorSelect = SDL_Color();
		SDL_Color m_colorImgBg = SDL_Color();
	};
}