#include "Sun.h"
#include "..\Graphics\Texture.h"
#include "..\Graphics\Texture2D.h"

namespace Urho3D
{


	Sun::Sun(Context* context) : Component(context)
	{

	}

	Sun::~Sun()
	{

	}

	void Sun::RegisterObject(Context* context)
	{
		context->RegisterFactory<Sun>();
	
		ACCESSOR_ATTRIBUTE("Lens Flare Color", GetLensFlareColor, SetLensFlareColor, Color, Color::WHITE , AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("Lens Flare Scale Factor", GetLensFlareScaleFactor, SetLensFlareScaleFactor, float, 0.125f, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("Light Scale Factor", GetLightScaleFactor, SetLightScaleFactor, Vector2, Vector2(0.5f,0.5f), AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("Min Distribution", GetDistributionValueMin, SetDistributionValueMin, float, 0.0f, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("Max Distribution", GetDistributionValueMax, SetDistributionValueMax, float, 0.5f, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("Min Scale", GetMinScale, SetMinScale, float,1.0f , AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("Max Scale", GetMaxScale, SetMaxScale, float,2.0f, AM_DEFAULT);
		
	}

}