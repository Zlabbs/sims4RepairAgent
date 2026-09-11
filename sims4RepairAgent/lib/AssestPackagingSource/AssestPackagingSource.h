#pragma once

#include <iostream>

/*=====================================================================================\
* Designed by Kyle Edwards.                                                            *
* Created on the 10/7/2026. Last updated 11/9/2026.                                    *
*=====================================================================================*/

#define SE_APS__MAX_ASSET_PATH_LENGTH 2048

#define SE_APS__PACK_ITEM_INVALID_LEN_ERROR ((unsigned int)-1)
#define SE_APS__PACK_MAX_CONTENTS_LENGTH 4294967295 //its unreasonable but it works
#define SE_APS__PACK_MAX_PATH_LENGTH 2048

#if _WIN32 || _WIN64 || _WINDOWS || _WINDLL
	#ifdef ASSESTPACKAGINGSOURCE_EXPORTS
	#define ASSESTPACKAGINGSOURCE_API __declspec(dllexport)
	#else
	#define ASSESTPACKAGINGSOURCE_API __declspec(dllimport)
	#endif
#else
#define ASSESTPACKAGINGSOURCE_API
#endif

namespace stackExternal
{
	namespace assets
	{
		/**
		* @brief finds the data between text based scopes ie it starts with x thing and ends with y thing like so: <example>, </example> it can then allow you to find data in even smaller scopes or get the data out as a string or a float or an int. it is ment to handle asset data from files and make it easy and fast to parse thougth them. To use it you must first provide it with the data as a char string and length is optinal.
		*/
		class ASSESTPACKAGINGSOURCE_API AssetObjectContainer
		{
		private:
			char* m_fieldContents = nullptr;
			unsigned int m_fieldContentsLength = 0;

		public:
			AssetObjectContainer();
			AssetObjectContainer(const char* item); //how you load data into the contents
			AssetObjectContainer(const char* item, unsigned int itemLength); ///how you load data into the contents
			~AssetObjectContainer();
			AssetObjectContainer& operator=(const AssetObjectContainer& t) = delete;
			AssetObjectContainer(AssetObjectContainer const&) = delete;
			AssetObjectContainer(AssetObjectContainer&& h) = default;



			/**
			* @brief Gets the contents as a string but one without a null terminator.
			* @return A undeleteable (it will be delete automatticly when not in use) char pointer to this string.
			*/
			char* GetContents();

			/**
			* @brief Gets the length of the contents in string form.
			* @return A length that should pervent you from accessing out of bounds memory.
			*/
			unsigned int GetContentsLength();

			/**
			* @brief Gets the contents as a int.
			* @return The contents as an int or 0 if the contents is invalid.
			*/
			int GetContentsInt();

			/**
			* @brief Gets the contents as a float.
			* @return Tthe contents as an float or 0 if the contents is invalid.
			*/
			float GetContentsFloat();

			/**
			* @brief Gets the contents as a bool.
			* @return The contents as an bool or false if the contents is invalid.
			*/
			bool GetContentsBool();


			/**
			* @brief Finds out if the contents are valid.
			* @return True if they are valid false if otherwise but it dose it faster than GetContentsBool.
			*/
			bool IsContentsValid();


			/**
			* @brief Gets the contents as a string then it finds a smaller string between scopeStart and scopeEnd and it will repeat this search going further down the file by itemIndex + 1 times.
			* @return An AssetObjectContainer whos contents should be the area between the scope or a invalid AssetObjectContainer if itemIndex or no scope of x type can be found after itemIndex searches.
			* @param[in] itemIndex: How many searched to do takeaway 1 (so 0 equals 1 search for example).
			* @param[in] scopeStart: Some text repasenting the start point of a scope which would look something like <example>.
			* @param[in] scopeEnd: Some text repasenting the end point of a scope which would look something like </example>.
			* @param[in/out] inOutSearchOffset: To access an item with an index higher than 0 the system has to find all other indexes before that one (like linked list indexing) which is slow. So This can be used to store where the last array seach ended and that can then be provided to the next array search via the same pramater making array access much faster as it no longer has to find the prior indexes.
			*/
			AssetObjectContainer GetSubObjectsByScope(unsigned int itemIndex, const char* scopeStart, const char* scopeEnd, unsigned int* inOutSearchOffset = nullptr);

			/**
			* @brief Gets the contents as a string then finds out how many smaller string there are between scopeStart and scopeEnd.
			* @return The amout of scopes there are.
			* @param[in] scopeStart: Some text repasenting the start point of a scope which would look something like <example>.
			* @param[in] scopeEnd: Some text repasenting the end point of a scope which would look something like </example>.
			*/
			unsigned int GetSubObjectsByScopeLength(const char* scopeStart, const char* scopeEnd);


			/**
			* @brief Gets the contents as a string then it finds a smaller string between scopeStart and scopeEnd but for every scopeStart detected along the way it will need to pair it to a scopeEnd and then only the first scope will be accessible making it great for tree structers with recusive elements but it is slow. it will repeat this search going further down the file by itemIndex + 1 times.
			* @return An AssetObjectContainer whos contents should be the area between the scope or a invalid AssetObjectContainer if itemIndex or no scope of x type can be found after itemIndex searches.
			* @param[in] itemIndex: How many searched to do takeaway 1 (so 0 equals 1 search for example).
			* @param[in] scopeStart: Some text repasenting the start point of a scope which would look something like <example>.
			* @param[in] scopeEnd: Some text repasenting the end point of a scope which would look something like </example>.
			* @param[in/out] inOutSearchOffset: To access an item with an index higher than 0 the system has to find all other indexes before that one (like linked list indexing) which is slow. So This can be used to store where the last array seach ended and that can then be provided to the next array search via the same pramater making array access much faster as it no longer has to find the prior indexes.
			*/
			AssetObjectContainer GetSubObjectsByScopeExpand(unsigned int itemIndex, const char* scopeStart, const char* scopeEnd, unsigned int* inOutSearchOffset = nullptr);

			/**
			* @brief Gets the contents as a string then it finds a smaller string between scopeStart and scopeEnd but for every scopeStart detected along the way it will need to pair it to a scopeEnd and then only the first scope will be accessible/a high domain element, making it great for tree structers with recusive elements but it is slow. then it will couut how many of these high domain elements there are.
			* @return A count of how many of x scopes there are.
			* @param[in] ScopeStart: some text repasenting the start point of a scope which would look something like <example>.
			* @param[in] ScopeEnd: some text repasenting the end point of a scope which would look something like </example>.
			*/
			unsigned int GetSubObjectsByScopeExpandLength(const char* scopeStart, const char* scopeEnd);


			/**
			* @brief Gets the contents as a string then it finds a smaller string between separator and separator. and it will repeat this search going further down the file by itemIndex + 1 times.
			* @return An AssetObjectContainer whos contents should be the area between the separators or a invalid AssetObjectContainer if itemIndex or no scope of x separator type can be found after itemIndex searches.
			* @param[in] itemIndex: The index of the element you are looking for <example>.
			* @param[in] separator: Some text repasenting the divider between diffrent elements and by default it is ,.
			* @param[in/out] inOutSearchOffset: To access an item with an index higher than 0 the system has to find all other indexes before that one (like linked list indexing) which is slow. So This can be used to store where the last array seach ended and that can then be provided to the next array search via the same pramater making array access much faster as it no longer has to find the prior indexes.
			*/
			AssetObjectContainer GetSubObjectByArrayElement(unsigned int itemIndex, const char* separator = ",", unsigned int* inOutSearchOffset = nullptr);

			/**
			* @brief Gets the contents as a string then it finds a smaller strings between separator and separator and counts how many of these strings it found. 
			* @return The counT of how many strings it found between the separators.
			* @param[in] separator: Some text repasenting the divider between diffrent elements and by default it is , </example>.
			*/
			unsigned int GetSubObjectByArrayElementLength(const char* separator = ",");
		};


		/*struct ASSESTPACKAGINGSOURCE_API PackItemDataLayout //header layout example
		{
			unsigned int pathLength = 0; //4-8 bytes
			char* pathBlock = nullptr; //? bytes
			unsigned int dataLength = 0; //4-8 bytes
			char* dataBlock = nullptr; //? bytes
		};*/

		/**
		* @brief if given a collection of files (and folders kinda) it can combine them all into one file (like how zile files work) but without compression and it can also access these files at fast speeds if the data has entirely been dumped in ram which is also useally required for it to access thoese files
		*/
		class ASSESTPACKAGINGSOURCE_API PackObjectContainer
		{
		private:
			char* m_packData = nullptr;
			size_t m_packDataLength = 0;
			size_t m_packDataIndexPoint = 0;

		public:
			PackObjectContainer();
			~PackObjectContainer();
			PackObjectContainer& operator=(const PackObjectContainer& t) = delete;
			PackObjectContainer(PackObjectContainer const&) = delete;
			PackObjectContainer(PackObjectContainer&& h) = default;

			//pack createing and destruction
			/**
			* @brief Creates the memory in ram for the pack so that files can be added to it or so that files can be read from it. NO READING SHOULD BE DONE HOWEVER UNTILL EndCreatePack IS EXACUTED.
			* @param[in] maxLengthOfNewPack: How much memory should the pack take up in bytes by default it should be 4.294967295 GB but it is avised that you do not use this default if you can spare to.
			*/
			void StartCreatePack(size_t maxLengthOfNewPack = SE_APS__PACK_MAX_CONTENTS_LENGTH);

			/**
			* @brief After this is exacuted NO MORE FILES SHOULD BE ADDED TO PACK and the pack can now be read from, futhermore any memory not used will be marked as inaccessible but it will be freeded when the PackObjectContainer is freed or when UnloadPack is exacuted.
			*/
			void EndCreatePack();

			/**
			* @brief Deletes the memory created for the pack in ram and and thus all contents in this pack will be lost to time.
			*/
			void UnloadPack();


			//pack size and indexing content here
			/**
			* @brief Finds out the size of the total accessible region and then returns it.
			* @return The size of the total accessible region.
			*/
			size_t GetTotalPackSize();

			/**
			* @brief Gets the index point of where we are at in the file and normally this is pointless.
			* @return The index point.
			*/
			size_t GetIndexPoint();

			/**
			* @brief Sets the index point of where we are at in the file and normally this is pointless.
			* @param[in] indexPoint: The index point in the pack.
			*/
			void SetIndexPoint(size_t indexPoint);


			//get from pack content here
			/**
			* @brief Sets the index point to 0 and then seaches for a file with X path and returns the size of that file.
			* @return The size of that file or SE_APS__PACK_ITEM_INVALID_LEN_ERROR.
			* @param[in] path: The path of the file.
			*/
			unsigned int GetFileLengthFromPack(const char* path); ///get file by path

			/**
			* @brief Sets the index point to 0 and searches for the data of a file in the pack.
			* @return A (undeletable char) to a char array of the files contents or nullptr
			* @param[in] path: The path of the file.
			*/
			char* GetFileDataFromPack(const char* path); ///get file by path


			/**
			* @brief Sets the index point to 0 and gets the data of a file in the pack by its index in the order all files were added to the pack starting from 0.
			* @return A (undeletable char) to a char array of the files contents or nullptr
			* @param[in] index: The index of the file.
			*/
			char* GetFileDataFromPackViaIndex(unsigned int index); ///get file by index

			/**
			* @brief Sets the index point to 0 and gets the file length of a file in the pack by its index in the order all files were added to the pack starting from 0.
			* @return The length of the files contents.
			* @param[in] index: The index of the file.
			*/
			unsigned int GetFileDataLengthFromPackViaIndex(unsigned int index); ///get file by index

			/**
			* @brief Gets the total amout of files in a pack.
			* @return The total.
			*/
			unsigned int GetFileCount(); ///get the total amount of files there are

			/**
			* @brief If provided with an index to a file.
			* @return It will return the file path (as an undeletable char array) of that file or nullptr.
			* @param[in] index: The index of the file.
			*/
			char* GetFilePathFromPackViaIndex(unsigned int index); //get file name by index

			/**
			* @brief If provided with an index to a file it will return the length of that files path.
			* @return It will return the the length of the file path string or SE_APS__PACK_ITEM_INVALID_LEN_ERROR
			* @param[in] index: The index of the file.
			*/
			unsigned int GetFilePathLengthFromPackViaIndex(unsigned int index); ///get file name by index


			//add to pack content here
			/**
			* @brief Ment to be used if the pack is being used to package contents this function makes it easy to add files to a pack.
			* @param[in] path: A string to the file path that can not be bigger in length than SE_APS__MAX_PACK_PATH_LENGTH.
 			* @param[in] inData: A char buffer of data that can not exceed or be equal to 4.294967295 giga bytes (if it is YOU WILL NEED TO BREAK IT DOWN).
			* @param[in] inDataLength: The length of the files data.
			*/
			void AddFileToPack(const char* path, const char* inData, unsigned int inDataLength); //adds a file to the pack


			//raw minulation of packs contents
			//raw get
			void operator>>(char& charter); ///make sure there is enougth room for all the packs contents
			void operator>>(char* charters); ///make sure there is enougth room for all the packs contents
			
			/**
			* @brief Gets the current block of data under the index point and this is useful for fast read oparations as with this you only have to find the size of the file and then you can use this to get the contents.
			* @return A (undeleteable) char array to the current block.
			*/
			char* GetBlock(); ///gets the char at IndexPoint but it returns it as a pointer to that char so it can be used with array minulators (array[1]) to get the next letter and so on so fourth

			//raw set
			void operator<<(char charter);
			void operator<<(char* charters); ///make sure the size of the array matches the size of the pack
			void operator<<(const char* charters); ///make sure the size of the array matches the size of the pack

			/**
			* @brief Insta fills a pack with the required data much faster than a operator<< can but it will not stop filling untill the pack is full to capacity which means that you do need pack start create set to the size of the provided string also it is best to use SetIndexPoint(0) before doing this just to be sure.
			* @param[in] charters: the text that will fill the pack
			*/
			void SetBlock(const char* charters); 
		};
	}
}