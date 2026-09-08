#pragma once

#include "SDL.h"

#include "UISource/UISource.h"
#include "AssestPackagingSource/AssestPackagingSource.h"

#include "UIComponent_InputData.h"

namespace UIShared
{
	class UIE_ID_Window : public stackExternal::UI::UIElement, UIComponent_InputData
	{
	private:
		bool m_inDragMode = false;
		int m_dragOffset[2] = { 0, 0 };

		bool m_inScaleWindowMode = false;
		int m_minScaleLockAxisLocation = 0;
		int m_scaleWindowDirection[2] = { 0, 0 };

		void MoveScaleToPixelOffset()
		{
			stackExternal::UI::UITransformBounds worldBounds = GetWorldTransformBounds();
			stackExternal::UI::UITransform trans = GetLocalTransform(); //gets actual size as pixles

			if (trans.percentageSize[0] == 0 && trans.percentageSize[1] == 0)
			{
				return;
			}

			trans.percentageSize[0] = 0;
			trans.percentageSize[1] = 0;
			trans.bounds.offsetSize[0] = worldBounds.offsetSize[0]; //set it to that size
			trans.bounds.offsetSize[1] = worldBounds.offsetSize[1];
			SetLocalTransform(trans);
		}

		void MoveLocationToPixelOffset()
		{	
			stackExternal::UI::UITransformBounds worldBounds = GetWorldTransformBounds();
			stackExternal::UI::UITransform trans = GetLocalTransform(); //gets actual location as pixles

			if (trans.anchorPos[0] == 0 && trans.anchorPos[1] == 0 && trans.pivotPos[0] == 0 && trans.pivotPos[0] == 0)
			{
				return;
			}

			trans.anchorPos[0] = 0;
			trans.anchorPos[1] = 0;
			trans.pivotPos[0] = 0;
			trans.pivotPos[1] = 0;
			trans.bounds.offsetPos[0] = 0; //so we can get the logation of its parent reguarless of what the parent wants
			trans.bounds.offsetPos[1] = 0;
			SetLocalTransform(trans);

			stackExternal::UI::UITransformBounds parentBounds = GetWorldTransformBounds();


			trans.bounds.offsetPos[0] = worldBounds.offsetPos[0] - parentBounds.offsetPos[0];
			trans.bounds.offsetPos[1] = worldBounds.offsetPos[1] - parentBounds.offsetPos[1];
			SetLocalTransform(trans);

		}

	protected:
		virtual void OnRemove() override
		{
			delete this;
		}

		virtual void OnRender(void* renderingContext, bool renderDebug) override
		{
			//handles dragging
			if (m_inDragMode == true) //this is how we make the window follow the mouse
			{
				stackExternal::UI::UITransformBounds worldBounds = GetWorldTransformBounds();
				stackExternal::UI::UITransform trans = GetLocalTransform();

				worldBounds.offsetPos[0] = worldBounds.offsetPos[0] - trans.bounds.offsetPos[0]; //takes use out of the relativeness from our parent
				worldBounds.offsetPos[1] = worldBounds.offsetPos[1] - trans.bounds.offsetPos[1];

				trans.bounds.offsetPos[0] = GetMousePos()[0] + (m_dragOffset[0] - worldBounds.offsetPos[0]);
				trans.bounds.offsetPos[1] = GetMousePos()[1] + (m_dragOffset[1] - worldBounds.offsetPos[1]);

				SetLocalTransform(trans);
			}

			if (m_windowFrameVisible == true)
			{
				stackExternal::UI::UITransformBounds worldBounds = GetWorldTransformBounds();
				std::array<int, 2> mousePosRaw = GetMousePos();
				SDL_Rect mousePos = SDL_Rect{mousePosRaw[0], mousePosRaw[1], 1, 1};


				if (m_inScaleWindowMode == true)
				{
					stackExternal::UI::UITransform trans = GetLocalTransform();

					if (m_scaleWindowDirection[0] == 1) //scale east
					{
						trans.bounds.offsetSize[0] = std::max(mousePos.x - worldBounds.offsetPos[0], m_minResize[0]);
					}

					if (m_scaleWindowDirection[0] == -1) //scale west
					{
						int parentOffset = worldBounds.offsetPos[0] - trans.bounds.offsetPos[0];
						int newSize = trans.bounds.offsetSize[0] + (worldBounds.offsetPos[0] - mousePos.x);

						if (newSize > m_minResize[0]) { trans.bounds.offsetPos[0] = mousePos.x - parentOffset; }
						else if (trans.bounds.offsetSize[0] > m_minResize[0]) { trans.bounds.offsetPos[0] = (mousePos.x + (newSize - m_minResize[0])) - parentOffset; }

						trans.bounds.offsetSize[0] = std::max(newSize, m_minResize[0]);
					}

					if (m_scaleWindowDirection[1] == 1) //scale north
					{
						trans.bounds.offsetSize[1] = std::max(mousePos.y - worldBounds.offsetPos[1], m_minResize[1]);
					}

					if (m_scaleWindowDirection[1] == -1) //scale south
					{
						int parentOffset = worldBounds.offsetPos[1] - trans.bounds.offsetPos[1];
						int newSize = trans.bounds.offsetSize[1] + (worldBounds.offsetPos[1] - mousePos.y);

						if (newSize > m_minResize[1]) { trans.bounds.offsetPos[1] = mousePos.y - parentOffset; }
						else if (trans.bounds.offsetSize[1] > m_minResize[1]) { trans.bounds.offsetPos[1] = (mousePos.y +(newSize - m_minResize[1])) - parentOffset; }

						trans.bounds.offsetSize[1] = std::max(newSize, m_minResize[1]);
					}


					SetLocalTransform(trans);
				}

				//boxes that collectively repasent the outline of the window and allow us to know what part of the outline you are at
				SDL_Rect resizeRectWest = SDL_Rect{ worldBounds.offsetPos[0] - 2, worldBounds.offsetPos[1], 5, worldBounds.offsetSize[1] };
				SDL_Rect resizeRectNorth = SDL_Rect{ worldBounds.offsetPos[0], worldBounds.offsetPos[1] - 2, worldBounds.offsetSize[0], 5 };
				SDL_Rect resizeRectEest = SDL_Rect{ worldBounds.offsetPos[0] + (worldBounds.offsetSize[0] - 2), worldBounds.offsetPos[1], 5, worldBounds.offsetSize[1] };
				SDL_Rect resizeRectSouth = SDL_Rect{ worldBounds.offsetPos[0], worldBounds.offsetPos[1] + (worldBounds.offsetSize[1] - 2), worldBounds.offsetSize[0], 5 };
				
				//outline checks for mouse
				if (SDL_HasIntersection(&mousePos, &resizeRectWest) == true || (m_scaleWindowDirection[0] != 0 && m_inScaleWindowMode == true)) //at west border
				{
					SetMouseCursorShapeType(((unsigned int)SDL_SystemCursor::SDL_SYSTEM_CURSOR_SIZEWE));
				}

				if (SDL_HasIntersection(&mousePos, &resizeRectNorth) == true || (m_scaleWindowDirection[1] != 0 && m_inScaleWindowMode == true)) //at north border
				{
					SetMouseCursorShapeType(((unsigned int)SDL_SystemCursor::SDL_SYSTEM_CURSOR_SIZENS));
				}

				if (SDL_HasIntersection(&mousePos, &resizeRectEest) == true) //at east border
				{
					SetMouseCursorShapeType(((unsigned int)SDL_SystemCursor::SDL_SYSTEM_CURSOR_SIZEWE));
				}

				if (SDL_HasIntersection(&mousePos, &resizeRectSouth) == true) //at south border
				{
					SetMouseCursorShapeType(((unsigned int)SDL_SystemCursor::SDL_SYSTEM_CURSOR_SIZENS));
				}
			}

			if (m_windowFrameVisible == false) { return; }

			stackExternal::UI::UITransformBounds worldBounds = GetWorldTransformBounds();

			//render window
			SDL_Rect renderRect = SDL_Rect{ worldBounds.offsetPos[0], worldBounds.offsetPos[1], worldBounds.offsetSize[0], worldBounds.offsetSize[1] };
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, 20, 20, 20, 255);
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &renderRect);
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, 80, 80, 80, 255);
			SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &renderRect);

			//render window bar
			renderRect.h = m_barHeight;
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, 10, 10, 10, 255);
			SDL_RenderFillRect((SDL_Renderer*)renderingContext, &renderRect);
			SDL_SetRenderDrawColor((SDL_Renderer*)renderingContext, 80, 80, 80, 255);
			SDL_RenderDrawRect((SDL_Renderer*)renderingContext, &renderRect);			
		}

		virtual bool OnEvent(unsigned int eventId, void* eventData) override
		{
			if (eventId == UISHAR__UICMD__SDL_EVENT && GetAttachedTree()->m_shouldRender == true) //is an event from sdl
			{
				SDL_Event* event = (SDL_Event*)eventData;

				if (event->type == SDL_MOUSEBUTTONDOWN)
				{
					stackExternal::UI::UITransformBounds worldBounds = GetWorldTransformBounds();
					SDL_Rect mousePos = SDL_Rect{ GetMousePos()[0], GetMousePos()[1], 1, 1 };

					if (m_windowFrameVisible == true) //are we resizing the window check
					{
						SDL_Rect resizeRectWest = SDL_Rect{ worldBounds.offsetPos[0] - 2, worldBounds.offsetPos[1], 5, worldBounds.offsetSize[1] };
						SDL_Rect resizeRectNorth = SDL_Rect{ worldBounds.offsetPos[0], worldBounds.offsetPos[1] - 2, worldBounds.offsetSize[0], 5 };
						SDL_Rect resizeRectEest = SDL_Rect{ worldBounds.offsetPos[0] + (worldBounds.offsetSize[0] - 2), worldBounds.offsetPos[1], 5, worldBounds.offsetSize[1] };
						SDL_Rect resizeRectSouth = SDL_Rect{ worldBounds.offsetPos[0], worldBounds.offsetPos[1] + (worldBounds.offsetSize[1] - 2), worldBounds.offsetSize[0], 5 };

						if (SDL_HasIntersection(&resizeRectWest, &mousePos) == true)
						{
							m_inScaleWindowMode = true;

							m_scaleWindowDirection[0] = -1;
							m_scaleWindowDirection[1] = 0;

							MoveLocationToPixelOffset();
							MoveScaleToPixelOffset();

							m_minScaleLockAxisLocation = worldBounds.offsetPos[0] + worldBounds.offsetSize[0];

							return true; //the event is being handled
						}

						if (SDL_HasIntersection(&resizeRectNorth, &mousePos) == true)
						{
							m_inScaleWindowMode = true;

							m_scaleWindowDirection[0] = 0;
							m_scaleWindowDirection[1] = -1;

							MoveLocationToPixelOffset();
							MoveScaleToPixelOffset();

							m_minScaleLockAxisLocation = worldBounds.offsetPos[1] + worldBounds.offsetSize[1];

							return true; //the event is being handled
						}


						if (SDL_HasIntersection(&resizeRectEest, &mousePos) == true)
						{
							m_inScaleWindowMode = true;

							m_scaleWindowDirection[0] = 1;
							m_scaleWindowDirection[1] = 0;

							MoveLocationToPixelOffset();
							MoveScaleToPixelOffset();

							return true; //the event is being handled
						}

						if (SDL_HasIntersection(&resizeRectSouth, &mousePos) == true)
						{
							m_inScaleWindowMode = true;

							m_scaleWindowDirection[0] = 0;
							m_scaleWindowDirection[1] = 1;

							MoveLocationToPixelOffset();
							MoveScaleToPixelOffset();

							return true; //the event is being handled
						}

						SDL_Rect dragRect = SDL_Rect{ worldBounds.offsetPos[0], worldBounds.offsetPos[1], worldBounds.offsetSize[0], m_barHeight };
						if (SDL_HasIntersection(&dragRect, &mousePos) == true) //could we be draging the window check
						{
							MoveLocationToPixelOffset();

							m_inDragMode = true;
							m_dragOffset[0] = dragRect.x - mousePos.x;
							m_dragOffset[1] = dragRect.y - mousePos.y;
							return true; //the event is being handled
						}


						SDL_Rect windowRect = SDL_Rect{ worldBounds.offsetPos[0], worldBounds.offsetPos[1], worldBounds.offsetSize[0], worldBounds.offsetSize[1]};
						if (SDL_HasIntersection(&dragRect, &mousePos) == true)
						{
							return true; //if the mouse is in the window but the event wasnt capatured by anything stop it from going to the next window which may be behind us 
						}
					}
				}
				else if (event->type == SDL_MOUSEBUTTONUP)
				{
					m_inScaleWindowMode = false;
					m_inDragMode = false;
				}
			}

			return false;
		}


		virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			//asset
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);
			SetTagName(asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContents(), asset.GetSubObjectsByScope(0, "<tagName>", "</tagName>").GetContentsLength());  

			//window bar height
			m_barHeight = asset.GetSubObjectsByScope(0, "<barHeight>", "</barHeight>").GetContentsInt();

			//is resizable
			m_windowFrameVisible = asset.GetSubObjectsByScope(0, "<windowFrameVisible>", "</windowFrameVisible>").GetContentsBool();

			//is visible
			m_windowFrameVisible = asset.GetSubObjectsByScope(0, "<windowFrameVisible>", "</windowFrameVisible>").GetContentsBool();

			//min resize size
			stackExternal::assets::AssetObjectContainer minResizeArray = asset.GetSubObjectsByScope(0, "<minResize>", "</minResize>");
			m_minResize[0] = minResizeArray.GetSubObjectByArrayElement(0).GetContentsInt();
			m_minResize[1] = minResizeArray.GetSubObjectByArrayElement(1).GetContentsInt();
		}

		virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength) override
		{
			stackExternal::assets::AssetObjectContainer asset = stackExternal::assets::AssetObjectContainer(loadFromBuffer, bufferLength);

			//window bar height
			stackExternal::assets::AssetObjectContainer barHeight = asset.GetSubObjectsByScope(0, "<barHeight>", "</barHeight>");
			if (barHeight.IsContentsValid() == true)
			{
				m_barHeight = barHeight.GetContentsInt();
				return;
			}

			//window is resizeable
			stackExternal::assets::AssetObjectContainer resizeable = asset.GetSubObjectsByScope(0, "<windowFrameVisible>", "</windowFrameVisible>");
			if (resizeable.IsContentsValid() == true)
			{
				m_windowFrameVisible = resizeable.GetContentsBool();
				return;
			}

			//window resize min
			stackExternal::assets::AssetObjectContainer minResizeArray = asset.GetSubObjectsByScope(0, "<minResize>", "</minResize>");
			if (minResizeArray.IsContentsValid() == true)
			{
				m_minResize[0] = minResizeArray.GetSubObjectByArrayElement(0).GetContentsInt();
				m_minResize[1] = minResizeArray.GetSubObjectByArrayElement(1).GetContentsInt();
				return;
			}
		}

	public:
		int m_barHeight = 16;
		int m_minResize[2] = { 100, 100 };
		bool m_windowFrameVisible = true;
	};
}