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
#include "../Graphics/Texture2D.h"
#include "../Graphics/Texture.h"
#include "../Scene/Scene.h"
#include "../Graphics/DebugRenderer.h"
#include "../Math/BoundingBox.h"
#include "../Graphics/Light.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Viewport.h"

#include "Galaxy.h"

#include "../DebugNew.h"










using namespace Urho3D;
DEFINE_APPLICATION_MAIN(Sample)
namespace Urho3D
{
	Sample::Sample(Context * context) : Application(context),
		yaw_(0.0f),
		pitch_(0.0f)
	{
		cache_ = NULL;
		ui_ = NULL;
		console_ = NULL;
		debugHud_ = NULL;
		graphics_ = NULL;
		drawDebug_ = false;
	}


	void Sample::Setup()
	{		
		// Modify engine startup parameters
		engineParameters_["WindowTitle"] = GetTypeName();
		engineParameters_["LogName"] = GetTypeName() + ".log";//GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs")
		engineParameters_["FullScreen"] = false;
		engineParameters_["Headless"] = false;
		engineParameters_["ResourcePaths"] = "Data;CoreData;GameData";
		
		Galaxy::RegisterObject(context_);
	}

	void Sample::Start()
	{

		cache_ = GetSubsystem<ResourceCache>();
		ui_ = GetSubsystem<UI>();
		graphics_ = GetSubsystem<Graphics>();
		
		//////////////////////////////////////////////////////////////////////////
		// Get default styles
		XMLFile* styleFile = cache_->GetResource<XMLFile>("UI/DefaultStyle.xml");
		ui_->GetRoot()->SetDefaultStyle(styleFile);
		// create main ui
		rootUI_ = ui_->GetRoot()->CreateChild<UIElement>("IDERoot");
		rootUI_->SetSize(ui_->GetRoot()->GetSize());
		rootUI_->SetTraversalMode(TM_DEPTH_FIRST);     // This is needed for root-like element to prevent artifacts
		rootUI_->SetDefaultStyle(styleFile);

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
		Renderer* renderer = GetSubsystem<Renderer>();
		Zone* zone = renderer->GetDefaultZone();
		zone->SetFogColor(Color(0.3f, 0.3f, 0.4f)); // Set background color for the scene

		//////////////////////////////////////////////////////////////////////////
		/// Create Cursor
// 		Cursor* cursor_ = new Cursor(context_);
// 		cursor_->SetStyleAuto(styleFile);
// 		ui_->SetCursor(cursor_);
// 		if (GetPlatform() == "Android" || GetPlatform() == "iOS")
// 			ui_->GetCursor()->SetVisible(false);
		/// Show Platform Cursor
// 		Input* input = GetSubsystem<Input>();
// 		input->SetMouseVisible(true);

		// Create the scene content
		CreateScene();

		// Create the UI content
		CreateInstructions();

		// Setup the viewport for displaying the scene
		SetupViewport();

		// Hook up to the frame update and render post-update events
		SubscribeToEvents();
		
	}

	void Sample::Stop()
	{
	
	
	}

	void Sample::CreateScene()
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();

		scene_ = new Scene(context_);

		// Create octree, use default volume (-1000, -1000, -1000) to (1000, 1000, 1000)
		// Also create a DebugRenderer component so that we can draw debug geometry
		scene_->CreateComponent<Octree>();
		scene_->CreateComponent<DebugRenderer>();

		// Create a Zone component for ambient lighting & fog control
		Node* zoneNode = scene_->CreateChild("Zone");
// 		Zone* zone = zoneNode->CreateComponent<Zone>();
// 		zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
// 		zone->SetAmbientColor(Color(0.1f, 0.1f, 0.1f));
// 		zone->SetFogStart(100.0f);
// 		zone->SetFogEnd(300.0f);

		// Create a directional light without shadows
		Node* lightNode = scene_->CreateChild("DirectionalLight");
		lightNode->SetDirection(Vector3(0.5f, -1.0f, 0.5f));
		Light* light = lightNode->CreateComponent<Light>();
		light->SetLightType(LIGHT_DIRECTIONAL);
		light->SetColor(Color(0.2f, 0.2f, 0.2f));
		light->SetSpecularIntensity(1.0f);


		// Create the camera. Limit far clip distance to match the fog
		camNode_ = scene_->CreateChild("Camera");
		Camera* camera = camNode_->CreateComponent<Camera>();
		camera->SetFarClip(1300.0f);

		// Set an initial position for the camera scene node above the plane
		camNode_->SetPosition(Vector3(0.0f, 5.0f, 0.0f));

		Node* galaxyNode = scene_->CreateChild("Galaxy");

		Galaxy* galaxyComponent = galaxyNode->CreateComponent<Galaxy>();

	}

	void Sample::SetupViewport()
	{
		Renderer* renderer = GetSubsystem<Renderer>();

		// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
		SharedPtr<Viewport> viewport(new Viewport(context_, scene_, camNode_->GetComponent<Camera>()));
		renderer->SetViewport(0, viewport);
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

	}

	void Sample::CreateInstructions()
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		UI* ui = GetSubsystem<UI>();

		// Construct new Text object, set string to display and font to use
		Text* instructionText = ui->GetRoot()->CreateChild<Text>();
		instructionText->SetText(
			"Use WASD keys and mouse/touch to move\n"
			"Space to toggle debug geometry"
			);
		instructionText->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
		// The text has multiple rows. Center them in relation to each other
		instructionText->SetTextAlignment(HA_CENTER);

		// Position the text relative to the screen center
		instructionText->SetHorizontalAlignment(HA_CENTER);
		instructionText->SetVerticalAlignment(VA_CENTER);
		instructionText->SetPosition(0, ui->GetRoot()->GetHeight() / 4);
	}

	void Sample::MoveCamera(float timeStep)
	{
		// Do not move if the UI has a focused element (the console)
		if (GetSubsystem<UI>()->GetFocusElement())
			return;

		Input* input = GetSubsystem<Input>();

		// Movement speed as world units per second
		float MOVE_SPEED = 20.0f;
		if (input->GetKeyDown(KEY_SHIFT))
		{
			MOVE_SPEED = 60.0f;
		}
		// Mouse sensitivity as degrees per pixel
		const float MOUSE_SENSITIVITY = 0.1f;

		// Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
		IntVector2 mouseMove = input->GetMouseMove();
		yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
		pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
		pitch_ = Clamp(pitch_, -90.0f, 90.0f);

		// Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
		camNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

		// Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
		if (input->GetKeyDown('W'))
			camNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
		if (input->GetKeyDown('S'))
			camNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
		if (input->GetKeyDown('A'))
			camNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
		if (input->GetKeyDown('D'))
			camNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);

		// Toggle debug geometry with space
		if (input->GetKeyPress(KEY_SPACE))
			drawDebug_ = !drawDebug_;
	}

	void Sample::SubscribeToEvents()
	{
		// Subscribe HandleUpdate() function for processing update events
		SubscribeToEvent(E_UPDATE, HANDLER(Sample, HandleUpdate));

		// Subscribe HandlePostRenderUpdate() function for processing the post-render update event, during which we request
		// debug geometry
		SubscribeToEvent(E_POSTRENDERUPDATE, HANDLER(Sample, HandlePostRenderUpdate));
	}

	void Sample::HandleUpdate(StringHash eventType, VariantMap& eventData)
	{
		using namespace Update;

		// Take the frame time step, which is stored as a float
		float timeStep = eventData[P_TIMESTEP].GetFloat();

		// Move the camera and animate the scene, scale movement with time step
		MoveCamera(timeStep);
	
	}

	void Sample::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
	{
		// If draw debug mode is enabled, draw viewport debug geometry. This time use depth test, as otherwise the result becomes
		// hard to interpret due to large object count
		if (drawDebug_)
			GetSubsystem<Renderer>()->DrawDebugGeometry(true);
	}

}