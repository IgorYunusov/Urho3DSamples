
#pragma once





#include "GameObject.h"

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;



class LightFlash : public GameObject
{
	OBJECT(LightFlash);
public:
    //-------------------------------------------------------------------------
    // Constructors & Destructors
    //-------------------------------------------------------------------------
	LightFlash(Context* context);
	~LightFlash();
	/// Register object factory.
	static void RegisterObject(Context* context);

	virtual void FixedUpdate(float timeStep);


	
protected:


private:

	
};

