#pragma once

#include <fstream>
#include <string>

#include "StandardShared/ConOutput/ConOutput.h"
#include "UIShared/Elements/UIE_CC_TextElement.h"

#include "AssestPackagingSource/AssestPackagingSource.h"
#include "StandardShared/ConOutput/ConOutput.h"

namespace sfra
{
	namespace packaging
	{
		namespace globals
		{
			stackExternal::assets::PackObjectContainer n_pack;
		}

		//use by ConnectionContext ui element so that it know if a texture or a font can be found in a pack
		bool IsInPackage(std::string item)
		{
			return (sfra::packaging::globals::n_pack.GetFileLengthFromPack(item.c_str()) != SE_APS__PACK_ITEM_INVALID_LEN_ERROR); //simple but effective
		}

		//use by ConnectionContext ui element so that it grab data from a pack
		void GetItemInPackage(std::string item, char** fileData, unsigned int* fileSize)
		{
			(*fileSize) = sfra::packaging::globals::n_pack.GetFileLengthFromPack(item.c_str());
			(*fileData) = sfra::packaging::globals::n_pack.GetBlock();
		}

		//loads the editor package
		bool LoadPackage()
		{
			char* pathToPackageRaw = SDL_GetBasePath();
			std::string pathToPackage = std::string(pathToPackageRaw);
			SDL_free(pathToPackageRaw);

			std::string mainPackage = pathToPackage+"pakdata/main.spk"; //where to find it

			std::ifstream fileReader = std::ifstream();
			fileReader.open(mainPackage, std::ios::binary);

			if (fileReader.is_open() == false || fileReader.good() == false || fileReader.eof() == true) //could not access package
			{
				standardShared::con::PrintText(standardShared::con::PrintType::Failure, "CRITCAL ERROR: ", "base package could not be found at:", mainPackage);
				return false;
			}

			//get pack size
			fileReader.seekg(0, std::ios::end);
			size_t fileSize = fileReader.tellg();
			fileSize = fileSize;
			fileReader.close();

			//get pack data
			fileReader.open(mainPackage, std::ios::binary);
			char* fileData = new char[fileSize];
			fileReader.read(fileData, fileSize);
			fileReader.close();

			//load content into pack container so files in the pack can be easily accessed
			sfra::packaging::globals::n_pack.StartCreatePack(fileSize);
			sfra::packaging::globals::n_pack.SetBlock(fileData);
			sfra::packaging::globals::n_pack.EndCreatePack();
			delete[] fileData;

			//the ui should only be able to connect to packaging so might as well connect that here
			UIShared::UIComponent_ConnectionContext::InitalizeConnectionContext(IsInPackage, GetItemInPackage, nullptr, nullptr, nullptr, nullptr);

			return true;
		}
	}
}