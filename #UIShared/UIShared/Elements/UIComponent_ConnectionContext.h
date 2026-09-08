#pragma once

#include <string>

namespace UIShared
{
	/**
	@brife connects the ui to the wider game/editor
	*/
	class UIComponent_ConnectionContext
	{
	private:
		static bool(*m_isInPack)(std::string);
		static void(*m_getPackData)(std::string, char**, unsigned int*);
		static void* (*m_compileScript)(std::string*);
		static void(*m_runSript)(void*, std::string, std::string, void*, void*, void*);
		static void(*m_freeScript)(void*);
		static std::string(*m_conVarInteraction)(std::string, std::string, bool);

	protected:
		bool IsItemInPack(std::string item)
		{
			if (m_isInPack != nullptr)
			{
				return m_isInPack(item);
			}

			return false;
		}

		void GetItemFromPack(std::string item, char** fileDataOut, unsigned int* fileLengthOut)
		{
			if (m_getPackData != nullptr)
			{
				m_getPackData(item, fileDataOut, fileLengthOut);
			}
		}


		void* CompileScript(std::string* scriptData)
		{
			if (m_compileScript != nullptr)
			{
				return m_compileScript(scriptData);
			}

			return nullptr;
		}

		void RunSript(void* script, std::string eventName, std::string customValue, void* self, void* parent, void* root)
		{
			if (m_runSript != nullptr)
			{
				m_runSript(script, eventName, customValue, self, parent, root);
			}
		}

		void FreeScript(void* script)
		{
			if (m_freeScript != nullptr)
			{
				m_freeScript(script);
			}
		}

		void SetConVar(std::string var, std::string value)
		{
			if (m_conVarInteraction != nullptr)
			{
				m_conVarInteraction(var, value, true); //true equals setting
			}
		}

		std::string GetConVar(std::string var)
		{
			if (m_conVarInteraction != nullptr)
			{
				return m_conVarInteraction(var, "", false); //true equals setting
			}

			return "CC_FAILURE";
		}


	public:
		static void InitalizeConnectionContext(bool(*isInPack)(std::string), void(*getPackData)(std::string, char**, unsigned int*), void* (*compileScript)(std::string*), void(*runSript)(void*, std::string, std::string, void*, void*, void*), void(*freeScript)(void*), std::string(*conVarInteraction)(std::string, std::string, bool))
		{
			m_isInPack = isInPack;
			m_getPackData = getPackData;
			m_compileScript = compileScript;
			m_runSript = runSript;
			m_freeScript = freeScript;
			m_conVarInteraction = conVarInteraction;
		}
	};

	bool(*UIComponent_ConnectionContext::m_isInPack)(std::string) = nullptr;
	void(*UIComponent_ConnectionContext::m_getPackData)(std::string, char**, unsigned int*) = nullptr;
	void* (*UIComponent_ConnectionContext::m_compileScript)(std::string*) = nullptr;
	void(*UIComponent_ConnectionContext::m_runSript)(void*, std::string, std::string, void*, void*, void*) = nullptr;
	void(*UIComponent_ConnectionContext::m_freeScript)(void*) = nullptr;
	std::string(*UIComponent_ConnectionContext::m_conVarInteraction)(std::string, std::string, bool) = nullptr;
}