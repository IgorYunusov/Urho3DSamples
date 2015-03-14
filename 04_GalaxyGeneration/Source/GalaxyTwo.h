#pragma once


#include "../Scene/LogicComponent.h"

#include "Macros.h"

namespace Urho3D
{

	class GalaxyTwo : public LogicComponent
	{
		OBJECT(GalaxyTwo);
	public:
		/// Construct.
		GalaxyTwo(Context* context);
		/// Destruct.
		virtual ~GalaxyTwo();
		/// Register object factory.
		static void RegisterObject(Context* context);

		// LogicComponent
		/// Called when the component is added to a scene node. Other components may not yet exist.
		virtual void Start();
		/// Called before the first update. At this point all other components of the node should exist. Will also be called if update events are not wanted; in that case the event is immediately unsubscribed afterward.
		virtual void DelayedStart();
		/// Called when the component is detached from a scene node, usually on destruction. Note that you will no longer have access to the node and scene at that point.
		virtual void Stop();
		/// Called on scene update, variable timestep.
		virtual void Update(float timeStep);

		// Create Galaxy Objects
		void CreateSun(const Vector3& pos, float scale, int type);
		float MapRange(float s, float a1, float a2, float b1, float b2);
		// Serializable Attributes
		U_PROPERTY_IMP_PASS_BY_REF(String, name_, Name)
		U_PROPERTY_IMP(int, size_, Size);
		U_PROPERTY_IMP(float, nebulaFade_, NebulaFade);
		U_PROPERTY_IMP(int, numHub_, NumHub);
		U_PROPERTY_IMP(int, numDisk_, NumDisk);
		U_PROPERTY_IMP(float, diskRad_, DiskRad);
		U_PROPERTY_IMP(float, hubRad_, HubRad);
		U_PROPERTY_IMP(int, numArms_, NumArms);
		U_PROPERTY_IMP(float, armRots_, ArmRots);
		U_PROPERTY_IMP(float, armWidth_, ArmWidth);
		U_PROPERTY_IMP(float, fuzz_, Fuzz);

		void generateGalaxy(float Size, int nrArms, int width, int beer, int colorType = -1, float centerPercent = 33, float centerScalePercent = 25, float axialScale = 0.75f);

	protected:

		// Serializable Attributes


		// other Attributes


	};
}



