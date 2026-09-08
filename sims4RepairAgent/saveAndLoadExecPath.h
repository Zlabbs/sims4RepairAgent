#pragma once

#include "SDL.h"

#include <fstream>
#include <string>

namespace sfra
{
	namespace svldExecPath
	{
		std::string LoadExecPath()
		{
			char* pathToAssetRaw = SDL_GetPrefPath("S4RA", "out");
			std::string pathToAsset = std::string(pathToAssetRaw) + "execPath.dat";
			SDL_free(pathToAssetRaw);


			std::ifstream fileReader = std::ifstream();
			fileReader.open(pathToAsset, std::ios::binary);

			std::string returnPath = "";

			while (fileReader.is_open() == true && fileReader.good() == true && fileReader.eof() == false) //could not access package
			{
				char letter = '\0';
				fileReader.read(&letter, 1);
				returnPath = returnPath + letter;
			}

			fileReader.close();

			return returnPath;
		}


		void SaveExecPath(std::string path)
		{
			std::fstream fileWriter = std::fstream();

			char* pathToAssetRaw = SDL_GetPrefPath("S4RA", "out");
			std::string pathToAsset = std::string(pathToAssetRaw) + "execPath.dat";
			SDL_free(pathToAssetRaw);

			fileWriter.open(pathToAsset, std::ios::out | std::ios::in | std::ios::trunc);

			fileWriter << path;

			fileWriter.close();
		}
	}
}