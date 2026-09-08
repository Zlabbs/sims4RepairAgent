#pragma once

#include <array>

#define UISHAR__UICMD__RESET 113355
#define UISHAR__UICMD__SDL_EVENT 224466
#define UISHAR__UICMD__UNDEFINED_TYPE ((unsigned int)(-1))
#define UISHAR__UICMD__MOUSETOKEYINDEX ((unsigned int)(SDL_KeyCode::SDLK_AC_SEARCH))
#define UISHAR__UICMD__MOUSETOKEYINDEX_MAX ((unsigned int)(SDL_KeyCode::SDLK_AC_BOOKMARKS))

namespace UIShared
{

	/**
	@brife tells the UI where the mouse is
	*/
	class UIComponent_InputData
	{
	private:
		static std::array<int, 2> m_mousePos;
		static bool m_capsDown;
		static bool m_shiftDown;
		static unsigned int m_mouseCursorShapeType;

	protected:
		std::array<int,2> GetMousePos()
		{
			return m_mousePos;
		}

		bool GetIsCapsDown()
		{
			return m_capsDown;
		}

		bool GetIsShiftDown()
		{
			return m_shiftDown;
		}

		void SetMouseCursorShapeType(unsigned int shape = UISHAR__UICMD__UNDEFINED_TYPE) //unsigned int 
		{
			m_mouseCursorShapeType = shape;
		}

	public:
		static void FeedMouseData(std::array<int, 2> pos)
		{
			m_mousePos = pos;
		}

		static void FeedKeyData(bool caps, bool shift)
		{
			m_capsDown = caps;
			m_shiftDown = shift;
		}

		static unsigned int GetMouseShapeType()
		{
			unsigned int temp = m_mouseCursorShapeType;
			m_mouseCursorShapeType = UISHAR__UICMD__UNDEFINED_TYPE;

			return temp;
		}
	};

	std::array<int, 2> UIComponent_InputData::m_mousePos = std::array<int, 2>();
	bool UIComponent_InputData::m_capsDown = false;
	bool UIComponent_InputData::m_shiftDown = false;
	unsigned int UIComponent_InputData::m_mouseCursorShapeType = UISHAR__UICMD__UNDEFINED_TYPE;
}

