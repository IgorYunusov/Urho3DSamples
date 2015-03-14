#include "../Urho3D.h"
#include "../Core/Context.h"
#include "InGameEditor.h"
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
#include "../Scene/Component.h"
#include "../Scene/Node.h"
#include "../UI/UIElement.h"
#include "UIGlobals.h"
#include "MenuBarUI.h"
#include "ToolBarUI.h"
#include "MiniToolBarUI.h"
#include "HierarchyWindow.h"
#include "AttributeInspector.h"
#include "ResourcePicker.h"
#include "EditorData.h"
#include "../Core/Variant.h"
#include "UIUtils.h"



namespace Urho3D
{

	InGameEditor::InGameEditor(Context* context) : Object(context)
	{

		editorData_ = new EditorData(context_);

		/// ResourcePickerManager is needed for the Attribute Inspector, so don't forget to init it
		context_->RegisterSubsystem(new ResourcePickerManager(context_));
		ResourcePickerManager* resPickerMng = GetSubsystem<ResourcePickerManager>();
		resPickerMng->Init();

		/// cache some Subsystems 
		cache_ = GetSubsystem<ResourceCache>();
		ui_ = GetSubsystem<UI>();
		graphics_ = GetSubsystem<Graphics>();

		/// load default style files, can be edited afterwards 
		defaultStyle_ = cache_->GetResource<XMLFile>("UI/IDEStyle.xml");
		iconStyle_ = cache_->GetResource<XMLFile>("UI/IDEIcons.xml");

		/// create main ui element 
		rootUI_ = ui_->GetRoot()->CreateChild<UIElement>("InGameEditor");
		rootUI_->SetSize(ui_->GetRoot()->GetSize());
		rootUI_->SetTraversalMode(TM_DEPTH_FIRST);     // This is needed for root-like element to prevent artifacts		

		rootUI_->SetPriority(100);
		rootUI_->SetDefaultStyle(defaultStyle_);
		rootUI_->SetOpacity(0.65f);
		//////////////////////////////////////////////////////////////////////////
		/// create menu bar with default menu entries
		menubar_ = MenuBarUI::Create(rootUI_, "EditorMenuBar");
		menubar_->CreateMenu("File");
		menubar_->CreateMenuItem("File", "Exit Editor", A_QUITEDITOR_VAR);
		menubar_->CreateMenu("Windows");
		menubar_->CreateMenuItem("Windows", "Attribute", A_SHOWATTRIBUTE_VAR);
		menubar_->CreateMenuItem("Windows", "Hierarchy", A_SHOWHIERARCHY_VAR);

		SubscribeToEvent(menubar_, E_MENUBAR_ACTION, HANDLER(InGameEditor, HandleMenuBarAction));

		//////////////////////////////////////////////////////////////////////////
		/// create the hierarchy editor
		hierarchyWindow_ = rootUI_->CreateChild<HierarchyWindow>("SceneHierarchyWindow");
		hierarchyWindow_->SetSize(200, 200);
		hierarchyWindow_->SetDefaultStyle(defaultStyle_);
		hierarchyWindow_->SetStyleAuto();
		hierarchyWindow_->SetMovable(true);
		hierarchyWindow_->SetIconStyle(iconStyle_);
		hierarchyWindow_->SetTitle("Scene Hierarchy");
		hierarchyWindow_->SetPosition(0, menubar_->GetHeight());
		/// \todo
		// dont know why the auto style does not work ... 
		hierarchyWindow_->SetTexture(cache_->GetResource<Texture2D>("Textures/UI.png"));
		hierarchyWindow_->SetImageRect(IntRect(48, 0, 64, 16));
		hierarchyWindow_->SetBorder(IntRect(4, 4, 4, 4));
		hierarchyWindow_->SetResizeBorder(IntRect(8, 8, 8, 8));

		SubscribeToEvent(hierarchyWindow_->GetHierarchyList(), E_SELECTIONCHANGED, HANDLER(InGameEditor, HandleHierarchyListSelectionChange));

		//////////////////////////////////////////////////////////////////////////
		/// create the attribute editor
		attributeInspector_ = new AttributeInspector(context_);
		UIElement* attrinsp = attributeInspector_->Create();
		rootUI_->AddChild(attrinsp);

		attrinsp->SetHeight(10 * ATTR_HEIGHT + 58);
		attrinsp->SetWidth(ATTRNAME_WIDTH * 2);
		attrinsp->SetPosition(200, menubar_->GetHeight());




		rootUI_->SetVisible(false);
	}

	InGameEditor::~InGameEditor()
	{
		rootUI_->Remove();
	}

	void InGameEditor::RegisterObject(Context* context)
	{
		context->RegisterFactory<InGameEditor>();
	}

	void InGameEditor::UpdateElements()
	{

	}

	void InGameEditor::SetDefaultStyle(XMLFile* style)
	{
		if (!style)
			return;
		if (defaultStyle_ == style)
			return;
		defaultStyle_ = style;
		rootUI_->SetDefaultStyle(style);

		UpdateElements();
	}

	void InGameEditor::SetVisible(bool enable)
	{
		if (enable != rootUI_->IsVisible())
		{
			Input* input = GetSubsystem<Input>();
			rootUI_->SetVisible(enable);

			if (enable)
			{
				// Show OS mouse
				input->SetMouseVisible(true, true);

				using namespace StartInGameEditor;
				VariantMap& newEventData = GetEventDataMap();
				SendEvent(E_START_INGAMEEDITOR_, newEventData);

			}
			else
			{

				// Restore OS mouse visibility
				input->ResetMouseVisible();
				ui_->SetFocusElement(NULL);

				using namespace QuitInGameEditor;
				VariantMap& newEventData = GetEventDataMap();
				SendEvent(E_QUIT_INGAMEEDITOR_, newEventData);
			}
		}

	}

	void InGameEditor::Toggle()
	{
		SetVisible(!IsVisible());
	}

	void InGameEditor::UpdateAttributeInspector()
	{
		if (attributeInspector_)
			if (attributeInspector_->GetAttributewindow()->IsVisible())
			{

				attributeInspector_->GetEditNodes() = editorData_->GetEditNodes();
				attributeInspector_->GetEditComponents() = editorData_->GetEditComponents();
				attributeInspector_->Update();
			}
				
	}

	XMLFile* InGameEditor::GetDefaultStyle() const
	{
		return defaultStyle_;
	}

	bool InGameEditor::IsVisible() const
	{
		return rootUI_->IsVisible();
	}

	Scene* InGameEditor::GetScene()
	{
		return scene_;
	}

	UIElement* InGameEditor::GetSceneUI()
	{
		return sceneUI_;
	}

	void InGameEditor::HandleMenuBarAction(StringHash eventType, VariantMap& eventData)
	{
		using namespace MenuBarAction;

		StringHash action = eventData[P_ACTION].GetStringHash();
		if (action == A_QUITEDITOR_VAR)
		{
			SetVisible(false);
		}
		else if (action == A_SHOWHIERARCHY_VAR)
		{
			hierarchyWindow_->SetVisible(true);
			ui_->SetFocusElement(NULL);
		}
		else if (action == A_SHOWATTRIBUTE_VAR)
		{
			if (!attributeInspector_->GetAttributewindow()->IsVisible())
			{
				attributeInspector_->GetAttributewindow()->SetVisible(true);
				attributeInspector_->GetEditNodes() = editorData_->GetEditNodes();
				attributeInspector_->GetEditComponents() = editorData_->GetEditComponents();
				attributeInspector_->Update();
			}
			ui_->SetFocusElement(NULL);
		}
	}

	void InGameEditor::HandleHierarchyListSelectionChange(StringHash eventType, VariantMap& eventData)
	{
		ListView* hierarchyList = hierarchyWindow_->GetHierarchyList();
		const PODVector<UIElement*>& items = hierarchyList->GetItems(); 
		const PODVector<unsigned>& indices = hierarchyList->GetSelections();

		editorData_->ClearSelection();

		for (unsigned int i = 0; i < indices.Size(); ++i)
		{
			unsigned int index = indices[i];
			UIElement* item = items[index];
			int type = item->GetVar(TYPE_VAR).GetInt();
			if (type == ITEM_COMPONENT)
			{
				Component* comp = GetListComponent(item);

				editorData_->AddSelectedComponent(comp);
			}
			else if (type == ITEM_NODE)
			{
				Node* node = GetListNode(item);

				editorData_->AddSelectedNode(node);
			}
			else if (type == ITEM_UI_ELEMENT)
			{
				UIElement* element = GetListUIElement(item);
				editorData_->AddSelectedUIElement(element);
			}
		}

		// If only one node/UIElement selected, use it for editing
		if (editorData_->GetNumSelectedNodes() == 1)
			editorData_->SetEditNode(editorData_->GetSelectedNodes()[0]);
		if (editorData_->GetNumSelectedUIElements() == 1)
			editorData_->SetEditUIElement(editorData_->GetSelectedUIElements()[0]);

		// If selection contains only components, and they have a common node, use it for editing
		if (editorData_->GetSelectedNodes().Empty() && !editorData_->GetSelectedComponents().Empty())
		{
			Node* commonNode = NULL;
			for (unsigned int i = 0; i < editorData_->GetNumSelectedComponents(); ++i)
			{
				if (i == 0)
					commonNode = editorData_->GetSelectedComponents()[i]->GetNode();
				else
				{
					if (editorData_->GetSelectedComponents()[i]->GetNode() != commonNode)
						commonNode = NULL;
				}
			}
			editorData_->SetEditNode(commonNode);
		}

		// Now check if the component(s) can be edited. If many selected, must have same type or have same edit node
		if (!editorData_->GetSelectedComponents().Empty())
		{
			if (editorData_->GetEditNode() == NULL)
			{
				StringHash compType = editorData_->GetSelectedComponents()[0]->GetType();
				bool sameType = true;
				for (unsigned int i = 1; i < editorData_->GetNumSelectedComponents(); ++i)
				{
					if (editorData_->GetSelectedComponents()[i]->GetType() != compType)
					{
						sameType = false;
						break;
					}
				}
				if (sameType)
					editorData_->SetEditComponents(editorData_->GetSelectedComponents());
			}
			else
			{
				editorData_->SetEditComponents(editorData_->GetSelectedComponents());
				editorData_->SetNumEditableComponentsPerNode(editorData_->GetNumSelectedComponents());
			}
		}

		// If just nodes selected, and no components, show as many matching components for editing as possible
		if (!editorData_->GetSelectedNodes().Empty() && editorData_->GetSelectedComponents().Empty() && editorData_->GetSelectedNodes()[0]->GetNumComponents() > 0)
		{
			unsigned int count = 0;
			for (unsigned int j = 0; j < editorData_->GetSelectedNodes()[0]->GetNumComponents(); ++j)
			{
				StringHash compType = editorData_->GetSelectedNodes()[0]->GetComponents()[j]->GetType();
				bool sameType = true;
				for (unsigned int i = 1; i < editorData_->GetNumSelectedNodes(); ++i)
				{
					if (editorData_->GetSelectedNodes()[i]->GetNumComponents() <= j || editorData_->GetSelectedNodes()[i]->GetComponents()[j]->GetType() != compType)
					{
						sameType = false;
						break;
					}
				}

				if (sameType)
				{
					++count;
					for (unsigned int i = 0; i < editorData_->GetNumSelectedNodes(); ++i)
						editorData_->AddEditComponent(editorData_->GetSelectedNodes()[i]->GetComponents()[j]);
				}
			}
			if (count > 1)
				editorData_->SetNumEditableComponentsPerNode(count);
		}

		if (editorData_->GetSelectedNodes().Empty() && editorData_->GetEditNode() != NULL)
			editorData_->AddEditNode(editorData_->GetEditNode());
		else
		{
			editorData_->SetEditNodes(editorData_->GetSelectedNodes());

			// Cannot multi-edit on scene and node(s) together as scene and node do not share identical attributes,
			// editing via gizmo does not make too much sense either
			if (editorData_->GetEditNodes().Size() > 1 && editorData_->GetEditNodes()[0] == scene_)
				editorData_->GetEditNodes().Erase(0);
		}

		if (editorData_->GetSelectedUIElements().Empty() && editorData_->GetEditUIElement() != NULL)
			editorData_->AddEditUIElement(editorData_->GetEditUIElement());
		else
			editorData_->SetEditUIElements(editorData_->GetSelectedUIElements());

	//	OnSelectionChange();

		// 		PositionGizmo();
		UpdateAttributeInspector();
		// 		UpdateCameraPreview();
	}

	Component* InGameEditor::GetListComponent(UIElement* item)
	{
		if (scene_.Null())
			return NULL;

		if (item == NULL)
			return NULL;

		if (item->GetVar(TYPE_VAR).GetInt() != ITEM_COMPONENT)
			return NULL;

		return scene_->GetComponent(item->GetVar(COMPONENT_ID_VAR).GetUInt());
	}

	Node* InGameEditor::GetListNode(UIElement* item)
	{
		if (scene_.Null())
			return NULL;

		if (item == NULL)
			return NULL;

		if (item->GetVar(TYPE_VAR).GetInt() != ITEM_NODE)
			return NULL;

		return scene_->GetNode(item->GetVar(NODE_ID_VAR).GetUInt());
	}

	UIElement* InGameEditor::GetListUIElement(UIElement* item)
	{
		if (sceneUI_.Null())
			return NULL;

		if (item == NULL)
			return NULL;

		// Get the text item's ID and use it to retrieve the actual UIElement the text item is associated to
		return GetUIElementByID(UIUtils::GetUIElementID(item));
	}

	UIElement* InGameEditor::GetUIElementByID(const Variant& id)
	{
		return id == UI_ELEMENT_BASE_ID ? NULL : sceneUI_->GetChild(UI_ELEMENT_ID_VAR, id, true);
	}

	void InGameEditor::SetScene(Scene* scene)
	{
		if (scene_ != scene)
		{

			scene_ = scene;
			hierarchyWindow_->SetScene(scene_);
		}
		

	}

	void InGameEditor::SetSceneUI(UIElement* sceneUI)
	{
		sceneUI_ = sceneUI;
	}

	void RegisterInGameEditor(Context* context)
	{



		ResourcePickerManager::RegisterObject(context);
		EditorData::RegisterObject( context);
		InGameEditor::RegisterObject(context);
		MenuBarUI::RegisterObject(context);
		ToolBarUI::RegisterObject(context);
		MiniToolBarUI::RegisterObject(context);
	}

}

