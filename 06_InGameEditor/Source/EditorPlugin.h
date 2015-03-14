#pragma once


#include "../Core/Object.h"
#include "../Core/Context.h"
#include "Macros.h"

namespace Urho3D
{
	class InGameEditor;

	class EditorPlugin : public Object
	{
		OBJECT(EditorPlugin);
		BASEOBJECT(EditorPlugin);
	public:
		/// Construct.
		EditorPlugin(Context* context);
		/// Destruct.
		virtual ~EditorPlugin();
		/// Register object factory.
		static void RegisterObject(Context* context);

		/// called when the plugin is activated. 
		virtual void Enter() {}
		/// Hide 
		virtual void Hide() {}
		/// Show 
		virtual void Show() {}
		/// called when the plugin is closed or switched to other main plugin.
		virtual void Leave() {}
		/// 
		virtual void Update(float delta) {  }

		/// return false if the input was not consumed otherwise true.
		virtual bool OnKeyInput(int key, bool down) { return false; }
		/// return false if the input was not consumed otherwise true.
		virtual bool OnMouseButtonInput(int button, bool down) { return false; }
		/// return false if this plugin is a subeditor and does not manipulate the main frame window
		virtual bool HasMainFrame() { return false; }


	protected:
		InGameEditor* editor_;

	};
}

