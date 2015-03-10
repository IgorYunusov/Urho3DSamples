//
// Copyright (c) 2008-2015 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "../Urho3D.h"
#include "Sample.h"

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/UI/Cursor.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Resource/XMLFile.h>
#include "../UI/UIElement.h"
#include "../Graphics/Zone.h"
#include "../Math/Color.h"
#include "../UI/Window.h"
#include "../IO/Log.h"
#include "../UI/Font.h"
#include "../UI/Button.h"
#include "../UI/UIEvents.h"
#include "../UI/ListView.h"
#include "../UI/FileSelector.h"
#include "../Graphics/Graphics.h"
#include "../Scene/Serializable.h"
#include "../Container/Vector.h"
#include "../Scene/Node.h"
#include "../Graphics/StaticModel.h"
#include "../Graphics/Model.h"
#include "../Graphics/Material.h"
#include "../Graphics/Octree.h"

#include "AttributeVariable.h"
#include "AttributeContainer.h"

#include "ResourcePicker.h"
#include "UIGlobals.h"

#include "AttributeInspector.h"
#include "../DebugNew.h"







using namespace Urho3D;
DEFINE_APPLICATION_MAIN(Sample)
namespace Urho3D
{
	Sample::Sample(Context * context) : Application(context)
	{
		cache_ = NULL;
		ui_ = NULL;
		console_ = NULL;
		debugHud_ = NULL;
		graphics_ = NULL;
	}


	void Sample::Setup()
	{		
		// Modify engine startup parameters
		engineParameters_["WindowTitle"] = GetTypeName();
		engineParameters_["LogName"] = GetTypeName() + ".log";//GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")
		engineParameters_["FullScreen"] = false;
		engineParameters_["Headless"] = false;
		engineParameters_["ResourcePaths"] = "Data;CoreData;GameData";
		RegisterAttributeUIs( context_);

	}

	void Sample::Start()
	{
		cache_ = GetSubsystem<ResourceCache>();
		ui_ = GetSubsystem<UI>();
		graphics_ = GetSubsystem<Graphics>();
		
		/// ResourcePickerManager is needed for the Attribute Inspector, so don't forget to init it
		context_->RegisterSubsystem(new ResourcePickerManager(context_));
		ResourcePickerManager* resPickerMng = GetSubsystem<ResourcePickerManager>();
		resPickerMng->Init();

		// Get default styles
		XMLFile* styleFile = cache_->GetResource<XMLFile>("UI/IDEStyle.xml");
		XMLFile* iconstyle = cache_->GetResource<XMLFile>("UI/EditorIcons.xml");

		ui_->GetRoot()->SetDefaultStyle(styleFile);

		//////////////////////////////////////////////////////////////////////////
		/// Create console
		console_ = engine_->CreateConsole();
		console_->SetDefaultStyle(styleFile);

		//////////////////////////////////////////////////////////////////////////
		/// Create debug HUD.
		debugHud_ = engine_->CreateDebugHud();
		debugHud_->SetDefaultStyle(styleFile);

		//////////////////////////////////////////////////////////////////////////
		/// Subscribe key down event
		SubscribeToEvent(E_KEYDOWN, HANDLER(Sample, HandleKeyDown));

		// edit clear color, set background color
// 		Renderer* renderer = GetSubsystem<Renderer>();
// 		Zone* zone = renderer->GetDefaultZone();
// 		zone->SetFogColor(Color(0.3f, 0.3f, 0.4f)); // Set background color for the scene

		//////////////////////////////////////////////////////////////////////////
		/// Create Cursor
// 		Cursor* cursor_ = new Cursor(context_);
// 		cursor_->SetStyleAuto(styleFile);
// 		ui_->SetCursor(cursor_);
// 		if (GetPlatform() == "Android" || GetPlatform() == "iOS")
// 			ui_->GetCursor()->SetVisible(false);
		/// Show Platform Cursor
		Input* input = GetSubsystem<Input>();
		input->SetMouseVisible(true);

		//////////////////////////////////////////////////////////////////////////
		// create main ui
		rootUI_ = ui_->GetRoot()->CreateChild<UIElement>("IDERoot");
		rootUI_->SetSize(ui_->GetRoot()->GetSize());
		rootUI_->SetTraversalMode(TM_DEPTH_FIRST);     // This is needed for root-like element to prevent artifacts
		rootUI_->SetDefaultStyle(styleFile);

		//////////////////////////////////////////////////////////////////////////
		/// create sample scene
		// Create the scene which will be rendered to a texture
		scene_ = new Scene(context_);

		// Create octree, use default volume (-1000, -1000, -1000) to (1000, 1000, 1000)
		scene_->CreateComponent<Octree>();

		// Create a Zone for ambient light & fog control
		Node* zoneNode = scene_->CreateChild("Zone");
		Zone* zone = zoneNode->CreateComponent<Zone>();
		// Set same volume as the Octree, set a close bluish fog and some ambient light
		zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
		zone->SetAmbientColor(Color(0.05f, 0.1f, 0.15f));
		zone->SetFogColor(Color(0.1f, 0.2f, 0.3f));
		zone->SetFogStart(10.0f);
		zone->SetFogEnd(100.0f);

		// Create randomly positioned and oriented box StaticModels in the scene
		const unsigned NUM_OBJECTS = 200;
		for (unsigned i = 0; i < NUM_OBJECTS; ++i)
		{
			Node* boxNode = scene_->CreateChild("Box");
			boxNode->SetPosition(Vector3(Random(200.0f) - 100.0f, Random(200.0f) - 100.0f, Random(200.0f) - 100.0f));
			// Orient using random pitch, yaw and roll Euler angles
			boxNode->SetRotation(Quaternion(Random(360.0f), Random(360.0f), Random(360.0f)));
			StaticModel* boxObject = boxNode->CreateComponent<StaticModel>();
			boxObject->SetModel(cache_->GetResource<Model>("Models/Box.mdl"));
			boxObject->SetMaterial(cache_->GetResource<Material>("Materials/Stone.xml"));
		}

		// Create a camera for the render-to-texture scene. Simply leave it at the world origin and let it observe the scene
		camNode_ = scene_->CreateChild("Camera");
		Camera* camera = camNode_->CreateComponent<Camera>();
		camera->SetFarClip(100.0f);

		// Create a point light to the camera scene node
		Light* light = camNode_->CreateComponent<Light>();
		light->SetLightType(LIGHT_POINT);
		light->SetRange(30.0f);

		//////////////////////////////////////////////////////////////////////////
		/// Setup Viewport
		Renderer* renderer = GetSubsystem<Renderer>();
		// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
		SharedPtr<Viewport> viewport(new Viewport(context_, scene_, camNode_->GetComponent<Camera>()));
		renderer->SetViewport(0, viewport);

		//////////////////////////////////////////////////////////////////////////
		/// 01 create simple Attribute ui elements
		UIElement* variableContainer = rootUI_->CreateChild<UIElement>();
		variableContainer->SetLayout(LM_VERTICAL, 4);

		BasicAttributeUI* attr = CreateAttributeUI(zone, zone->GetAttributes()->At(0), 0, styleFile);
		variableContainer->AddChild(attr);

		attr = CreateAttributeUI(zone, zone->GetAttributes()->At(1), 1, styleFile);
		variableContainer->AddChild(attr);

		attr = CreateAttributeUI(zone, zone->GetAttributes()->At(2), 2, styleFile);
		variableContainer->AddChild(attr);

		//////////////////////////////////////////////////////////////////////////
		/// 02 create simple Attribute Container ui element
		UIElement* serializableContainer = rootUI_->CreateChild<UIElement>();	
		serializableContainer->SetPosition(100, 100);

		AttributeContainer* container = serializableContainer->CreateChild<AttributeContainer>();

		container->SetStyleAuto();
		container->SetTitle(zone->GetTypeName());
		container->SetSerializableAttributes(zone);

		// Resize the node editor according to the number of variables, up to a certain maximum
		unsigned int maxAttrs = container->GetAttributeList()->GetContentElement()->GetNumChildren();
		container->GetAttributeList()->SetHeight(maxAttrs * ATTR_HEIGHT + 4);
		container->SetHeight(maxAttrs * ATTR_HEIGHT + 58);
		container->SetWidth(ATTRNAME_WIDTH * 2);

		container->SetStyleAuto(styleFile);
		container->SetIcon(iconstyle, zone->GetTypeName());

		//////////////////////////////////////////////////////////////////////////
		/// 02 create simple Attribute Container ui element
		attributeInspector_ = new AttributeInspector(context_);
		UIElement* attrinsp = attributeInspector_->Create();
		rootUI_->AddChild(attrinsp);

		attrinsp->SetHeight(maxAttrs * ATTR_HEIGHT + 58);
		attrinsp->SetWidth(ATTRNAME_WIDTH * 2);

		IntVector2 size = attrinsp->GetSize();
		Graphics* graphics = GetSubsystem<Graphics>();
		attrinsp->SetPosition((graphics->GetWidth() - size.x_) / 2, (graphics->GetHeight() - size.y_) / 2);	
		/// \todo add multiple components and nodes.
		/// \todo set inspector mode: editable mode(to edit attributes), view only mode (to only view the attributes without editing them) 
		/// \todo UIElements are not supported for now.
		/// you have to add either a node or a component for editing it
		//attributeInspector_->GetEditComponents().Push(zone);
		attributeInspector_->GetEditNodes().Push(camNode_);
		for (unsigned i = 0; i < camNode_->GetComponents().Size();i++)
		{
			Component* c = camNode_->GetComponents()[i];
			attributeInspector_->GetEditComponents().Push(c);
		}
	
		/// and dont forget to update 
		attributeInspector_->Update();
	}

	void Sample::Stop()
	{
	
	
	}

	void Sample::HandleKeyDown(StringHash eventType, VariantMap& eventData)
	{
		using namespace KeyDown;

		int key = eventData[P_KEY].GetInt();

		// Close console (if open) or exit when ESC is pressed
		if (key == KEY_ESC)
		{
			Console* console = GetSubsystem<Console>();
			if (console->IsVisible())
				console->SetVisible(false);
			else
				GetSubsystem<Engine>()->Exit();
		}

		// Toggle console with F1
		else if (key == KEY_F1)
			GetSubsystem<Console>()->Toggle();

		// Toggle debug HUD with F2
		else if (key == KEY_F2)
			GetSubsystem<DebugHud>()->ToggleAll();

		// Common rendering quality controls, only when UI has no focused element
		else if (!GetSubsystem<UI>()->GetFocusElement())
		{
			Renderer* renderer = GetSubsystem<Renderer>();

			// Texture quality
			if (key == '1')
			{
				int quality = renderer->GetTextureQuality();
				++quality;
				if (quality > QUALITY_HIGH)
					quality = QUALITY_LOW;
				renderer->SetTextureQuality(quality);
			}

			// Material quality
			else if (key == '2')
			{
				int quality = renderer->GetMaterialQuality();
				++quality;
				if (quality > QUALITY_HIGH)
					quality = QUALITY_LOW;
				renderer->SetMaterialQuality(quality);
			}

			// Specular lighting
			else if (key == '3')
				renderer->SetSpecularLighting(!renderer->GetSpecularLighting());

			// Shadow rendering
			else if (key == '4')
				renderer->SetDrawShadows(!renderer->GetDrawShadows());

			// Shadow map resolution
			else if (key == '5')
			{
				int shadowMapSize = renderer->GetShadowMapSize();
				shadowMapSize *= 2;
				if (shadowMapSize > 2048)
					shadowMapSize = 512;
				renderer->SetShadowMapSize(shadowMapSize);
			}

			// Shadow depth and filtering quality
			else if (key == '6')
			{
				int quality = renderer->GetShadowQuality();
				++quality;
				if (quality > SHADOWQUALITY_HIGH_24BIT)
					quality = SHADOWQUALITY_LOW_16BIT;
				renderer->SetShadowQuality(quality);
			}

			// Occlusion culling
			else if (key == '7')
			{
				bool occlusion = renderer->GetMaxOccluderTriangles() > 0;
				occlusion = !occlusion;
				renderer->SetMaxOccluderTriangles(occlusion ? 5000 : 0);
			}

			// Instancing
			else if (key == '8')
				renderer->SetDynamicInstancing(!renderer->GetDynamicInstancing());


		}
		// Take screenshot
		else if (key == '9')
		{
			Graphics* graphics = GetSubsystem<Graphics>();
			Image screenshot(context_);
			graphics->TakeScreenShot(screenshot);
			// Here we save in the Data folder with date and time appended
			screenshot.SavePNG(GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Screenshot_" +
				Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
		}
	}





}