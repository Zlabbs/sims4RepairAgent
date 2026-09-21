#pragma once

#define NOMINMAX

#include "SDL.h"

#include <string>
#include <fstream>
#include <vector>
#include <array>
#include <thread>
#include <atomic>
#include <cstdlib>
#include <Lmcons.h>
#include <windows.h>

#include "AssestPackagingSource/AssestPackagingSource.h"

#include "StandardShared/ConOutput/ConOutput.h"

namespace sfra
{
	namespace scrman
	{
		struct ScriptData
		{
			bool needsAdmin = false;
			bool needsAdminFromAgent = false;
			std::string autoCheckScript = "";
			std::string runScript = "";
			std::string title = "";
			std::string description = "";
			std::vector<std::string> searchTags_Helpful = std::vector<std::string>();
			std::vector<std::string> searchTags_Required = std::vector<std::string>();


			bool inter_didAutoCheckScriptSayWeNeedToRun = false;
			unsigned int inter_SearchScore = 0;
		};

		typedef std::vector<ScriptData> ScriptSet;

		namespace globals
		{
			std::string n_sims4RepairAgentDonateWebLocation = "";
			std::string n_sims4Location = "SM_G_S4L: error value not set";
			ScriptSet n_masterScriptSet = ScriptSet(); ///should not be change by anything other than the functon in the exterior namespace
			ScriptSet n_searchResults = ScriptSet();
		}

		namespace hidden
		{
			std::thread* n_autoRunThread = nullptr;
			std::atomic<bool> n_autoRunThreadInProgress = false;

			std::thread* n_scriptUpdatingThread = nullptr;
			std::atomic<bool> n_updateInProgress = false;
			std::atomic<bool> n_updateNeedsToBeApplyed = false;
			std::atomic<bool> n_updateSuccessful = false;
		}


		void UpdateScriptData_ThreadWorker()
		{
			hidden::n_updateInProgress = true;

			char* pathToPackageRaw = SDL_GetBasePath();
			std::string pathToPackage = std::string(pathToPackageRaw);
			SDL_free(pathToPackageRaw);

			pathToPackage = ("" + pathToPackage + "AgentScriptUpdater.exe");

			STARTUPINFOA execStartUpInfo;
			PROCESS_INFORMATION execProcessInfo;

			ZeroMemory(&execStartUpInfo, sizeof(execStartUpInfo));
			execStartUpInfo.cb = sizeof(execStartUpInfo);
			ZeroMemory(&execProcessInfo, sizeof(execProcessInfo));

			unsigned long exitCode = 0;
			if (CreateProcessA(pathToPackage.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &execStartUpInfo, &execProcessInfo) == 0)
			{
				CloseHandle(execProcessInfo.hProcess);
				CloseHandle(execProcessInfo.hThread);
				hidden::n_updateSuccessful = false;
				hidden::n_updateInProgress = false;
				return;
			}

			WaitForSingleObject(execProcessInfo.hProcess, INFINITE);
			GetExitCodeProcess(execProcessInfo.hProcess, &exitCode);

			CloseHandle(execProcessInfo.hProcess);
			CloseHandle(execProcessInfo.hThread);

			//int exitCode = WinExec(("\"" + pathToPackage + "AgentScriptUpdater.exe\"").c_str(), SW_HIDE);
			hidden::n_updateSuccessful = (exitCode == 0);

			hidden::n_updateInProgress = false;
		}

		void UpdateScriptData()
		{
			hidden::n_updateInProgress = true;
			hidden::n_scriptUpdatingThread = new std::thread(UpdateScriptData_ThreadWorker);
		}

		void EndUpdateScriptData()
		{
			if (hidden::n_updateInProgress == false && hidden::n_scriptUpdatingThread != nullptr)
			{
				if (hidden::n_scriptUpdatingThread->joinable() == true)
				{
					hidden::n_scriptUpdatingThread->join();
				}

				delete hidden::n_scriptUpdatingThread;
				hidden::n_scriptUpdatingThread = nullptr;
				hidden::n_updateNeedsToBeApplyed = true;
			}
		}

		bool DidUpdateWork()
		{
			return hidden::n_updateSuccessful;
		}

		bool IsUpdateInProgress()
		{
			return hidden::n_updateInProgress;
		}

		bool ShouldApplyUpdateNow()
		{
			bool temp = hidden::n_updateNeedsToBeApplyed;
			hidden::n_updateNeedsToBeApplyed = false;
			return temp;
		}

		bool SafeToStartUpdateInstance()
		{
			return (hidden::n_scriptUpdatingThread == nullptr);
		}



		stackExternal::assets::AssetObjectContainer GetMasterScriptAsset()
		{
			//get master script data
			char* pathToAssetRaw = SDL_GetPrefPath("S4RA", "out\\sims4RepairAgent_Scripts-main");
			if (pathToAssetRaw == nullptr) { return stackExternal::assets::AssetObjectContainer(); }

			std::string pathToAsset = std::string(pathToAssetRaw)+"scriptsData.saf";
			SDL_free(pathToAssetRaw);

			std::ifstream fileReader = std::ifstream();
			fileReader.open(pathToAsset, std::ios::binary);


			if (fileReader.is_open() == false || fileReader.good() == false || fileReader.eof() == true) //could not access package
			{
				standardShared::con::PrintText(standardShared::con::PrintType::Warning, "WARNING SFRA_SCRMAN_GMSA: ", "master script could not be located at:", pathToAsset);
				return stackExternal::assets::AssetObjectContainer();
			}


			//get asset size
			fileReader.seekg(0, std::ios::end);
			size_t fileSize = fileReader.tellg();
			fileSize = fileSize;
			fileReader.close();


			//get pack data
			fileReader.open(pathToAsset, std::ios::binary);
			char* fileData = new char[fileSize];
			fileReader.read(fileData, fileSize);
			fileReader.close();


			return stackExternal::assets::AssetObjectContainer(fileData, ((unsigned int)fileSize));
		}

		bool LoadMasterScriptFromAsset(stackExternal::assets::AssetObjectContainer* asset)
		{
			//quick fail
			if (asset->IsContentsValid() == false)
			{
				return false;
			}

			//clears out old junk data
			globals::n_masterScriptSet.clear();

			globals::n_sims4RepairAgentDonateWebLocation = std::string(asset->GetSubObjectsByScope(0, "<donateWebLink>", "</donateWebLink>").GetContents(), asset->GetSubObjectsByScope(0, "<donateWebLink>", "</donateWebLink>").GetContentsLength());


			//go thougth the asset and reflects everying out to the master script set
			unsigned int jumpPoint = 0;
			for (unsigned int i = 0; i < asset->GetSubObjectsByScopeLength("<script>", "</script>"); i++)
			{
				ScriptData currentScriptReal = ScriptData();
				stackExternal::assets::AssetObjectContainer currentScriptAsset = asset->GetSubObjectsByScope(0, "<script>", "</script>", &jumpPoint);

				//admin check, main script, auto test script
				currentScriptReal.needsAdmin = currentScriptAsset.GetSubObjectsByScope(0, "<needsAdmin>", "</needsAdmin>").GetContentsBool();
				currentScriptReal.needsAdminFromAgent = currentScriptAsset.GetSubObjectsByScope(0, "<needsAdminFromAgent>", "</needsAdminFromAgent>").GetContentsBool();
				currentScriptReal.runScript = std::string(currentScriptAsset.GetSubObjectsByScope(0, "<runScript>", "</runScript>").GetContents(), currentScriptAsset.GetSubObjectsByScope(0, "<runScript>", "</runScript>").GetContentsLength());
				currentScriptReal.autoCheckScript = std::string(currentScriptAsset.GetSubObjectsByScope(0, "<autoCheckScript>", "</autoCheckScript>").GetContents(), currentScriptAsset.GetSubObjectsByScope(0, "<autoCheckScript>", "</autoCheckScript>").GetContentsLength());
				

				//title & description
				currentScriptReal.title = std::string(currentScriptAsset.GetSubObjectsByScope(0, "<title>", "</title>").GetContents(), currentScriptAsset.GetSubObjectsByScope(0, "<title>", "</title>").GetContentsLength());
				currentScriptReal.description = std::string(currentScriptAsset.GetSubObjectsByScope(0, "<desc>", "</desc>").GetContents(), currentScriptAsset.GetSubObjectsByScope(0, "<desc>", "</desc>").GetContentsLength());


				//helpful searh tags fast loader
				stackExternal::assets::AssetObjectContainer helpfulTags = currentScriptAsset.GetSubObjectsByScope(0, "<searchTags_Helpful>", "</searchTags_Helpful>");
				unsigned int internalJumpPoint = 0;
				unsigned int arrayLength = helpfulTags.GetSubObjectByArrayElementLength();
				for (unsigned int o = 0; o < arrayLength; o++)
				{
					stackExternal::assets::AssetObjectContainer tag = helpfulTags.GetSubObjectByArrayElement(0, ",", &internalJumpPoint);
					currentScriptReal.searchTags_Helpful.push_back(std::string(tag.GetContents(), tag.GetContentsLength()));
				}

				//required search tags fast loader
				stackExternal::assets::AssetObjectContainer requiredTags = currentScriptAsset.GetSubObjectsByScope(0, "<searchTags_Required>", "</searchTags_Required>");
				internalJumpPoint = 0;
				 arrayLength = requiredTags.GetSubObjectByArrayElementLength();
				for (unsigned int o = 0; o < arrayLength; o++)
				{
					stackExternal::assets::AssetObjectContainer tag = requiredTags.GetSubObjectByArrayElement(0, ",", &internalJumpPoint);
					currentScriptReal.searchTags_Required.push_back(std::string(tag.GetContents(), tag.GetContentsLength()));
				}

				//add object to master set
				globals::n_masterScriptSet.push_back(currentScriptReal);
			}

			return true;
		}



		void RunAutoRunChecks_ThreadWorker()
		{
			hidden::n_autoRunThreadInProgress = true;

			char* pathToAssetRaw = SDL_GetPrefPath("S4RA", "out\\sims4RepairAgent_Scripts-main");
			std::string pathToAsset = std::string(pathToAssetRaw);
			SDL_free(pathToAssetRaw);

			for (size_t i = 0; i < globals::n_masterScriptSet.size(); i++)
			{
				if (globals::n_masterScriptSet[i].autoCheckScript != "")
				{
					char* pathToAssetRaw = SDL_GetPrefPath("S4RA", "out\\sims4RepairAgent_Scripts-main");
					std::string pathToAsset = std::string(pathToAssetRaw);
					SDL_free(pathToAssetRaw);


					std::string commandLineInstructions = "/c call " + pathToAsset + globals::n_masterScriptSet[i].autoCheckScript;
					char commandLineInterfaceProgramRaw[] = "C:\\Windows\\System32\\cmd.exe";
					char* commandLineInstructionsRaw = (char*)(commandLineInstructions.c_str());


					STARTUPINFOA execStartUpInfo;
					PROCESS_INFORMATION execProcessInfo;

					ZeroMemory(&execStartUpInfo, sizeof(execStartUpInfo));
					execStartUpInfo.cb = sizeof(execStartUpInfo);
					ZeroMemory(&execProcessInfo, sizeof(execProcessInfo));

					unsigned long exitCode = 0;
					if (CreateProcessA(commandLineInterfaceProgramRaw, commandLineInstructionsRaw, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &execStartUpInfo, &execProcessInfo) == false)
					{
						standardShared::con::PrintText(standardShared::con::PrintType::Warning, "WARNING SFRA_SCRMAN_RARC:", "auto run process creation failed attempted to create: ", commandLineInstructions);
					}

					WaitForSingleObject(execProcessInfo.hProcess, INFINITE);
					GetExitCodeProcess(execProcessInfo.hProcess, &exitCode);

					globals::n_masterScriptSet[i].inter_didAutoCheckScriptSayWeNeedToRun = (exitCode != 0);

					CloseHandle(execProcessInfo.hProcess);
					CloseHandle(execProcessInfo.hThread);
				}
			}

			hidden::n_autoRunThreadInProgress = false;
		}

		void RunAutoRunChecks()
		{
			hidden::n_autoRunThreadInProgress = true;
			hidden::n_autoRunThread = new std::thread(RunAutoRunChecks_ThreadWorker);
		}

		void EndRunAutoRunChecks()
		{
			if (hidden::n_autoRunThreadInProgress == false && hidden::n_autoRunThread != nullptr)
			{
				if (hidden::n_autoRunThread->joinable() == true)
				{
					hidden::n_autoRunThread->join();
				}

				delete hidden::n_autoRunThread;
				hidden::n_autoRunThread = nullptr;
			}
		}

		bool IsAutoRunChecksInProgress()
		{
			return hidden::n_autoRunThreadInProgress;
		}

		bool SafeToStartAutoRunInstance()
		{
			return (hidden::n_autoRunThread == nullptr);
		}


		void RunScript(ScriptData* data)
		{
			data->inter_didAutoCheckScriptSayWeNeedToRun = false;

			char* pathToAssetRaw = SDL_GetPrefPath("S4RA", "out\\sims4RepairAgent_Scripts-main");
			std::string pathToAsset = std::string(pathToAssetRaw);
			SDL_free(pathToAssetRaw);

			std::string commandLineInstructions = "/c call " + pathToAsset + data->runScript;
			char commandLineInterfaceProgramRaw[] = "C:\\Windows\\System32\\cmd.exe";
			char* commandLineInstructionsRaw = (char*)(commandLineInstructions.c_str());

			if (data->needsAdminFromAgent == false)
			{
				ShellExecuteA(NULL, "open", commandLineInterfaceProgramRaw, commandLineInstructionsRaw, globals::n_sims4Location.c_str(), SW_HIDE);
			}
			else
			{
				ShellExecuteA(NULL, "runas", commandLineInterfaceProgramRaw, commandLineInstructionsRaw, globals::n_sims4Location.c_str(), SW_HIDE);
			}
		}


		int ResultsFilterQsortComparer(const void* a, const void* b)
		{
			if (((ScriptData*)a)->inter_SearchScore < ((ScriptData*)b)->inter_SearchScore) return 1;
			if (((ScriptData*)a)->inter_SearchScore == ((ScriptData*)b)->inter_SearchScore) return 0;
			if (((ScriptData*)a)->inter_SearchScore > ((ScriptData*)b)->inter_SearchScore) return -1;
		}

		ScriptSet GetFilterdResultsFromMasterSet(std::string filter)
		{
			ScriptSet returnSet = ScriptSet();
			returnSet.reserve(globals::n_masterScriptSet.size());

			//forces the filter intop lowercase
			for (size_t i = 0; i < filter.size(); i++)
			{
				filter[i] = tolower(filter[i]);
			}


			//scores the scripts
			for (size_t i = 0; i < globals::n_masterScriptSet.size(); i++)
			{
				int currentSearchScore = 0;


				//required tags
				for (size_t o = 0; currentSearchScore >= 0 && o < globals::n_masterScriptSet[i].searchTags_Required.size(); o++)
				{
					if (filter.find(globals::n_masterScriptSet[i].searchTags_Required[o]) != std::string::npos)
					{
						currentSearchScore = currentSearchScore + 1;
					}
					else
					{
						currentSearchScore = -1;
						break;
					}
				}

				//helpful to idenify best results tags
				for (size_t o = 0; currentSearchScore >= 0 && o < globals::n_masterScriptSet[i].searchTags_Helpful.size(); o++)
				{
					if (filter.find(globals::n_masterScriptSet[i].searchTags_Helpful[o]) != std::string::npos)
					{
						currentSearchScore = currentSearchScore + 1;
					}
				}

				//stores the score and the datat if the score is above 0
				globals::n_masterScriptSet[i].inter_SearchScore = currentSearchScore;
				if (currentSearchScore > 0)
				{
					returnSet.push_back(globals::n_masterScriptSet[i]);
				}
			}

			//sorts by highest score
			if (returnSet.size() > 0)
			{
				std::qsort(&returnSet[0], returnSet.size(), sizeof(ScriptData), ResultsFilterQsortComparer);
			}

			return returnSet;
		}
	}
}