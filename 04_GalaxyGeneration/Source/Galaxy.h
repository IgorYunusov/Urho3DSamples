#pragma once


#include "../Scene/LogicComponent.h"

#include "Macros.h"

namespace Urho3D
{

	class Galaxy : public LogicComponent
	{
		OBJECT(Galaxy);
	public:
		/// Construct.
		Galaxy(Context* context);
		/// Destruct.
		virtual ~Galaxy();
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
		void CreateSun(const Vector3& pos,float scale,int type);

		// Serializable Attributes
		U_PROPERTY_IMP(int, maxSunsX_, MaxSunsX);
		U_PROPERTY_IMP(int, maxSunsY_, MaxSunsY);
		U_PROPERTY_IMP(int, maxSunsZ_, MaxSunsZ);
		U_PROPERTY_IMP_PASS_BY_REF(Vector3,delta_,Delta)
		U_PROPERTY_IMP_PASS_BY_REF(Vector3,minVariance_,MinVariance)
		U_PROPERTY_IMP_PASS_BY_REF(Vector3,maxVariance_,MaxVariance)

	protected:

		// Serializable Attributes
		float g_GalaxyDistanceDecreaseParameter1 = 0.125f;
		float g_GalaxyDistanceDecreaseParameter2 = 5.0f;
		float g_GalaxyDistanceDecreaseParameter3 = 10.5f;

		float g_SpiralGalaxyDistanceExponent = 0.6f;

		float g_SpiralGalaxyTwistingFactorMin = 0.6f;
		float g_SpiralGalaxyTwistingFactorMax = 0.8f;

		float g_SpiralGalaxyBulgeRadius = 360.0f;

		long g_NumOfSpiralArms = 2;

		Vector3 SpiralGalaxyPlaneDirection1 = Vector3(1.0f, 0.0f, 0.0f);// g_CameraHorizontalOriginal;
		Vector3 SpiralGalaxyPlaneDirection2 = Vector3(0.0f, 1.0f, 0.0f);//g_CameraVerticalOriginal;
		Vector3 ThreeArmedSpiralGalaxyBaseDirection = Vector3(0.0f, 1.0f, 0.0f);//g_CameraVerticalOriginal;

		float g_GalaxyStellarDistanceIncreaseFactor1 = 1.7f;
		float g_GalaxyStellarDistanceIncreaseFactor2 = 0.01f;

		float g_SpiralGalaxyFlatnessFactor = 0.3f;

		// other Attributes
		long maxStars_;

		float maxLocalGroupDistanceSq_;


	};
}



