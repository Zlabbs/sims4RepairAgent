#pragma once

#include <string>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"
#include "MediaShared/SDLText/SDLText.h"

#include "UIE_SceneAttachedRoot.h"

#include "UIComponent_InputData.h"
#include "UIComponent_ConnectionContext.h"
#include "UIComponent_SDLScissorStack.h"

#define UISHAR__KEYBUTTON_BLANK_KEY 0 //all is ok but there is an action unbinded
#define UISHAR__KEYBUTTON_UNKNOWN_KEY ((unsigned int)(-1)) //something went horribly wong
#define UISHAR__KEYBUTTON_UNKNOWN_GROUP ((unsigned int)(-1))
#define UISHAR__KEYBUTTON_CLAIMOWNERSHIP 12123

namespace UIShared
{
	class UIE_CC_ID_KeyButton;

	struct UIE_CC_ID_KeyButtonOwnershipClaimStruct
	{
		unsigned int keyBeingOwned = 0;
		unsigned int keyOwnGroup = 0;
		UIE_CC_ID_KeyButton* newOwner = nullptr;
	};

	class UIE_CC_ID_KeyButton : public stackExternal::UI::UIElement, UIComponent_ConnectionContext, UIComponent_InputData
	{
	private:
		unsigned int m_myCurrentKeyCode = 0;
		bool m_needsToRerenderText = true;
		bool m_waitingForKeyInput = false;

	protected:
		void SetConVarKeyOwnership(unsigned int value)
		{
			if (value != UISHAR__KEYBUTTON_BLANK_KEY && value != UISHAR__KEYBUTTON_UNKNOWN_KEY)
			{
				UIE_CC_ID_KeyButtonOwnershipClaimStruct ownerClaim = UIE_CC_ID_KeyButtonOwnershipClaimStruct();
				ownerClaim.keyBeingOwned = value;
				ownerClaim.keyOwnGroup = m_claimOwnershipGroupId;
				ownerClaim.newOwner = this;

				GetAttachedTree()->PassEventAsListUnhandleable(UISHAR__KEYBUTTON_CLAIMOWNERSHIP, &ownerClaim);
			}

			SetConVar(m_conVarName, std::to_string(value));

			m_needsToRerenderText = true;

			try
			{
				m_myCurrentKeyCode = ((unsigned int)std::stoi(GetConVar(m_conVarName)));
			}
			catch (...)
			{
				m_myCurrentKeyCode = UISHAR__KEYBUTTON_UNKNOWN_KEY;
			}
		}

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
			if (m_waitingForKeyInput == true) { SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorNormSel.r, m_colorNormSel.g, m_colorNormSel.b, m_colorNormSel.a); }
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &renderRect);


			//renders text
			if (m_needsToRerenderText == true)
			{
				std::string textAsVisible = "";

				switch (m_myCurrentKeyCode)
				{
				case ((unsigned int)SDL_KeyCode::SDLK_LSHIFT):
					textAsVisible = "LShift";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_RSHIFT):
					textAsVisible = "RShift";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_LALT):
					textAsVisible = "LAlt";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_RALT):
					textAsVisible = "RAlt";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_LCTRL):
					textAsVisible = "LCtrl";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_RCTRL):
					textAsVisible = "RCtrl";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_BACKSPACE):
					textAsVisible = "BackSpace";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_RETURN):
					textAsVisible = "Enter";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_SPACE):
					textAsVisible = "Space";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_INSERT):
					textAsVisible = "INS";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_HOME):
					textAsVisible = "HOME";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_PAGEUP):
					textAsVisible = "P_UP";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_PAGEDOWN):
					textAsVisible = "P_DOWN";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_DELETE):
					textAsVisible = "DEL";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_PRINTSCREEN):
					textAsVisible = "PrintScr";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_SCROLLLOCK):
					textAsVisible = "SLock";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_PAUSE):
					textAsVisible = "pause";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_UP):
					textAsVisible = "Up";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_DOWN):
					textAsVisible = "Down";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_LEFT):
					textAsVisible = "Left";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_RIGHT):
					textAsVisible = "Right";
					break;


				case ((unsigned int)SDL_KeyCode::SDLK_END):
					textAsVisible = "END";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_F1):
					textAsVisible = "FUNC_1";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_F2):
					textAsVisible = "FUNC_2";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_F3):
					textAsVisible = "FUNC_3";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_F4):
					textAsVisible = "FUNC_4";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_F5):
					textAsVisible = "FUNC_5";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_F6):
					textAsVisible = "FUNC_6";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_F7):
					textAsVisible = "FUNC_7";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_F8):
					textAsVisible = "FUNC_8";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_F9):
					textAsVisible = "FUNC_9";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_F10):
					textAsVisible = "FUNC_10";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_F11):
					textAsVisible = "FUNC_11";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_F12):
					textAsVisible = "FUNC_12";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_NUMLOCKCLEAR):
					textAsVisible = "KP_NUMLOCK";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_DIVIDE):
					textAsVisible = "KP_DIV";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_MULTIPLY):
					textAsVisible = "KP_MUL";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_MINUS):
					textAsVisible = "KP_SUB";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_PLUS):
					textAsVisible = "KP_ADD";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_ENTER):
					textAsVisible = "KP_ENTER";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_PERIOD):
					textAsVisible = "KP_FULLSTOP";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_0):
					textAsVisible = "KP_0";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_1):
					textAsVisible = "KP_1";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_2):
					textAsVisible = "KP_2";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_3):
					textAsVisible = "KP_3";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_4):
					textAsVisible = "KP_4";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_5):
					textAsVisible = "KP_5";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_6):
					textAsVisible = "KP_6";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_7):
					textAsVisible = "KP_7";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_8):
					textAsVisible = "KP_8";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_KP_9):
					textAsVisible = "KP_9";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_CAPSLOCK):
					textAsVisible = "CLock";
					break;

				case ((unsigned int)SDL_KeyCode::SDLK_TAB):
					textAsVisible = "Tab";
					break;

				case UISHAR__KEYBUTTON_BLANK_KEY:
					textAsVisible = " "; //not set
					break;

				case UISHAR__KEYBUTTON_UNKNOWN_KEY:
					textAsVisible = "???|1"; //unknown text from convar failure
					break;

				default:
					textAsVisible += (char)m_myCurrentKeyCode;
					if (m_myCurrentKeyCode < 31 || m_myCurrentKeyCode > 255) { textAsVisible = "???|2"; } //unknow text from printing failure
					if (m_myCurrentKeyCode >= UISHAR__UICMD__MOUSETOKEYINDEX && m_myCurrentKeyCode <= UISHAR__UICMD__MOUSETOKEYINDEX_MAX) { textAsVisible = "Mouse " + std::to_string(m_myCurrentKeyCode - UISHAR__UICMD__MOUSETOKEYINDEX); }
					break;
				}

				m_textBaseObject.SetText((SDL_Renderer*)renderingContext, textAsVisible);
				m_needsToRerenderText = false;
			}


			//actually renders text
			if (m_waitingForKeyInput == false)
			{
				m_textBaseObject.SetPos({ bounds.offsetPos[0], bounds.offsetPos[1] });
				m_textBaseObject.SetSize({ bounds.offsetSize[0], bounds.offsetSize[1] });
				SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, m_colorText.r, m_colorText.g, m_colorText.b, m_colorText.a);
				m_textBaseObject.RenderText((SDL_Renderer*)renderingContext);
			}

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
		}

		virtual bool OnEvent(unsigned int eventId, void* eventData) override
		{
			if (eventId == UISHAR__UICMD__RESET)
			{
				m_waitingForKeyInput = false;
			}


			if (eventId == UISHAR__KEYBUTTON_CLAIMOWNERSHIP) //to stop other key buttons having the same key as us
			{
				UIE_CC_ID_KeyButtonOwnershipClaimStruct* event = (UIE_CC_ID_KeyButtonOwnershipClaimStruct*)eventData;
				
				if (event->keyBeingOwned != UISHAR__KEYBUTTON_UNKNOWN_KEY && event->keyBeingOwned != UISHAR__KEYBUTTON_BLANK_KEY && event->keyOwnGroup != UISHAR__KEYBUTTON_UNKNOWN_GROUP && event->newOwner != this) //the key they own is valid and it isnt us
				{
					if (m_myCurrentKeyCode == event->keyBeingOwned && event->keyOwnGroup == m_claimOwnershipGroupId) //the key they own is the same as us and there now allowed to share that key with us
					{
						SetConVarKeyOwnership(UISHAR__KEYBUTTON_BLANK_KEY); //blank out there key
					}
				}
			}


			if (eventId == UISHAR__UICMD__SDL_EVENT && GetAttachedTree()->m_shouldRender == true) //is an event from sdl
			{
				SDL_Event* event = (SDL_Event*)eventData;

				if (m_waitingForKeyInput == true) //we are waithing for a key or mouse button to be pressed
				{
					if (event->type == SDL_MOUSEBUTTONDOWN)
					{
						//mouse input gets remaped so it works with everthing a lot easier
						SetConVarKeyOwnership(UISHAR__UICMD__MOUSETOKEYINDEX + event->button.button); //sets mouse to key nutton

						m_waitingForKeyInput = false;
					}

					if (event->type == SDL_KEYDOWN)
					{
						unsigned int key = (unsigned int)(event->key.keysym.sym);
						SetConVarKeyOwnership(key);

						m_waitingForKeyInput = false; //sets key to key button
					}

					return true;
				}


				if (event->type == SDL_MOUSEBUTTONDOWN) //did you click button
				{
					stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
					SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] };
					SDL_Rect mousePos = SDL_Rect{ GetMousePos()[0], GetMousePos()[1], 1, 1 };

					if (SDL_HasIntersection(&mousePos, &renderRect) == true) //mouse clicked button
					{
						if (UIComponent_SDLScissorStack::IsEmpty() == false) //sissor check just to make sure
						{
							SDL_Rect checkRect = UIComponent_SDLScissorStack::Top();
							if (SDL_HasIntersection(&renderRect, &checkRect) == false) //oh no we cant handle the event
							{
								return false;
							}
						}

						if (event->button.button == SDL_BUTTON_LEFT)
						{
							m_waitingForKeyInput = true;
							return true;
						}

						if (event->button.button == SDL_BUTTON_RIGHT)
						{
							SetConVarKeyOwnership(UISHAR__KEYBUTTON_BLANK_KEY);
							return true;
						}
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


			//text aligment
			m_textBaseObject.SetHorizontalAlignment((mediaShared::media::SDLText_HorizontalLocation)(asset.GetSubObjectsByScope(0, "<horizontalAlignment>", "</horizontalAlignment>").GetContentsInt()));
			m_textBaseObject.SetVerticalAlignment((mediaShared::media::SDLText_VerticalLocation)(asset.GetSubObjectsByScope(0, "<verticalAlignment>", "</verticalAlignment>").GetContentsInt()));

			//font
			int fontSize = asset.GetSubObjectsByScope(0, "<textSize>", "</textSize>").GetContentsInt();
			std::string font =  std::string(asset.GetSubObjectsByScope(0, "<font>", "</font>").GetContents(), asset.GetSubObjectsByScope(0, "<font>", "</font>").GetContentsLength());

			char* fileData = nullptr;
			unsigned int fileSize = 0;
			GetItemFromPack(font, &fileData, &fileSize);

			if (fileSize > 0)
			{
				m_textBaseObject.SetFontPack(font, fileData, fileSize, fontSize);
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


			//color stuff colorText
			stackExternal::assets::AssetObjectContainer colorArrayT = asset.GetSubObjectsByScope(0, "<colorText>", "</colorText>");
			m_colorText.r = colorArrayT.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorText.g = colorArrayT.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorText.b = colorArrayT.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorText.a = colorArrayT.GetSubObjectByArrayElement(3).GetContentsInt();


			//color stuff colorNorm
			stackExternal::assets::AssetObjectContainer colorArrayNS = asset.GetSubObjectsByScope(0, "<colorNormSel>", "</colorNormSel>");
			m_colorNormSel.r = colorArrayNS.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorNormSel.g = colorArrayNS.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorNormSel.b = colorArrayNS.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorNormSel.a = colorArrayNS.GetSubObjectByArrayElement(3).GetContentsInt();

			//color stuff colorOuline
			stackExternal::assets::AssetObjectContainer colorArrayOS = asset.GetSubObjectsByScope(0, "<colorOutlineSel>", "</colorOutlineSel>");
			m_colorOutlineSel.r = colorArrayOS.GetSubObjectByArrayElement(0).GetContentsInt();
			m_colorOutlineSel.g = colorArrayOS.GetSubObjectByArrayElement(1).GetContentsInt();
			m_colorOutlineSel.b = colorArrayOS.GetSubObjectByArrayElement(2).GetContentsInt();
			m_colorOutlineSel.a = colorArrayOS.GetSubObjectByArrayElement(3).GetContentsInt();

			//convar stuff
			m_conVarName = std::string(asset.GetSubObjectsByScope(0, "<conVarName>", "</conVarName>").GetContents(), asset.GetSubObjectsByScope(0, "<conVarName>", "</conVarName>").GetContentsLength());
			m_claimOwnershipGroupId = ((unsigned int)asset.GetSubObjectsByScope(0, "<conVarName>", "</conVarName>").GetContentsInt());


			//tells the button to connect to the con var
			m_needsToRerenderText = true;
			try
			{
				m_myCurrentKeyCode = ((unsigned int)std::stoi(GetConVar(m_conVarName)));
			}
			catch (...)
			{
				m_myCurrentKeyCode = UISHAR__KEYBUTTON_UNKNOWN_KEY;
			}
		}

	public:
		mediaShared::media::SDLText m_textBaseObject = mediaShared::media::SDLText();
		int m_lineThickness = 2;

		SDL_Color m_colorHover = SDL_Color();
		SDL_Color m_colorNorm = SDL_Color();
		SDL_Color m_colorOutline = SDL_Color();
		SDL_Color m_colorText = SDL_Color();

		SDL_Color m_colorNormSel = SDL_Color();
		SDL_Color m_colorOutlineSel = SDL_Color();


		unsigned int m_claimOwnershipGroupId = 0;
		std::string m_conVarName = "test";
	};
}