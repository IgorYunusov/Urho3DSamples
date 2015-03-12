
#include "..\Urho3D.h"
#include "..\IO\Log.h"
#include "..\Core\Context.h"
#include "Galaxy.h"
#include "..\Scene\Serializable.h"
#include "..\Graphics\BillboardSet.h"
#include "..\Scene\Component.h"
#include "..\Scene\Node.h"
#include "..\Scene\Scene.h"
#include "..\Graphics\Material.h"
#include "..\Resource\ResourceCache.h"
#include "..\Math\Vector2.h"
#include "..\Math\MathDefs.h"


namespace Urho3D
{

	inline long lrnd(long low, long high)
	{
		long tempLrnd = low + (high - low) * ((long)rand()) / RAND_MAX;

		if (tempLrnd < high)
			return tempLrnd;
		else
			return low;
	}
	inline float frnd(float low, float high)
	{
		return low + (high - low) * ((float)rand()) / RAND_MAX;
	}


	Galaxy::Galaxy(Context* context) : LogicComponent(context),
		maxSunsX_(10),
		maxSunsZ_(10),
		maxSunsY_(10),
		delta_(10.0f, 10.0f, 10.0f),
		minVariance_(-2.0f, -2.0f, -2.0f),
		maxVariance_(2.0f, 2.0f, 2.0f)
	{
		SetUpdateEventMask(USE_UPDATE);

	}

	Galaxy::~Galaxy()
	{

	}

	void Galaxy::RegisterObject(Context* context)
	{
		context->RegisterFactory<Galaxy>();

		ACCESSOR_ATTRIBUTE("Max Suns X", GetMaxSunsX, SetMaxSunsX, int, 10, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("Max Suns Y", GetMaxSunsY, SetMaxSunsY, int, 10, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("Max Suns Z", GetMaxSunsZ, SetMaxSunsZ, int, 10, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("Delta", GetDelta, SetDelta, Vector3, Vector3(10.0f, 10.0f, 10.0f), AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("Min Variance", GetMinVariance, SetMinVariance, Vector3, Vector3(-2.0f, -2.0f, -2.0f), AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("Max Variance", GetMaxVariance, SetMaxVariance, Vector3, Vector3(2.0f, 2.0f, 2.0f), AM_DEFAULT);
	}

	void Galaxy::Start()
	{
		/// create galaxy 
		maxSunsY_ = 30;
		maxSunsX_ = 2;
		maxSunsZ_ = 30;
		g_NumOfSpiralArms = 3;

		delta_.x_ = 15.0f;
		delta_.y_ = 15.0f;
		delta_.z_ = 4.0f;

 
		int numSunsMax = maxSunsX_*maxSunsZ_*maxSunsY_;

		Vector<Vector3> stellarPositions;
		stellarPositions.Resize(numSunsMax);

		Vector<bool> useStellarPositions;
		useStellarPositions.Resize(numSunsMax);


		long i = 0, j = 0, k = 0;

		Vector3 centerPos;
		Vector3 diffVector;

		Vector3 startPos = Vector3(-0.5f*(maxSunsX_ - 1)*delta_.x_,
			-0.5f*(maxSunsY_ - 1)*delta_.y_,
			-0.5f*(maxSunsZ_ - 1)*delta_.z_);

		long counter = 0;

		float distance = 0.0f;

		float maxDistance = g_GalaxyDistanceDecreaseParameter3*startPos.Length();

		float varX = 0.0f;
		float varY = 0.0f;
		float varZ = 0.0f;

		for (i = 0; i < maxSunsX_; i++)
		{
			for (j = 0; j < maxSunsY_; j++)
			{
				for (k = 0; k < maxSunsZ_; k++)
				{
					if (lrnd(2, 4) == 2)
						varX = frnd(0.5f*maxVariance_.x_, maxVariance_.x_);
					else
						varX = frnd(minVariance_.x_, 0.5f*minVariance_.x_);

					if (lrnd(2, 4) == 2)
						varY = frnd(0.5f*maxVariance_.y_, maxVariance_.y_);
					else
						varY = frnd(minVariance_.y_, 0.5f*minVariance_.y_);

					if (lrnd(2, 4) == 2)
						varZ = frnd(0.5f*maxVariance_.z_, maxVariance_.z_);
					else
						varZ = frnd(minVariance_.z_, 0.5f*minVariance_.z_);

					stellarPositions[counter] = startPos + Vector3(i*delta_.x_, j*delta_.y_, k*delta_.z_) + Vector3(varX, varY, varZ);

					/*StellarPosition[counter] = StartPos + D3DXVECTOR3(i*g_GalaxyParam_DeltaX, j*g_GalaxyParam_DeltaY, k*g_GalaxyParam_DeltaZ)+
					D3DXVECTOR3(frnd(g_GalaxyParam_MinVarianceX, g_GalaxyParam_MaxVarianceX),
					frnd(g_GalaxyParam_MinVarianceY, g_GalaxyParam_MaxVarianceY),
					frnd(g_GalaxyParam_MinVarianceZ, g_GalaxyParam_MaxVarianceZ));
					*/
					useStellarPositions[counter] = true;

					counter++;
				}
			}
		}


		counter = 0;

		float randomValue = 0.0f;

		float quotient = 0.0f;

		for (i = 0; i < numSunsMax; i++)
		{
			diffVector = stellarPositions[i] - centerPos;

			distance = diffVector.Length();

			randomValue = frnd(0.0f, 1.0f);
			//randomValue = frnd_LookUp(0.0f, 1.0f); 

			quotient = distance / maxDistance;

			//if(randomValue >= 1.0f-pow(quotient, g_GalaxyDistanceDecreaseParameter1))

			if (randomValue >= 1.0f - pow(quotient, g_GalaxyDistanceDecreaseParameter1*pow(1.0f - quotient, g_GalaxyDistanceDecreaseParameter2)))
			{
				useStellarPositions[i] = false;
			}
			else
				counter++;
		}

		LOGINFOF("Galaxy Info: NumSunsMax %i", numSunsMax);
		LOGINFOF("Galaxy Info: counter %i", counter);

		Vector3 DiffVectorRotated;
		Quaternion rotation;
		Matrix3 rotationMat;
		Vector3 GalacticAxis;

		GalacticAxis = SpiralGalaxyPlaneDirection1.CrossProduct(SpiralGalaxyPlaneDirection2);

		float dot1 = 0.0f, dot2 = 0.0f, dot3 = 0.0f;

		float distanceFactor = 0.0f;

		long randomDir = 0;

		if (g_NumOfSpiralArms == 4)
		{
			for (i = 0; i < numSunsMax; i++)
			{
				if (useStellarPositions[i] == true)
				{
					diffVector = stellarPositions[i] - centerPos;
					
					dot1 = diffVector.DotProduct(SpiralGalaxyPlaneDirection1);
					dot2 = diffVector.DotProduct(SpiralGalaxyPlaneDirection2);
					dot3 = diffVector.DotProduct(GalacticAxis);

					distance = sqrtf(dot1*dot1 + dot2*dot2);

					if (distance < g_SpiralGalaxyBulgeRadius)
					{
						distanceFactor = pow(distance, g_SpiralGalaxyDistanceExponent);
						diffVector *= pow(g_GalaxyStellarDistanceIncreaseFactor1, g_GalaxyStellarDistanceIncreaseFactor2*distance);
						stellarPositions[i] = diffVector + centerPos;

						continue;
					}

					randomDir = lrnd(0, 4);

					if (randomDir == 0)
					{
						diffVector = SpiralGalaxyPlaneDirection1*distance + (g_SpiralGalaxyFlatnessFactor*dot3)*GalacticAxis;
					}
					else if (randomDir == 1)
					{
						diffVector = -SpiralGalaxyPlaneDirection1*distance + (g_SpiralGalaxyFlatnessFactor*dot3)*GalacticAxis;
					}
					else if (randomDir == 2)
					{
						diffVector = SpiralGalaxyPlaneDirection2*distance + (g_SpiralGalaxyFlatnessFactor*dot3)*GalacticAxis;
					}
					else if (randomDir == 3)
					{
						diffVector = -SpiralGalaxyPlaneDirection2*distance + (g_SpiralGalaxyFlatnessFactor*dot3)*GalacticAxis;
					}

					distanceFactor = pow(distance, g_SpiralGalaxyDistanceExponent);
					rotation.FromAngleAxis(frnd(g_SpiralGalaxyTwistingFactorMin, g_SpiralGalaxyTwistingFactorMax)*distanceFactor*M_RADTODEG, GalacticAxis);
					rotationMat = rotation.RotationMatrix();

					DiffVectorRotated = rotationMat*diffVector;

					DiffVectorRotated *= pow(g_GalaxyStellarDistanceIncreaseFactor1, g_GalaxyStellarDistanceIncreaseFactor2*distance);

					stellarPositions[i] = DiffVectorRotated + centerPos;
				}
			}
		}
		else if (g_NumOfSpiralArms == 3)
		{
			Vector3 ThreeSpiralArmsDirection2;
			Vector3 ThreeSpiralArmsDirection3;

			rotation.FromAngleAxis(120.0f, GalacticAxis);
			rotationMat = rotation.RotationMatrix();

			ThreeSpiralArmsDirection2 = rotationMat*ThreeArmedSpiralGalaxyBaseDirection;
			ThreeSpiralArmsDirection3 = rotationMat*ThreeSpiralArmsDirection2;

			for (i = 0; i < numSunsMax; i++)
			{
				if (useStellarPositions[i] == true)
				{
					diffVector = stellarPositions[i] - centerPos;

					dot1 = diffVector.DotProduct(SpiralGalaxyPlaneDirection1);
					dot2 = diffVector.DotProduct(SpiralGalaxyPlaneDirection2);
					dot3 = diffVector.DotProduct(GalacticAxis);

					distance = sqrtf(dot1*dot1 + dot2*dot2);

					if (distance < g_SpiralGalaxyBulgeRadius)
					{
						distanceFactor = pow(distance, g_SpiralGalaxyDistanceExponent);
						diffVector *= pow(g_GalaxyStellarDistanceIncreaseFactor1, g_GalaxyStellarDistanceIncreaseFactor2*distance);
						stellarPositions[i] = diffVector + centerPos;

						continue;
					}

					randomDir = lrnd(0, 3);

					if (randomDir == 0)
					{
						diffVector = ThreeArmedSpiralGalaxyBaseDirection*distance + (g_SpiralGalaxyFlatnessFactor*dot3)*GalacticAxis;
					}
					else if (randomDir == 1)
					{
						diffVector = ThreeSpiralArmsDirection2*distance + (g_SpiralGalaxyFlatnessFactor*dot3)*GalacticAxis;
					}
					else if (randomDir == 2)
					{
						diffVector = ThreeSpiralArmsDirection3*distance + (g_SpiralGalaxyFlatnessFactor*dot3)*GalacticAxis;
					}


					distanceFactor = pow(distance, g_SpiralGalaxyDistanceExponent);

					rotation.FromAngleAxis(frnd(g_SpiralGalaxyTwistingFactorMin, g_SpiralGalaxyTwistingFactorMax)*distanceFactor*M_RADTODEG, GalacticAxis);
					rotationMat = rotation.RotationMatrix();

					DiffVectorRotated = rotationMat*diffVector;

					DiffVectorRotated *= pow(g_GalaxyStellarDistanceIncreaseFactor1, g_GalaxyStellarDistanceIncreaseFactor2*distance);

					stellarPositions[i] = DiffVectorRotated + centerPos;
				}
			}
		}
		else //if(g_NumOfSpiralArms == 2)
		{
			for (i = 0; i < numSunsMax; i++)
			{
				if (useStellarPositions[i] == true)
				{
					diffVector = stellarPositions[i] - centerPos;

					dot1 = diffVector.DotProduct(SpiralGalaxyPlaneDirection1);
					dot2 = diffVector.DotProduct(SpiralGalaxyPlaneDirection2);
					dot3 = diffVector.DotProduct(GalacticAxis);

					distance = sqrtf(dot1*dot1 + dot2*dot2);

					if (distance < g_SpiralGalaxyBulgeRadius)
					{
						distanceFactor = pow(distance, g_SpiralGalaxyDistanceExponent);
						diffVector *= pow(g_GalaxyStellarDistanceIncreaseFactor1, g_GalaxyStellarDistanceIncreaseFactor2*distance);
						stellarPositions[i] = diffVector + centerPos;

						continue;
					}

					randomDir = lrnd(0, 2);

					if (randomDir == 0)
					{
						diffVector = SpiralGalaxyPlaneDirection1*distance + (g_SpiralGalaxyFlatnessFactor*dot3)*GalacticAxis;
					}
					else if (randomDir == 1)
					{
						diffVector = -SpiralGalaxyPlaneDirection1*distance + (g_SpiralGalaxyFlatnessFactor*dot3)*GalacticAxis;
					}


					distanceFactor = pow(distance, g_SpiralGalaxyDistanceExponent);


					rotation.FromAngleAxis(frnd(g_SpiralGalaxyTwistingFactorMin, g_SpiralGalaxyTwistingFactorMax)*distanceFactor*M_RADTODEG, GalacticAxis);
					rotationMat = rotation.RotationMatrix();

					DiffVectorRotated = rotationMat*diffVector;

					DiffVectorRotated *= pow(g_GalaxyStellarDistanceIncreaseFactor1, g_GalaxyStellarDistanceIncreaseFactor2*distance);

					stellarPositions[i] = DiffVectorRotated + centerPos;
				}
			}
		}

		maxStars_ = counter;

		maxLocalGroupDistanceSq_ = 900.0f;

		float RandomSpectralValue= 0.0f;
		long SpectralType =0;

		float SunScale= 0.0f;
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		BillboardSet* billboardObject = GetNode()->CreateComponent<BillboardSet>();
		billboardObject->SetNumBillboards(maxStars_);
		billboardObject->SetMaterial(cache->GetResource<Material>("Materials/Particle.xml"));
	//	billboardObject->SetSorted(true);
		int index = 0;
		for (i = 0; i < numSunsMax; i++)
		{
			if (useStellarPositions[i] == true)
			{
				RandomSpectralValue = frnd(0.0f, 1.0);

// 				if (RandomSpectralValue >= g_SunBlue_DistributionValueMin && RandomSpectralValue < g_SunBlue_DistributionValueMax)
// 				{
// 					SpectralType = 0;
// 					SunScale = frnd(g_SunBlue_MinScale, g_SunBlue_MaxScale);
// 				}
// 				else if (RandomSpectralValue >= g_SunOrange_DistributionValueMin && RandomSpectralValue < g_SunOrange_DistributionValueMax)
// 				{
// 					SpectralType = 1;
// 					SunScale = frnd(g_SunOrange_MinScale, g_SunOrange_MaxScale);
// 				}
// 				else if (RandomSpectralValue >= g_SunRedDwarf_DistributionValueMin && RandomSpectralValue < g_SunRedDwarf_DistributionValueMax)
// 				{
// 					SpectralType = 2;
// 					SunScale = frnd(g_SunRedDwarf_MinScale, g_SunRedDwarf_MaxScale);
// 				}
// 				else if (RandomSpectralValue >= g_SunRedGiant_DistributionValueMin && RandomSpectralValue < g_SunRedGiant_DistributionValueMax)
// 				{
// 					SpectralType = 3;
// 					SunScale = frnd(g_SunRedGiant_MinScale, g_SunRedGiant_MaxScale);
// 				}
// 				else if (RandomSpectralValue >= g_SunYello_DistributionValueMin && RandomSpectralValue < g_SunYello_DistributionValueMax)
// 				{
// 					SpectralType = 4;
// 					SunScale = frnd(g_SunYello_MinScale, g_SunYello_MaxScale);
// 				}
// 				else
// 				{
// 					SpectralType = 4;
// 					SunScale = frnd(g_SunYello_MinScale, g_SunYello_MaxScale);
// 				}
				SunScale = 1.0f;
				CreateSun(stellarPositions[i], SunScale, SpectralType);

				Billboard* bb = billboardObject->GetBillboard(index);
				if (bb)
				{
					bb->position_ = stellarPositions[i];
					bb->size_ = Vector2(Random(2.0f) + 3.0f, Random(2.0f) + 3.0f);
					bb->rotation_ = Random() * 360.0f;
					bb->enabled_ = true;
				}
				index++;
			}

		}
		// After modifying the billboards, they need to be "commited" so that the BillboardSet updates its internals
		billboardObject->Commit();
	}

	void Galaxy::DelayedStart()
	{

	}

	void Galaxy::Stop()
	{

	}

	void Galaxy::Update(float timeStep)
	{

	}

	void Galaxy::CreateSun(const Vector3& pos, float scale, int type)
	{

	}

}