#pragma once

#pragma once

#include <chrono>
#include <string>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

#include "MediaShared/SDLText/SDLText.h"

#include "../Elements/UIComponent_InputData.h"
#include "../Elements/UIComponent_ConnectionContext.h"

#include "scriptManagment.h"
#include "KeyboardEvents.h"

namespace UIShared
{
	class SIMS_CC_ID_SearchBox : public stackExternal::UI::UIElement, UIComponent_ConnectionContext, UIComponent_InputData
	{
	private:
		std::chrono::steady_clock::time_point m_baseTimerPoint = std::chrono::steady_clock::now();

		mediaShared::media::SDLText m_baseText = mediaShared::media::SDLText();
		bool m_inBlankMode = true;
		bool m_isRenderInBlankMode = false;


		std::string m_lastFramesInput = "";
		std::string m_currentInput = "";

		bool m_inInputMode = false;
		bool m_textInputBlinkerState = true;

		void Enter()
		{
			if (m_currentInput == "") //no text go back to the blank
			{
				m_inBlankMode = true;
			}

			m_inInputMode = false;

			if (m_currentInput == "")
			{
				sfra::scrman::globals::n_searchResults = sfra::scrman::globals::n_masterScriptSet;
				return;
			}

			sfra::scrman::globals::n_searchResults = sfra::scrman::GetFilterdResultsFromMasterSet(m_currentInput);
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


			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorText.r, m_colorText.g, m_colorText.b, m_colorText.a);
			if (m_inBlankMode == true) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_blankTextColor.r, m_blankTextColor.g, m_blankTextColor.b, m_blankTextColor.a); }
			if (m_inBlankMode == true && m_isRenderInBlankMode == false) { m_baseText.SetText((SDL_Renderer*)renderingContext, m_blankText); m_lastFramesInput = ""; m_isRenderInBlankMode = true; }

			//text rendering
			std::string currentOnScreenText = m_currentInput;

			if (m_inInputMode == true)
			{
				if (m_textInputBlinkerState == true)
				{
					currentOnScreenText = currentOnScreenText + "_";
				}
				else
				{
					currentOnScreenText = currentOnScreenText + "  ";
				}
			}

			//making sure we arnt rerendering the text pointlessly
			if (m_lastFramesInput != currentOnScreenText && m_inBlankMode == false)
			{
				m_lastFramesInput = currentOnScreenText;
				m_baseText.SetText((SDL_Renderer*)renderingContext, currentOnScreenText);
			}

			//actual text rendering
			m_baseText.SetPos({ renderRect.x,renderRect.y });
			m_baseText.SetSize({ renderRect.w, renderRect.h });
			m_baseText.RenderText((SDL_Renderer*)renderingContext);


			//makes the cursor blink
			std::chrono::steady_clock::time_point timePointNow = std::chrono::steady_clock::now();
			if (std::chrono::duration<double>{timePointNow - m_baseTimerPoint}.count() > 0.5)
			{
				m_baseTimerPoint = timePointNow;
				m_textInputBlinkerState = !m_textInputBlinkerState;
			}
		}

		virtual bool OnEvent(unsigned int eventId, void* eventData) override
		{
			if (eventId == UISHAR__UICMD__SDL_EVENT && GetAttachedTree()->m_shouldRender == true) //is an event from sdl
			{
				SDL_Event* event = (SDL_Event*)eventData;

				if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
				{
					if (m_inInputMode == true)
					{
						Enter();
						return true;
					}

					stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
					SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
					SDL_Rect mousePos = SDL_Rect{ GetMousePos()[0], GetMousePos()[1], 1, 1 };

					if (SDL_HasIntersection(&mousePos, &renderRect) == true) //go to input mode
					{
						m_lastFramesInput = "!!!<<<>>>!!!";
						m_isRenderInBlankMode = false;
						m_inBlankMode = false;
						m_inInputMode = true;
						return true;
					}
				}
			}

			if (eventId == MOD_KEY_DOWN && m_inInputMode == true)
			{
				unsigned int letter = *((unsigned int*)eventData);

				if (letter > 31 && letter < 255)
				{
					if (GetIsCapsDown() == true)
					{
						letter = (unsigned int)toupper((int)letter);
					}

					if (GetIsShiftDown() == true)
					{
						switch (letter) //boi this be LOOOOOOOOOOOOOOOOOOOONG LIKE MY BF MASSIVE CO**
						{
						case '1':
							letter = '!';
							break;
						case '2':
							letter = '"';
							break;
						case '3':
							letter = '£';
							break;
						case '4':
							letter = '$';
							break;
						case '5':
							letter = '%';
							break;
						case '6':
							letter = '^';
							break;
						case '7':
							letter = '&';
							break;
						case '8':
							letter = '*';
							break;
						case '9':
							letter = '(';
							break;
						case '0':
							letter = ')';
							break;
						case '-':
							letter = '_';
							break;
						case '=':
							letter = '+';
							break;
						case '[':
							letter = '{';
							break;
						case ']':
							letter = '}';
							break;
						case ';':
							letter = ':';
							break;
						case '\'':
							letter = '@';
							break;
						case '#':
							letter = '~';
							break;
						case ',':
							letter = '<';
							break;
						case '.':
							letter = '>';
							break;
						case '/':
							letter = '?';
							break;
						case '\\':
							letter = '|';
							break;

						default:
							break;
						}
					}

					m_currentInput = m_currentInput + ((char)(letter));
				}
				else if (letter == 8 && m_currentInput.length() >= 1)
				{
					m_currentInput = m_currentInput.substr(0, m_currentInput.length() - 1);
				}
				else if (letter == 13)
				{
					Enter();
				}

				return true;
			}

			return false;
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
		
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());


			//font
			int fontSize = asset.GetSubObjectsByScope(0, "<textSize>", "</textSize>").GetContentsInt();
			std::string font = std::string(asset.GetSubObjectsByScope(0, "<font>", "</font>").GetContents(), asset.GetSubObjectsByScope(0, "<font>", "</font>").GetContentsLength());

			char* fileData = nullptr;
			unsigned int fileSize = 0;
			GetItemFromPack(font, &fileData, &fileSize);

			if (fileSize > 0)
			{
				m_baseText.SetFontPack(font, fileData, fileSize, fontSize);
			}


			//text location
			m_baseText.SetHorizontalAlignment(mediaShared::media::SDLText_HorizontalLocation::HScrollLeft);
			m_baseText.SetVerticalAlignment(mediaShared::media::SDLText_VerticalLocation::VCenter);


			//line stuff
			m_lineThickness = asset.GetSubObjectsByScope(0, "<lineThickness>", "</lineThickness>").GetContentsInt();

			//blank text
			m_blankText = std::string(asset.GetSubObjectsByScope(0, "<blankText>", "</blankText>").GetContents(), asset.GetSubObjectsByScope(0, "<blankText>", "</blankText>").GetContentsLength());

			//color stuff blank text color
			stackExternal::assets::AssetObjectContainer colorArrayB = asset.GetSubObjectsByScope(0, "<blankTextColor>", "</blankTextColor>");
			m_blankTextColor.r = colorArrayB.GetSubObjectByArrayElement(0).GetContentsInt();
			m_blankTextColor.g = colorArrayB.GetSubObjectByArrayElement(1).GetContentsInt();
			m_blankTextColor.b = colorArrayB.GetSubObjectByArrayElement(2).GetContentsInt();
			m_blankTextColor.a = colorArrayB.GetSubObjectByArrayElement(3).GetContentsInt();

			//color stuff colorHover
			stackExternal::assets::AssetObjectContainer colorArrayT = asset.GetSubObjectsByScope(0, "<colorText>", "</colorText>");
			m_colorText.r = colorArrayT.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorText.g = colorArrayT.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorText.b = colorArrayT.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorText.a = colorArrayT.GetSubObjectByArrayElement(3).GetContentsInt();


			//color stuff colorNorm
			stackExternal::assets::AssetObjectContainer colorArrayN = asset.GetSubObjectsByScope(0, "<colorNorm>", "</colorNorm>");
			m_colorNorm.r = colorArrayN.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorNorm.g = colorArrayN.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorNorm.b = colorArrayN.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorNorm.a = colorArrayN.GetSubObjectByArrayElement(3).GetContentsInt();

			//color stuff colorOuline
			stackExternal::assets::AssetObjectContainer colorArrayO = asset.GetSubObjectsByScope(0, "<colorOut>", "</colorOut>");
			m_colorOutline.r = colorArrayO.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorOutline.g = colorArrayO.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorOutline.b = colorArrayO.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorOutline.a = colorArrayO.GetSubObjectByArrayElement(3).GetContentsInt();
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

			//blank text
			stackExternal::assets::AssetObjectContainer blankText = asset.GetSubObjectsByScope(0, "<blankText>", "</blankText>");
			if (blankText.IsContentsValid() == true)
			{
				m_blankText = std::string(blankText.GetContents(), blankText.GetContentsLength());
			}

			//color stuff blank text color
			stackExternal::assets::AssetObjectContainer colorArrayB = asset.GetSubObjectsByScope(0, "<blankTextColor>", "</blankTextColor>");
			if (colorArrayB.IsContentsValid() == true)
			{
				m_blankTextColor.r = colorArrayB.GetSubObjectByArrayElement(0).GetContentsInt();
				m_blankTextColor.g = colorArrayB.GetSubObjectByArrayElement(1).GetContentsInt();
				m_blankTextColor.b = colorArrayB.GetSubObjectByArrayElement(2).GetContentsInt();
				m_blankTextColor.a = colorArrayB.GetSubObjectByArrayElement(3).GetContentsInt();
			}


			//set color hover
			stackExternal::assets::AssetObjectContainer colorArrayT = asset.GetSubObjectsByScope(0, "<colorText>", "</colorText>");
			if (colorArrayT.IsContentsValid() == true)
			{
				m_colorText.r = colorArrayT.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorText.g = colorArrayT.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorText.b = colorArrayT.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorText.a = colorArrayT.GetSubObjectByArrayElement(3).GetContentsInt();
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
			stackExternal::assets::AssetObjectContainer colorArrayO = asset.GetSubObjectsByScope(0, "<colorOut>", "</colorOut>");
			if (colorArrayO.IsContentsValid() == true)
			{
				m_colorOutline.r = colorArrayO.GetSubObjectByArrayElement(0).GetContentsInt();
				m_colorOutline.g = colorArrayO.GetSubObjectByArrayElement(1).GetContentsInt();
				m_colorOutline.b = colorArrayO.GetSubObjectByArrayElement(2).GetContentsInt();
				m_colorOutline.a = colorArrayO.GetSubObjectByArrayElement(3).GetContentsInt();
				return;
			}
		}

	public:
		SDL_Color m_colorText = SDL_Color();
		SDL_Color m_colorNorm = SDL_Color();
		SDL_Color m_colorOutline = SDL_Color();
		SDL_Color m_blankTextColor = SDL_Color();

		std::string m_blankText = "";

		int m_lineThickness = 2;
	};
}