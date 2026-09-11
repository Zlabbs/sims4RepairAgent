#pragma once

#include <iostream>

/*=====================================================================================\
* Designed by Kyle Edwards.                                                            *
* Created on the 10/7/2026. Last updated 11/9/2026.                                    *
*=====================================================================================*/

#define SE_UIS__MAX_NAME_LEN 2048
#define SE_UIS__INVALID_ELEMENT ((unsigned int)(-1))

#if _WIN32 || _WIN64 || _WINDOWS || _WINDLL
	#ifdef UISOURCE_EXPORTS
	#define UISOURCE_API __declspec(dllexport)
	#else
	#define UISOURCE_API __declspec(dllimport)
	#endif
#else
#define UISOURCE_API
#endif

namespace stackExternal
{
	namespace UI
	{
		class UITreeContiner;

		/**
		* @brief A combination of posistion and size using pixel base transforming ie pos 0,0 is top left corner of the screen.
		* @param[var] offsetPos: A int array of length two.
		* @param[var] offsetSize: A int array of length two.
		*/
		struct UISOURCE_API UITransformBounds
		{
			int offsetPos[2] = { 0, 0 }; ///x, y
			int offsetSize[2] = { 0, 0 }; ///w, h
		};

		/**
		* @brief The structure which contains the local transform of any given UI object
		* @param[var] bounds: 
		* @param[var] percentageSize: Will be times by the size of the parent and then this will be added to the elements actual size.
		* @param[var] anchorPos: Will be times by the parent size and then that will be combined with the parent posistion and out offset offsetPos to give us our final location.
		* @param[var] pivotPos: Is the final location takeaway the elements size times by the pivotPos.
		*/
		struct UISOURCE_API UITransform
		{
			UITransformBounds bounds = UITransformBounds();

			float percentageSize[2] = { 0, 0 }; //child{w,h} = parent{w,h} * child{psw,psh};
			float anchorPos[2] = { 0, 0 }; //child{x,y} = parent{x,y} +(parent{w,h} * child{apx,apy})
			float pivotPos[2] = { 0, 0 }; //child{x,y} = child{x,y} -(child{w,h} * child{ppx,ppy});
		};

		/**
		* @brief The base UI element class.
		*/
		class UISOURCE_API UIElement
		{
		private:
			unsigned int m_parentId = 0;
			unsigned int* m_childrenIds = nullptr;
			unsigned int m_childrenIdsLength = 0;

			UITransform m_localTransform = UITransform();
			UITreeContiner* m_treeObject = nullptr;

			char* m_tagName = nullptr;

		protected:
			/**
			* @brief Gets exacuted when the element is added to the tree container offically.
			*/
			virtual void OnAddedToTree();

			/**
			* @brief Gets exacuted when the element is being rendered.
			* @param[in] renderingContext: The context in which the UI needs to render.
			* @param[in] renderDebugOverlay: If true then the UI should render a non interactive debug overlay if code is set up for that.
			*/
			virtual void OnRender(void* renderingContext, bool renderDebugOverlay);

			/**
			* @brief Recives any event which may be passed to the ui as  well as that events type and any data that may be related to it.
			* @return return true to say the event has been handled and dose not need to be passed on to anyone else.
			* @param[in] eventId: The event type.
			* @param[in] eventData: The event data.
			*/
			virtual bool OnEvent(unsigned int eventId, void* eventData);

			/**
			* @brief the best way to avoid memory leaks is to but delete this in here btw.
			*/
			virtual void OnRemove(); //put delete this in here


			/**
			* @brief detaches a UI element from all its connections such as the one to the tree container, the children and the parent.
			*/
			void RemoveLinks(); ///call on an elements deleteion for everything to be freeded correctly

		public:
			UIElement();
			virtual ~UIElement();
			UIElement& operator=(const UIElement& t) = delete;
			UIElement(UIElement const&) = delete;
			UIElement(UIElement&& h) = default;


			/**
			* @brief Sets the name of this element.
			* @param[in] tagName: The new name of this element which cant be bigger than SE_UIS__MAX_NAME_LEN.
			*/
			void SetTagName(const char* tagName);

			/**
			* @brief Sets the name of this element.
			* @param[in] tagName: The new name of this element.
			* @param[in] nameLength: The length of the elements name which cant be bigger than SE_UIS__MAX_NAME_LEN.
			*/
			void SetTagName(const char* tagName, unsigned int nameLength);

			/**
			* @brief Gets the name of this element. 
			* @return The name as an undeleteable char array.
			*/
			const char* GetTagName();

			/**
			* @brief Removes the name applyed to this element. 
			*/
			void RemoveTagName();


			/**
			* @brief gets the bounds a child should conform to.
			* @return a UITransformBounds object.
			* @param[in] childInQuestion: The child you want to get the bounds of.
			*/
			virtual UITransformBounds GetWorldTransformBoundsForChild(UIElement* childInQuestion);


			/**
			* @brief If provided with a char buffer of setting it will apply all thoese settings to the transform and to other systems which may need configureing.
			* @param[in] propertyContext: A void pointer to to some context the the propity system may need such as a rendering system.
			* @param[in] loadFromBuffer: The char buffer which contans the settings data.
			* @param[in] bufferLength: The length of the buffer.
			*/
			virtual void LoadPropities(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength); ///for custom elements

			/**
			* @brief If provided with a buffer to store settings it will write its settings to that file.
			* @param[in] propertyContext: A void pointer to to some context the the propity system may need such as a rendering system.
			* @param[in] saveToBuffer: The buffer to save to.
			* @param[in] bufferLengthUsed: How much of the buffers length was used and if its more than bufferLength then an error must of occored
			* @param[in] bufferLength: How big the buffer is.
			*/
			virtual void SavePropities(void* propertyContext, char* saveToBuffer, unsigned int* bufferLengthUsed, unsigned int bufferLength);  ///for custom elements

			/**
			* @brief If provided with a char buffer combined with a length and a value of any type it will apply the value to only one propity.
			* @param[in] propertyContext: A void pointer to to some context the the propity system may need such as a rendering system.
			* @param[in] loadFromBuffer: The char buffer which contans the settings data.
			* @param[in] bufferLength: The length of the buffer.
			*/
			virtual void ApplyAProperty(void* propertyContext, char* loadFromBuffer, unsigned int bufferLength); ///for custom elements


			/**
			* @brief Sets the local space transform.
			* @param[in] transform: The new local transform.
			*/
			void SetLocalTransform(UITransform transform);

			/**
			* @brief Gets the local space transform
			* @return The local space transform.
			*/
			UITransform GetLocalTransform();

			/**
			* @brief Gets our location and size in world space.
			* @return The bounds of our location and size in world space.
			*/
			UITransformBounds GetWorldTransformBounds();


			/**
			* @brief Gets the parent of this UI element.
			* @return The parent.
			*/
			UIElement* GetParent();

			/**
			* @brief Gets the parent of this UI element.
			* @return The parent tree continer id.
			*/
			unsigned int GetParentId();

			/**
			* @brief Gets a child by its index in the elements child array.
			* @return The child.
			*/
			UIElement* GetChild(unsigned int childIndex);

			/**
			* @brief Gets a child by its index in the elements child array.
			* @return The childs tree continer id.
			*/
			unsigned int GetChildId(unsigned int childIndex);

			/**
			* @brief Finds out how many children there are and returns the length
			* @return The length of how many children there are.
			*/
			unsigned int GetChildLength();


			/**
			* @brief Get the tree container this UI element is inside of.
			* @return the UITreeContiner as a pointer.
			*/
			UITreeContiner* GetAttachedTree();

			friend class UITreeContiner;
		};

		/**
		* @brief Contains an entire tree of UI elements and passes render events and normal events to them
		* @param[var] m_shouldRender: Should this continer render its UI elements.
		* @param[var] m_shouldReciveEvents: Should this container send events to its UI elements.
		* @param[var] m_idTag: Te tag this UIContainer is refured to as.
		*/
		class UISOURCE_API UITreeContiner
		{
		private:
			UIElement** m_elementsBuffer = nullptr;
			unsigned int m_elementsBufferLength = 0;

			unsigned int* m_elementStack = nullptr;
			unsigned int m_elementsStackIndex = 0;
			unsigned int m_elementsStackLength = 0;

			char* m_tagName = nullptr;

			void PushStackId(unsigned int id);
			void PopStackId();
			unsigned int TopStackId();
			unsigned int LengthOfIdStack();

			unsigned int AttachElementToTree(UIElement* element, unsigned int parentElement);

		public:
			bool m_shouldDebugRender = false;
			bool m_shouldRender = true;
			bool m_shouldReciveEvents = true;

			UITreeContiner();
			~UITreeContiner();
			UITreeContiner& operator=(const UITreeContiner& t) = delete;
			UITreeContiner(UITreeContiner const&) = delete;
			UITreeContiner(UITreeContiner&& h) = default;


			/**
			* @brief Sets the name of this element.
			* @param[in] tagName: The new name of this element.
			*/
			void SetTagName(const char* tagName);

			/**
			* @brief Sets the name of this element.
			* @param[in] tagName: The new name of this element.
			* @param[in] nameLength: The length of the elements name.
			*/
			void SetTagName(const char* tagName, unsigned int nameLength);

			/**
			* @brief Gets the name of this element.
			* @return The name as an undeleteable char array.
			*/
			const char* GetTagName();

			/**
			* @brief Removes the name applyed to this element.
			*/
			void RemoveTagName();


			/**
			* @brief Adds a element to the tree then pushes it to the stack and from there all new added elements will be attached to it untill it is poped.
			* @param[in] element: The element being added to the tree and stack.
			*/
			void PushElementDomain(UIElement* element);

			/**
			* @brief Pops an element from the stack ads from there all new elements will be added to the parent of the poped element.
			*/
			void PopElementDomain();

			/**
			* @brief Adds an element but one this is not capable of having children as it will not be added to the stack.
			* @param[in] element: The UI element being addded.
			*/
			void AddElement(UIElement* element);

			/**
			* @brief removes all elements in a tree.
			*/
			void UnrootTree();

			/**
			* @brief removes the element stack which you should not do untill all the UI elements have been added.
			*/
			void FlushElementDomainStack();

			/**
			* @brief When an element is added it also gets stored in an array and tree accessibe structure and this allows you to get an element by its array index.
			* @return A ui element or nullptr
			* @param[in] elementId: The UI element you want.
			*/
			UIElement* GetElement(unsigned int elementId);

			/**
			* @brief finds the first element added to the tree with that tag name and returns it.
			* @return A ui element or nullptr
			*/
			UIElement* GetElementByTagName(const char* tagName);

			/**
			* @brief finds the first element added to the tree with that tag name and returns it.
			* @return A ui element or nullptr.
			*/
			unsigned int GetElementIdByTagName(const char* tagName);

			/**
			* @brief Gets the total number of elements in the ui tree.
			* @return The total length.
			*/
			unsigned int GetElementLength();


			/**
			* @brief Renders the ui as a tree which can tend to be a bit slow and genrally the RenderTreeAsList will render the same result but maybe faster.
			* @param[in] renderingContext: Use to provide the UI with what it needs to render anything.
			* @param[in] rootNodeIndex: the index of the root node which should be 0.
			*/
			void RenderTreeAsTree(void* renderingContext, unsigned int rootNodeIndex = 0);

			/**
			* @brief renderes the tree as a list.
			* @param[in] renderingContext: Use to provide the UI with what it needs to render anything.
			*/
			void RenderTreeAsList(void* renderingContext);


			/**
			* @brief passes an event to the tree and the tree will process and pass it to elements in a way thats similer to depth first search processing.
			* @return The element id of the element that handled the event or if the event went unhandled or it wasnt allowed to be exacuted then it will return SE_UIS__INVALID_ELEMENT insted.
			* @param[in] eventId: The id of the event
			* @param[in] eventData: The data to go along with the event.
			* @param[in] rootNodeIndex: The element id of the node we are starting the transmission of this event from.
			*/
			unsigned int PassEventAsTree(unsigned int eventId, void* eventData, unsigned int rootNodeIndex = 0);

			/**
			* @brief will loop thougth every item as if we are looping thougth a list and it will past the event to the elements that way.
			* @return The element id of the element that handled the event or if the event went unhandled or it wasnt allowed to be exacuted then it will return SE_UIS__INVALID_ELEMENT insted.
			* @param[in] eventId: The id of the event
			* @param[in] eventData: The data to go along with the event.
			*/
			unsigned int PassEventAsList(unsigned int eventId, void* eventData);

			/**
			* @brief will get the z base render order of all the ui elements and send the events to the elements base on there height (useful for if you have buttons that overlap).
			* @return The element id of the element that handled the event or if the event went unhandled or it wasnt allowed to be exacuted then it will return SE_UIS__INVALID_ELEMENT insted.
			* @param[in] eventId: The id of the event
			* @param[in] eventData: The data to go along with the event.
			*/
			unsigned int PassEventAsHeightList(unsigned int eventId, void* eventData);

			/**
			* @brief Will pass the event to every element in the tree using the tree sturcter but this event can not be handled.
			* @param[in] eventId: The id of the event
			* @param[in] eventData: The data to go along with the event.
			* @param[in] rootNodeIndex: The element id of the node we are starting the transmission of this event from.
			*/
			void PassEventAsTreeUnhandleable(unsigned int eventId, void* eventData, unsigned int rootNodeIndex = 0);

			/**
			* @brief Will pass the event to every element in the tree using the tree sturcter but this event can not be handled.
			* @param[in] eventId: The id of the event
			* @param[in] eventData: The data to go along with the event.
			*/
			void PassEventAsListUnhandleable(unsigned int eventId, void* eventData);

		};
	}
}
