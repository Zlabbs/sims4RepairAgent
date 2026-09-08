#pragma once

#include <fstream>
#include <string>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

#include "UIE_SceneAttachedRoot.h"

#include "UIComponent_InputData.h"
#include "UIComponent_ConnectionContext.h"
#include "UIComponent_SDLScissorStack.h"

namespace UIShared
{
	class UIE_CC_ID_Button : public stackExternal::UI::UIElement, UIComponent_ConnectionContext, UIComponent_InputData
	{
	private:
		void* m_scriptData = nullptr;
		bool m_inHoverMode = false;

	protected:
		virtual void OnRemove() override
		{
			FreeScript(m_scriptData);
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


			//hover event
			if (SDL_HasIntersection(&mousePos, &renderRect) == true)
			{
				bool canRunEvent = true;

				if (UIComponent_SDLScissorStack::IsEmpty() == false) //sissor check just to make sure
				{
					SDL_Rect checkRect = UIComponent_SDLScissorStack::Top();
					if (SDL_HasIntersection(&renderRect, &checkRect) == false) //oh no we cant handle the event
					{
						canRunEvent = false;
					}
				}

				if (m_inHoverMode == false && canRunEvent == true)
				{
					std::string eventInScriptToRun = "OnButtonHover";
					std::string dataToPassOnToScript = "";
					UIE_CC_ID_Button* runnerOfScript = this;
					void* eventParentObject = UIE_SceneAttachedRoot::GetUiOwner(); //the thing rendering this ui if its screen render this would be nullptr
					stackExternal::UI::UITreeContiner* ownerOfUiTree = GetAttachedTree();

					RunSript(m_scriptData, eventInScriptToRun, dataToPassOnToScript, runnerOfScript, eventParentObject, ownerOfUiTree);
					m_inHoverMode = true;
				}
			}
			else
			{
				m_inHoverMode = false;
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
						std::string eventInScriptToRun = "OnButtonClick";
						std::string dataToPassOnToScript = "";
						UIE_CC_ID_Button* runnerOfScript = this;
						void* eventParentObject = UIE_SceneAttachedRoot::GetUiOwner(); //the thing rendering this ui if its screen render this would be nullptr
						stackExternal::UI::UITreeContiner* ownerOfUiTree = GetAttachedTree();

						RunSript(m_scriptData, eventInScriptToRun, dataToPassOnToScript, runnerOfScript, eventParentObject, ownerOfUiTree);
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


			//script data
			std::string scriptPath = std::string(asset.GetSubObjectsByScope(0, "<script>", "</script>").GetContents(), asset.GetSubObjectsByScope(0, "<script>", "</script>").GetContentsLength());
			if (IsItemInPack(scriptPath) == false) //loading from file
			{
				std::string scriptFileData = ""; //where we dump the file data
				std::ifstream fileReader = std::ifstream();
				fileReader.open(scriptPath, std::ios::binary);

				while (fileReader.is_open() == true && fileReader.good() == true && fileReader.eof() == false) //gets all file contents into single string
				{
					std::string parserLine = "";
					std::getline(fileReader, parserLine);
					scriptFileData = scriptFileData + parserLine;
				}

				m_scriptData = CompileScript(&scriptFileData); //compiles that string and gets a script
			}
			else
			{
				char* fileData = nullptr;
				unsigned int fileSize = 0;
				GetItemFromPack(scriptPath, &fileData, &fileSize);

				if (fileSize > 0)
				{	
					std::string temp = std::string(fileData, fileSize);
					m_scriptData = CompileScript(&temp); //simples
				}
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
		}

	public:
		SDL_Color m_colorHover = SDL_Color();
		SDL_Color m_colorNorm = SDL_Color();
		SDL_Color m_colorOutline = SDL_Color();

		int m_lineThickness = 2;
	};
}