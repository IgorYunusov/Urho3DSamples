#pragma once
#include "../Core/Object.h"

#include "..\Container\Vector.h"
#include "..\Core\Variant.h"
#include "Macros.h"

namespace Urho3D
{
	class MenuBarUI;
	class ToolBarUI;
	class MiniToolBarUI;
	class UIElement;
	class ProjectManager;
	class ResourceCache;
	class UI;
	class Graphics;
	class ProjectWindow;
	class HierarchyWindow;
	class Scene;
	class View3D;
	class Window;
	class Node;
	class Component;
	class Resource;
	class Editor;
	class XMLFile;
	class AttributeWindowUI;
	class AttributeInspector;
	class FileSelector;
	class Camera;


	class EditorData : public Object
	{
		OBJECT(EditorData);
	public:
		EditorData(Context* context);
		virtual ~EditorData();
		static void RegisterObject(Context* context);

		///  Selection 
		void ClearSelection();


		void AddSelectedComponent(Component* comp);
		void AddSelectedNode(Node* node);
		void AddEditComponent(Component* comp);
		void AddEditNode(Node* node);
		void AddSelectedUIElement(UIElement* element);
		void AddEditUIElement(UIElement* element);

		unsigned GetNumSelectedUIElements();
		unsigned GetNumSelectedComponents();
		unsigned GetNumSelectedNodes();
		unsigned GetNumEditComponents();
		unsigned GetNumEditNodes();

		Vector<Node*>&		GetSelectedNodes();
		Vector<Component*>&	GetSelectedComponents();
		Vector<UIElement*>&	GetSelectedUIElements();
		Vector<Node*>&		GetEditNodes();
		Vector<Component*>&	GetEditComponents();
		UIElement*			GetEditUIElement();
		Node*				GetEditNode();
		unsigned int		GetNumEditableComponentsPerNode();

		void	SetEditNode(Node* node);
		void	SetEditUIElement(UIElement* element);
		void	SetSelectedNodes(Vector<Node*>& nodes);
		void	SetSelectedComponents(Vector<Component*>& comps);
		void	SetSelectedUIElements(Vector<UIElement*>& elemets);
		void	SetEditNodes(Vector<Node*>& nodes);
		void	SetEditComponents(Vector<Component*>& comps);
		void	SetEditUIElements(Vector<UIElement*>& elements);
		void	SetNumEditableComponentsPerNode(unsigned int num);

		void			SetGlobalVarNames(const String& name);
		const Variant&	GetGlobalVarNames(StringHash& name);


	protected:
		/// Selection
		Vector<Node*>		selectedNodes_;
		Vector<Component*>	selectedComponents_;
		Vector<UIElement*>	selectedUIElements_;

		UIElement*	editUIElement_;
		Node*		editNode_;

		Vector<Node*>		editNodes_;
		Vector<Component*>	editComponents_;
		Vector<UIElement*>	editUIElements_;

		unsigned int numEditableComponentsPerNode_;

		// Node or UIElement hash-to-varname reverse mapping
		VariantMap globalVarNames_;

		

	};

}