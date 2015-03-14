#pragma once

#include "../Urho3D.h"
#include "../Core/Object.h"
#include "../Core/Context.h"
#include "Macros.h"

namespace Urho3D
{

	class EditorPlugin : public Object
	{
		OBJECT(EditorPlugin);
	public:
		/// Construct.
		EditorPlugin(Context* context);
		/// Destruct.
		virtual ~EditorPlugin();
		/// Register object factory.
		static void RegisterObject(Context* context);

	protected:
		

	};
}

