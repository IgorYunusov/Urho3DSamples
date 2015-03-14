

#include "../Urho3D.h"
#include "../Core/Context.h"
#include "EditorData.h"
#include "../Core/CoreEvents.h"
#include "../UI/DropDownList.h"
#include "../Engine/EngineEvents.h"
#include "../UI/Font.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/GraphicsEvents.h"
#include "../Input/Input.h"
#include "../Input/InputEvents.h"
#include "../IO/IOEvents.h"
#include "../UI/LineEdit.h"
#include "../UI/ListView.h"
#include "../IO/Log.h"
#include "../Resource/ResourceCache.h"
#include "../UI/ScrollBar.h"
#include "../UI/Text.h"
#include "../UI/UI.h"
#include "../UI/UIEvents.h"
#include "../Scene/Scene.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Texture2D.h"

#include "UIGlobals.h"
#include "MenuBarUI.h"
#include "ToolBarUI.h"
#include "MiniToolBarUI.h"
#include "HierarchyWindow.h"
#include "AttributeInspector.h"
#include "ResourcePicker.h"

namespace Urho3D
{

	void EditorData::RegisterObject(Context* context)
	{
		context->RegisterFactory<EditorData>();
	}

	EditorData::EditorData(Context* context) : Object(context),
		editUIElement_(NULL),
		editNode_(NULL),
		numEditableComponentsPerNode_(1)
	{

	}

	EditorData::~EditorData()
	{

	}

	void EditorData::ClearSelection()
	{
		selectedNodes_.Clear();
		selectedComponents_.Clear();
		selectedUIElements_.Clear();

		editUIElement_ = NULL;
		editNode_ = NULL;

		editNodes_.Clear();
		editComponents_.Clear();
		editUIElements_.Clear();

		numEditableComponentsPerNode_ = 1;
	}



	void EditorData::AddSelectedComponent(Component* comp)
	{
		if (comp != NULL)
			selectedComponents_.Push(comp);
	}

	void EditorData::AddSelectedNode(Node* node)
	{
		if (node != NULL)
			selectedNodes_.Push(node);
	}

	void EditorData::AddEditComponent(Component* comp)
	{
		if (comp != NULL)
			editComponents_.Push(comp);
	}

	void EditorData::AddEditNode(Node* node)
	{
		if (node != NULL)
			editNodes_.Push(node);
	}



	void EditorData::AddSelectedUIElement(UIElement* element)
	{
		if (element != NULL)
			selectedUIElements_.Push(element);
	}

	unsigned EditorData::GetNumSelectedUIElements()
	{
		return selectedUIElements_.Size();
	}

	unsigned EditorData::GetNumSelectedComponents()
	{
		return selectedComponents_.Size();
	}

	unsigned EditorData::GetNumSelectedNodes()
	{
		return selectedNodes_.Size();
	}

	unsigned EditorData::GetNumEditComponents()
	{
		return editComponents_.Size();
	}

	unsigned EditorData::GetNumEditNodes()
	{
		return editNodes_.Size();
	}

	Node* EditorData::GetEditNode()
	{
		return editNode_;
	}

	void EditorData::SetEditNode(Node* node)
	{
		editNode_ = node;
	}

	void EditorData::SetEditUIElement(UIElement* element)
	{
		editUIElement_ = element;
	}

	UIElement* EditorData::GetEditUIElement()
	{
		return editUIElement_;
	}

	void EditorData::SetNumEditableComponentsPerNode(unsigned int num)
	{
		numEditableComponentsPerNode_ = num;
	}

	unsigned int EditorData::GetNumEditableComponentsPerNode()
	{
		return numEditableComponentsPerNode_;
	}

	void EditorData::AddEditUIElement(UIElement* element)
	{
		if (element != NULL)
			editUIElements_.Push(element);
	}


	void EditorData::SetSelectedNodes(Vector<Node*>& nodes)
	{
		selectedNodes_ = nodes;
	}

	void EditorData::SetSelectedComponents(Vector<Component*>& comps)
	{
		selectedComponents_ = comps;
	}

	void EditorData::SetSelectedUIElements(Vector<UIElement*>& elemets)
	{
		selectedUIElements_ = elemets;
	}

	void EditorData::SetEditNodes(Vector<Node*>& nodes)
	{
		editNodes_ = nodes;
	}

	void EditorData::SetEditComponents(Vector<Component*>& comps)
	{
		editComponents_ = comps;
	}

	void EditorData::SetEditUIElements(Vector<UIElement*>& elements)
	{
		editUIElements_ = elements;
	}

	Vector<Node*>& EditorData::GetSelectedNodes()
	{
		return selectedNodes_;
	}

	Vector<Component*>& EditorData::GetSelectedComponents()
	{
		return selectedComponents_;
	}

	Vector<UIElement*>& EditorData::GetSelectedUIElements()
	{
		return selectedUIElements_;
	}

	Vector<Node*>& EditorData::GetEditNodes()
	{
		return editNodes_;
	}

	Vector<Component*>& EditorData::GetEditComponents()
	{
		return editComponents_;
	}

	void EditorData::SetGlobalVarNames(const String& name)
	{
		globalVarNames_[name] = name;
	}

	const Variant& EditorData::GetGlobalVarNames(StringHash& name)
	{
		return globalVarNames_[name];
	}

}