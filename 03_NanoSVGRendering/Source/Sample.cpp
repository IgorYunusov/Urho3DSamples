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

#include <stdio.h>
#include <string.h>
#include <float.h>
#define NANOSVG_ALL_COLOR_KEYWORDS  // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION
#include "nanosvg/nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg/nanosvgrast.h"



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
		Input* input = GetSubsystem<Input>();
		input->SetMouseVisible(true);

		//////////////////////////////////////////////////////////////////////////
		/// create an svg image
		rootUI_->AddChild(CreateSVGSprite("GameData/svg/23.svg"));

		Sprite* drawing = CreateSVGSprite("GameData/svg/drawing.svg");
		if (drawing)
		{
			// Set logo sprite hot spot
			drawing->SetHotSpot(0, drawing->GetHeight());
			drawing->SetAlignment(HA_LEFT, VA_BOTTOM);
			rootUI_->AddChild(drawing);
		}
		Sprite* nano = CreateSVGSprite("GameData/svg/nano.svg");
		if (nano)
		{
			// Set logo sprite hot spot
			nano->SetHotSpot(0, -nano->GetHeight());
			nano->SetAlignment(HA_LEFT, VA_TOP);
			rootUI_->AddChild(nano);
		}
		
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





	Sprite* Sample::CreateSVGSprite(const String& file)
	{
		FileSystem* fileSystem = GetSubsystem<FileSystem>();
		String exePath = fileSystem->GetProgramDir();

		String svgfile = exePath + file;
		if (!fileSystem->FileExists(svgfile))
			return NULL;

		NSVGimage *image = NULL;
		NSVGrasterizer *rast = NULL;
		int w, h;
		LOGINFOF("parsing %s\n", svgfile.CString());

		image = nsvgParseFromFile(svgfile.CString(), "px", 128.0f);
		if (image == NULL) {
			LOGERROR("Could not open SVG image.\n");
		}
		w = image->width;
		h = image->height;

		rast = nsvgCreateRasterizer();
		if (rast == NULL) {
			LOGERROR("Could not init rasterizer.\n");
		}

		rast->tessTol = 2.25f;
		rast->distTol = 2.1f;

		SharedPtr<Image> saveimage(new Image(context_));
		saveimage->SetSize(w, h, 4);

		LOGINFOF("rasterizing image %d x %d\n", w, h);
		nsvgRasterize(rast, image, 0.0f, 0.0f, 1, saveimage->GetData(), w, h, w * 4);

		nsvgDeleteRasterizer(rast);
		nsvgDelete(image);

		// Get logo texture
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		Texture2D* logoTexture = new Texture2D(context_);
		logoTexture->SetData(saveimage);

		Sprite* logoSprite_ = new Sprite(context_);

		// Set logo sprite texture
		logoSprite_->SetTexture(logoTexture);

		int textureWidth = logoTexture->GetWidth();
		int textureHeight = logoTexture->GetHeight();

		// Set logo sprite scale
		logoSprite_->SetScale(256.0f / textureWidth);

		// Set logo sprite size
		logoSprite_->SetSize(textureWidth, textureHeight);

		// Set logo sprite hot spot
		logoSprite_->SetHotSpot(textureWidth / 2, textureHeight / 2);

		// Set logo sprite alignment
		logoSprite_->SetAlignment(HA_CENTER, VA_CENTER);

		return logoSprite_;
	}

}