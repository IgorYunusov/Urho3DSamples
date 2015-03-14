#include "EditorPlugin.h"

namespace Urho3D
{


	EditorPlugin::EditorPlugin(Context* context) : Object(context)
	{

	}

	EditorPlugin::~EditorPlugin()
	{

	}

	void EditorPlugin::RegisterObject(Context* context)
	{
		context->RegisterFactory<EditorPlugin>();
	}

}

