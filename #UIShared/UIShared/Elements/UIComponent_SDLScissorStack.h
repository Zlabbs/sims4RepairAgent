#pragma once

#include "SDL.h"

#include <stack>

namespace UIShared
{

	/**
	@brife used to make it so elements cant pop out of an area
	*/
	class UIComponent_SDLScissorStack
	{
	private:
		static std::stack<SDL_Rect> m_scissorStackCore;
		static bool m_isUnderflowing;

		void SetScissor(SDL_Renderer* render)
		{
			if (m_scissorStackCore.empty() == false)
			{
				SDL_Rect newSissorRect = m_scissorStackCore.top();
				SDL_RenderSetClipRect(render, &newSissorRect);
			}
			else
			{
				SDL_RenderSetClipRect(render, NULL);
			}
		}

	protected:

		void PushToStack(SDL_Renderer* render, SDL_Rect sissor)
		{
			SDL_Rect intersectResults = sissor;
			if (m_scissorStackCore.size() > 1 && SDL_IntersectRect(&m_scissorStackCore.top(), &sissor, &intersectResults) == false) //calulates how much of the current sissor oparation is visible in the parent
			{
				intersectResults = SDL_Rect(); //the current sissor oparation is not visible so nethier should its children be
			}

			if (m_scissorStackCore.size() < 20) { m_scissorStackCore.push(intersectResults); } //pushes the results or fails

			m_isUnderflowing = false;
			SetScissor(render); //changes the sissor location
		}

		void PopFromStack(SDL_Renderer* render)
		{
			if (m_scissorStackCore.empty() == false) //are there sissor oparation still in the stack
			{
				m_scissorStackCore.pop(); //if so pop the top one of them
			}
			else 
			{
				m_isUnderflowing = true; //to many items poped
			}

			SetScissor(render);
		}

	public:
		static void PopAll()
		{
			m_isUnderflowing = false;

			while (m_scissorStackCore.empty() == false)
			{
				m_scissorStackCore.pop();
			}
		}

		static bool IsEmpty()
		{
			return m_scissorStackCore.empty();
		}

		static SDL_Rect Top()
		{
			return m_scissorStackCore.top();
		}

		static bool IsErrorOverFlowing()
		{
			return (m_scissorStackCore.size() >= 20);
		}

		static bool IsErrorUnderFlowing()
		{
			return m_isUnderflowing;
		}
	};

	std::stack<SDL_Rect> UIComponent_SDLScissorStack::m_scissorStackCore = std::stack<SDL_Rect>();
	bool UIComponent_SDLScissorStack::m_isUnderflowing = false;
}