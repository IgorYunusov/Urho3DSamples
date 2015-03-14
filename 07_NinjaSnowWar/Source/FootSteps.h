
#pragma once


#include "GameObject.h"

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;



class FootSteps : public GameObject
{
	OBJECT(FootSteps);
public:
    //-------------------------------------------------------------------------
    // Constructors & Destructors
    //-------------------------------------------------------------------------
	FootSteps(Context* context);
	~FootSteps();
	/// Register object factory.
	static void RegisterObject(Context* context);

	/// Called when the component is added to a scene node. Other components may not yet exist.
	virtual void Start();
	
	virtual void HandleAnimationTrigger(StringHash eventType, VariantMap& eventData);
protected:


private:

	
};

