
#pragma once


#include "GameObject.h"

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;
const int snowcrateHealth = 5;
const int snowcratePoints = 250;

class SnowCrate : public GameObject
{
	OBJECT(SnowCrate);
public:

	SnowCrate(Context* context);
	~SnowCrate();
	/// Register object factory.
	static void RegisterObject(Context* context);

	/// Called when the component is added to a scene node. Other components may not yet exist.
	virtual void Start();

	virtual void FixedUpdate(float timeStep);

	
	
protected:


private:

	
};

