#pragma once

#pragma once

#include <chrono>
#include <fstream>
#include <string>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

#include "MediaShared/SDLText/SDLText.h"
#include "MediaShared/SDLImage/SDLImage.h"

#include "../Elements/UIComponent_InputData.h"
#include "../Elements/UIComponent_ConnectionContext.h"
#include "../Elements/UIComponent_SDLScissorStack.h"

#include "scriptManagment.h"

namespace UIShared
{
	class SIMS_CC_ID_MasterList : public stackExternal::UI::UIElement, UIComponent_ConnectionContext, UIComponent_InputData
	{
	private:
		mediaShared::media::SDLImage m_bgTexture = mediaShared::media::SDLImage();
		mediaShared::media::SDLImage m_bgAdminIcon = mediaShared::media::SDLImage();

		mediaShared::media::SDLText m_titleText = mediaShared::media::SDLText();
		mediaShared::media::SDLText m_descriptionText = mediaShared::media::SDLText();


	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			if (renderingContext == nullptr) { return; }

			stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();
			bounds.offsetSize[1] = 0;

			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, 255, 255, 255, 255);

			sfra::scrman::ScriptSet mainScriptSet = sfra::scrman::globals::n_searchResults;
			if (mainScriptSet.size() == 0) { mainScriptSet = sfra::scrman::globals::n_masterScriptSet; }

			for (size_t i = 0; i < mainScriptSet.size(); i++)
			{
				m_bgTexture.SetPos({ bounds.offsetPos[0], bounds.offsetPos[1] + bounds.offsetSize[1] });
				m_bgTexture.SetSize({ bounds.offsetSize[0], 48 });
				m_bgTexture.RenderImage((SDL_Renderer*)renderingContext);

				if (mainScriptSet[i].needsAdmin == true)
				{
					m_bgAdminIcon.SetPos({ bounds.offsetPos[0] + bounds.offsetSize[0] - (28), bounds.offsetPos[1] + bounds.offsetSize[1] + 2 });
					m_bgAdminIcon.RenderImage((SDL_Renderer*)renderingContext);
				}

				m_titleText.SetPos({ bounds.offsetPos[0] + 2, bounds.offsetPos[1] + bounds.offsetSize[1] + 2 });
				m_titleText.SetSize({ bounds.offsetSize[0], 25 });
				m_titleText.SetText((SDL_Renderer*)renderingContext, mainScriptSet[i].title);
				m_titleText.RenderText((SDL_Renderer*)renderingContext);

				m_descriptionText.SetPos({ bounds.offsetPos[0] + 2, bounds.offsetPos[1] + bounds.offsetSize[1] + 28 });
				m_descriptionText.SetSize({ bounds.offsetSize[0], 17 });
				m_descriptionText.SetText((SDL_Renderer*)renderingContext, mainScriptSet[i].description);
				m_descriptionText.RenderText((SDL_Renderer*)renderingContext);


				bounds.offsetSize[1] = bounds.offsetSize[1] + 24 + 16 + 8;
			}

			bounds.offsetSize[1] = bounds.offsetSize[1] + 24 + 16 + 8;

			stackExternal::UI::UITransform trans = GetLocalTransform();
			trans.bounds.offsetSize[1] = bounds.offsetSize[1];
			SetLocalTransform(trans);
		}

		virtual bool OnEvent(unsigned int eventId, void* eventData) override
		{
			if (eventId == UISHAR__UICMD__SDL_EVENT && GetAttachedTree()->m_shouldRender == true) //is an event from sdl
			{
				SDL_Event* event = (SDL_Event*)eventData;

				if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
				{
					stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();

					sfra::scrman::ScriptSet mainScriptSet = sfra::scrman::globals::n_searchResults;
					if (mainScriptSet.size() == 0) { mainScriptSet = sfra::scrman::globals::n_masterScriptSet; }

					for (size_t i = 0; i < mainScriptSet.size(); i++)
					{
						bounds.offsetSize[1] = 24 + 16 + 8;
						SDL_Rect renderRect = SDL_Rect{ bounds.offsetPos[0], bounds.offsetPos[1], bounds.offsetSize[0], bounds.offsetSize[1] }; //the actual pos and size of the button
						SDL_Rect mousePos = SDL_Rect{ GetMousePos()[0], GetMousePos()[1], 1, 1 }; //the mouse pos
						SDL_Rect renderRectCliped = renderRect; //the size and pos of the button cliped to the sizor stack

						bool canRunEvent = true;

						if (UIComponent_SDLScissorStack::IsEmpty() == false) //sissor check just to make sure
						{
							SDL_Rect checkRect = UIComponent_SDLScissorStack::Top();

							if (SDL_IntersectRect(&checkRect, &renderRect, &renderRectCliped) == false) //oh no we cant handle the event
							{
								canRunEvent = false;
							}
						}

						if (canRunEvent == true && SDL_HasIntersection(&mousePos, &renderRectCliped) == true) //go to input mode
						{
							sfra::scrman::RunScript(mainScriptSet[i]);
							//std::cout << "click auto button: " << i << "\n";
						}

						bounds.offsetPos[1] = bounds.offsetPos[1] + bounds.offsetSize[1];
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

			std::string font = "fonts\\mainFont.ttf";
			char* fontData = nullptr;
			unsigned int fontDataSize = 0;
			GetItemFromPack(font, &fontData, &fontDataSize);

			if (fontDataSize > 0)
			{
				m_titleText.SetFontPack(font, fontData, fontDataSize, 24);
				m_descriptionText.SetFontPack(font, fontData, fontDataSize, 16);

				m_titleText.SetHorizontalAlignment(mediaShared::media::SDLText_HorizontalLocation::HLeft);
				m_titleText.SetVerticalAlignment(mediaShared::media::SDLText_VerticalLocation::VCenter);

				m_descriptionText.SetHorizontalAlignment(mediaShared::media::SDLText_HorizontalLocation::HLeft);
				m_descriptionText.SetVerticalAlignment(mediaShared::media::SDLText_VerticalLocation::VCenter);
			}


			std::string background = "textures\\item_bg.png";
			char* bgData = nullptr;
			unsigned int bgDataSize = 0;
			GetItemFromPack(background, &bgData, &bgDataSize);

			if (bgDataSize > 0)
			{
				m_bgTexture.SetTexturePack((SDL_Renderer*)propertyContext, background, bgData, bgDataSize);
			}


			std::string admin = "textures\\item_a.png";
			char* adminData = nullptr;
			unsigned int adminDataSize = 0;
			GetItemFromPack(admin, &adminData, &adminDataSize);

			if (adminDataSize > 0)
			{
				m_bgAdminIcon.SetTexturePack((SDL_Renderer*)propertyContext, admin, adminData, adminDataSize);
				m_bgAdminIcon.SetSize(m_bgAdminIcon.GetTextureOrignalSize());
			}
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{

		}

	public:

	};
}