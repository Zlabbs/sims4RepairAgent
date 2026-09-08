#pragma once

#include <fstream>
#include <string>

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

#include "UIE_SceneAttachedRoot.h"

#include "UIComponent_ConnectionContext.h"
#include "UIComponent_InputData.h" //this only gets included so we have an easy access to UISHAR__UICMD__MOUSETOKEYINDEX

namespace UIShared
{
	class UIE_CC_KeyEvent : public stackExternal::UI::UIElement, UIComponent_ConnectionContext
	{
	private:
		void* m_scriptData = nullptr;

	protected:
		virtual void OnRemove() override
		{
			FreeScript(m_scriptData);
			delete this;
		}

		virtual bool OnEvent(unsigned int eventId, void* eventData) override
		{
			if (eventId == UISHAR__UICMD__SDL_EVENT) //is an event from sdl
			{
				SDL_Event* event = (SDL_Event*)eventData;
				stackExternal::UI::UITransformBounds bounds = GetWorldTransformBounds();

				if (m_visibilityRequired == false || (bounds.offsetPos[0] + bounds.offsetSize[0] >= 0 && bounds.offsetPos[1] + bounds.offsetSize[1] >= 0 && GetAttachedTree()->m_shouldRender == true))
				{
					if (event->type == SDL_MOUSEBUTTONDOWN)
					{
						//mouse input gets remaped so it works with everthing a lot easier
						unsigned int key = (UISHAR__UICMD__MOUSETOKEYINDEX + event->button.button); //sets mouse to key button

						std::string eventInScriptToRun = "OnKeyPressAll";
						std::string dataToPassOnToScript = std::to_string(key);
						UIE_CC_KeyEvent* runnerOfScript = this;
						void* eventParentObject = UIE_SceneAttachedRoot::GetUiOwner(); //the thing rendering this ui if its screen render this would be nullptr
						stackExternal::UI::UITreeContiner* ownerOfUiTree = GetAttachedTree();

						RunSript(m_scriptData, eventInScriptToRun, dataToPassOnToScript, runnerOfScript, eventParentObject, ownerOfUiTree); //for key presses that involve both keyboard and mouse


						eventInScriptToRun = "OnMousePress";
						dataToPassOnToScript = std::to_string(event->button.button);
						RunSript(m_scriptData, eventInScriptToRun, dataToPassOnToScript, runnerOfScript, eventParentObject, ownerOfUiTree); //for mouse presses only
						return true;
					}

					if (event->type == SDL_KEYDOWN)
					{
						unsigned int key = (unsigned int)(event->key.keysym.sym);

						std::string eventInScriptToRun = "OnKeyPressAll";
						std::string dataToPassOnToScript = std::to_string(key);
						UIE_CC_KeyEvent* runnerOfScript = this;
						void* eventParentObject = UIE_SceneAttachedRoot::GetUiOwner(); //the thing rendering this ui if its screen render this would be nullptr
						stackExternal::UI::UITreeContiner* ownerOfUiTree = GetAttachedTree();

						RunSript(m_scriptData, eventInScriptToRun, dataToPassOnToScript, runnerOfScript, eventParentObject, ownerOfUiTree); //for key presses that involve both keyboard and mouse


						eventInScriptToRun = "OnKeyPress";
						dataToPassOnToScript = std::to_string(key);
						RunSript(m_scriptData, eventInScriptToRun, dataToPassOnToScript, runnerOfScript, eventParentObject, ownerOfUiTree);
						return true;
					}
				}
			}

			return false;
		}

		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  

			m_visibilityRequired = asset.GetSubObjectsByScope(0, "<visibilityRequired>", "</visibilityRequired>").GetContentsBool(); //dose the event or the parent of the event I should say need to be visible

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

		}
	public:
		int m_visibilityRequired = true;
	};
}