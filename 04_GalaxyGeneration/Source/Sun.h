#pragma once
#include "../Urho3D.h"
#include "../Scene/Component.h"
#include "../Core/Context.h"
#include "Macros.h"

namespace Urho3D
{
	class Texture;
	class Color;

	class Sun : public Component
	{
		OBJECT(Sun);
	public:
		/// Construct.
		Sun(Context* context);
		/// Destruct.
		virtual ~Sun();
		/// Register object factory.
		static void RegisterObject(Context* context);

		// Serializable Attributes
		U_PROPERTY_IMP_PASS_BY_REF(Color, lensFlareColor_, LensFlareColor);
		U_PROPERTY_IMP(float, lensFlareScaleFactor_, LensFlareScaleFactor);
		U_PROPERTY_IMP_PASS_BY_REF(Vector2, lightScaleFactor_, LightScaleFactor);
		U_PROPERTY_IMP(float, distributionValueMin_, DistributionValueMin);
		U_PROPERTY_IMP(float, distributionValueMax_, DistributionValueMax);
		U_PROPERTY_IMP(float, minScale_, MinScale);
		U_PROPERTY_IMP(float, maxScale_, MaxScale);

	protected:
		// Serializable Attributes
		/// Sun texture.
		SharedPtr<Texture> sunTexture_;

	};
}
