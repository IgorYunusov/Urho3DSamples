
#include "../Urho3D.h"
#include "EditorPlugin.h"
#include "InGameEditor.h"

namespace Urho3D
{


	EditorPlugin::EditorPlugin(Context* context) : Object(context),
		editor_(NULL)
	{
		editor_ = GetSubsystem<InGameEditor>();
	}

	EditorPlugin::~EditorPlugin()
	{

	}

	void EditorPlugin::RegisterObject(Context* context)
	{
		context->RegisterFactory<EditorPlugin>("EditorPlugin");
	}

}

