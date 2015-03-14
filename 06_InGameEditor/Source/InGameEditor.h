#pragma once



#include "../Core/Object.h"
#include "Macros.h"

namespace Urho3D
{

	EVENT(E_QUIT_INGAMEEDITOR_, QuitInGameEditor)
	{
	
	}
	EVENT(E_START_INGAMEEDITOR_, StartInGameEditor)
	{

	}
	class Node;
	class Component;

	class Button;
	class BorderImage;
	class DropDownList;
	class Engine;
	class Font;
	class LineEdit;
	class ListView;
	class Text;
	class UIElement;
	class XMLFile;
	class Scene;
	class MenuBarUI;
	class ToolBarUI;
	class MiniToolBarUI;

	class ResourceCache;
	class UI;
	class Graphics;
	class HierarchyWindow;
	class AttributeInspector;
	class EditorData;

	class InGameEditor : public Object
	{
		OBJECT(InGameEditor);
	public:
		/// Construct.
		InGameEditor(Context* context);
		/// Destruct.
		virtual ~InGameEditor();
		/// Register object factory.
		static void RegisterObject(Context* context);

		/// Update elements to layout properly. Call this after manually adjusting the sub-elements.
		void UpdateElements();
		/// Toggle visibility.
		void Toggle();
		/// Update Attribute Inspector manually.
		void UpdateAttributeInspector();

		/// Set UI elements' style from an XML file.
		void SetDefaultStyle(XMLFile* style);
		/// Show or hide.
		void SetVisible(bool enable);
		/// Set the scene to be edited.
		void SetScene(Scene* scene);
		/// Set the scene to be edited.
		void SetSceneUI(UIElement* sceneUI);

		/// Return the UI style file.
		XMLFile* GetDefaultStyle() const;
		/// Return whether is visible.
		bool IsVisible() const;
		/// Return the edited scene.
		Scene* GetScene();
		/// Return the edited scene.
		UIElement* GetSceneUI();

	protected:
		void HandleMenuBarAction(StringHash eventType, VariantMap& eventData);
		void HandleHierarchyListSelectionChange(StringHash eventType, VariantMap& eventData);

		Component*	GetListComponent(UIElement* item);
		Node*		GetListNode(UIElement* item);
		UIElement*	GetListUIElement(UIElement*  item);
		UIElement*	GetUIElementByID(const Variant& id);

		ResourceCache* cache_;
		UI* ui_;
		Graphics* graphics_;

		SharedPtr<UIElement>	rootUI_;
		SharedPtr<Scene>		scene_;
		SharedPtr<UIElement>	sceneUI_;
		SharedPtr<EditorData> editorData_;

		SharedPtr<MenuBarUI>	 menubar_;
		SharedPtr<ToolBarUI>	 toolbar_;
		SharedPtr<MiniToolBarUI> minitoolbar_;
		SharedPtr<HierarchyWindow> hierarchyWindow_;
		SharedPtr<AttributeInspector> attributeInspector_;
		SharedPtr<XMLFile>		 defaultStyle_;
		SharedPtr<XMLFile>		 iconStyle_;
		
		// Serializable Attributes

	};


	void RegisterInGameEditor(Context* context);
}

