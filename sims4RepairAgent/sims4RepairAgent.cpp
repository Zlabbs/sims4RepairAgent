// sims4RepairAgent.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "SDL.h"

#include "AssestPackagingSource/AssestPackagingSource.h"
#include "UISource/UISource.h"

#include "UIShared/ModElements/SIMS_CC_ID_ExecPath.h"
#include "UIShared/ModElements/KeyboardEvents.h"
#include "MediaShared/ProgramClass/SDLProgramClass.h"
#include "StandardShared/ConOutput/ConOutput.h"
#include "UIShared/AssetDecoder.h"

#include "packageMounting.h"
#include "scriptManagment.h"
#include "saveAndLoadExecPath.h"

void ProgramClassExitEvent()
{
	standardShared::con::PrintText(standardShared::con::PrintType::Normal, "deleting: cach data", "", "");

	mediaShared::media::SDLImage::FreeTextureCache();
	mediaShared::media::SDLText::FreeTextureBuffer();
	mediaShared::media::SDLText::FreeFontCache();

	standardShared::con::PrintText(standardShared::con::PrintType::Normal, "deleting: worker threads", "", "");
	sfra::scrman::EndUpdateScriptData();
	sfra::scrman::EndRunAutoRunChecks();
}

int main(int argc, char* argv[])
{
	//loads package
	if (sfra::packaging::LoadPackage() == false)
	{
		return 2;
	}


	//loads window
	SDL_Event event = SDL_Event();
	std::map<unsigned int, bool> keyMap = std::map<unsigned int, bool>();
	mediaShared::program::SDLProgramClass::Start("sims4 repair agent", 500, 600);
	mediaShared::program::SDLProgramClass::SetPreRemoveEvent(ProgramClassExitEvent);
	SDL_SetWindowMinimumSize(mediaShared::program::SDLProgramClass::m_windowObject, 500, 600);


	//loads ui
	char* assetData = sfra::packaging::globals::n_pack.GetFileDataFromPack("uisBuilt\\S4RA_UI.suio");
	unsigned int assetByteLength = sfra::packaging::globals::n_pack.GetFileLengthFromPack("uisBuilt\\S4RA_UI.suio");

	if (assetByteLength == SE_APS__PACK_ITEM_INVALID_LEN_ERROR)
	{
		return 1;
	}

	stackExternal::assets::AssetObjectContainer uiAsset = stackExternal::assets::AssetObjectContainer(assetData, assetByteLength);
	bool canCreateDebugUi = false;

	stackExternal::UI::UITreeContiner* treeContainerMain = UIShared::DecodeAssestToUiTree(mediaShared::program::SDLProgramClass::m_rendererObject, &uiAsset, canCreateDebugUi);


	//load exec path
	std::string execPath = sfra::svldExecPath::LoadExecPath();
	if (execPath != "")
	{
		//sfra::scrman::globals::n_sims4Location = execPath; //the line bellow should also do this
		UIShared::SIMS_CC_ID_ExecPath* execPathUIObject = ((UIShared::SIMS_CC_ID_ExecPath*)treeContainerMain->GetElementByTagName("execPath"));
		execPathUIObject->SetPath(execPath);
	}


	//dowlaod the latest version of the scripts db (MUST BE DONE BEFORE SCRIPTS ARE USED FOR SECURITY REASONS)
	sfra::scrman::UpdateScriptData();
	

	//render loop
	while (mediaShared::program::SDLProgramClass::IsValid() == true)
	{
		//base update stuff
		SDL_PollEvent(&event);
		mediaShared::program::SDLProgramClass::Update(event);
		SDL_Renderer* renderer = mediaShared::program::SDLProgramClass::m_rendererObject;
		unsigned int key = (unsigned int)(event.key.keysym.sym);

		//connect ui to keyboad and mouse
		UIShared::UIComponent_InputData::FeedKeyData(SDL_GetModState() & SDL_Keymod::KMOD_CAPS, SDL_GetModState() & SDL_Keymod::KMOD_SHIFT); //feeds the game ui the caps and shif key states

		int mouseX, mouseY = 0;
		SDL_GetMouseState(&mouseX, &mouseY);
		UIShared::UIComponent_InputData::FeedMouseData({ mouseX, mouseY });

		switch (event.type)
		{
		case SDL_EventType::SDL_KEYDOWN:
			if (keyMap.count(key) == 0 || keyMap[key] == false)
			{
				int ui = treeContainerMain->PassEventAsHeightList(MOD_KEY_DOWN, &key);

				keyMap[key] = true;
			}
			break;

		case SDL_EventType::SDL_KEYUP:
			treeContainerMain->PassEventAsHeightList(MOD_KEY_UP, &key);

			keyMap[key] = false;
			break;

		default:
			break;
		}


		//render ui
		UIShared::UIE_ScreenRoot::SetScreenSize(mediaShared::program::SDLProgramClass::m_windowCurrentSize);
		treeContainerMain->RenderTreeAsList(mediaShared::program::SDLProgramClass::m_rendererObject); //it will render the tree as a list mean no depth first search
		treeContainerMain->PassEventAsHeightList(UISHAR__UICMD__SDL_EVENT, &event); //gets the z hight of all elements and passes the events to the elements higher up on z first and then passes it to the ones with a low z last 


		SDL_RenderPresent(mediaShared::program::SDLProgramClass::m_rendererObject);

		if (sfra::scrman::IsUpdateInProgress() == false)
		{
			sfra::scrman::EndUpdateScriptData();
		}

		if (sfra::scrman::ShouldApplyUpdateNow() == true) //sfra::scrman::DidUpdateWork() == true if your using this it will stop old versions of the db being applyed even if they are there so we dont use it
		{
			stackExternal::assets::AssetObjectContainer updateAsset = sfra::scrman::GetMasterScriptAsset();
			sfra::scrman::LoadMasterScriptFromAsset(&updateAsset);
			sfra::scrman::RunAutoRunChecks();
		}
	}

	//final exit steps
	delete treeContainerMain;
	sfra::svldExecPath::SaveExecPath(sfra::scrman::globals::n_sims4Location);

	return 0;
}

