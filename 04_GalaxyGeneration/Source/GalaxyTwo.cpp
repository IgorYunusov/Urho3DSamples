


#include "..\Urho3D.h"
#include "..\IO\Log.h"
#include "..\Core\Context.h"
#include "..\Scene\Serializable.h"
#include "..\Graphics\BillboardSet.h"
#include "..\Scene\Component.h"
#include "..\Scene\Node.h"
#include "..\Scene\Scene.h"
#include "..\Graphics\Material.h"
#include "..\Resource\ResourceCache.h"
#include "..\Math\Vector2.h"
#include "..\Math\MathDefs.h"
#include "GalaxyTwo.h"
#include "open-simplex-noise.h"
#define sind(x) (sin(fmod((x),360) * M_PI / 180))
#define cosd(x) (cos(fmod((x),360) * M_PI / 180))
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


	GalaxyTwo::GalaxyTwo(Context* context) : LogicComponent(context),
		size_(60),
		nebulaFade_(0.333f),
		numHub_(1000),
		numDisk_(4000),
		diskRad_(120.0f),
		hubRad_(85.0f),
		numArms_(4),
		armRots_(2.0f),
		armWidth_(80.0f),
		fuzz_(45.0f)
	{
		SetUpdateEventMask(USE_UPDATE);

	}

	GalaxyTwo::~GalaxyTwo()
	{

	}

	void GalaxyTwo::RegisterObject(Context* context)
	{
		context->RegisterFactory<GalaxyTwo>();


		ACCESSOR_ATTRIBUTE("Name", GetName, SetName, String, String::EMPTY, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("Size", GetSize, SetSize, int, 60, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("NebulaFade", GetNebulaFade, SetNebulaFade, float, 0.333f, AM_DEFAULT);

		ACCESSOR_ATTRIBUTE("NumHub", GetNumHub, SetNumHub, int, 2000, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("NumDisk", GetNumDisk, SetNumDisk, int, 4000, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("DiskRad", GetDiskRad, SetDiskRad, float, 90.0f, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("HubRad", GetHubRad, SetHubRad, float, 45.0f, AM_DEFAULT);

		ACCESSOR_ATTRIBUTE("NumArms", GetNumArms, SetNumArms, int, 2, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("ArmRots", GetArmRots, SetArmRots, float, 1.0f, AM_DEFAULT);
		ACCESSOR_ATTRIBUTE("ArmWidth", GetArmWidth, SetArmWidth, float, 60.0f, AM_DEFAULT);

		ACCESSOR_ATTRIBUTE("Fuzz", GetFuzz, SetFuzz, float, 25.0f, AM_DEFAULT);
	}

	void GalaxyTwo::Start()
	{
		/// create galaxy 
		/*
		PODVector<int> buffer;
		buffer.Resize(size_*size_);

		float armSeparation = 0.0f, angle = 0.0f, dist = 0.0f, maxim = 0.0f;

		Vector<Vector2> stars;
		stars.Resize(numHub_ + numDisk_);

		/// Arms
		armSeparation = 360.0f / numArms_;
		for (int i = 0; i < numDisk_; ++i) 
		{
			dist = hubRad_ + Random() * diskRad_ ;
			// This is the clever bit, that puts a star at a given distance
			// into an arm: First, it wraps the star round by the number of
			// rotations specified. By multiplying the distance by the number of
			// rotations the rotation is proportional to the distance from the
			// center, to give curvature
			angle = ((360.0 * armRots_ * (dist / diskRad_)) +
				Random() * armWidth_ + // move the point further around by a random factor up to ARMWIDTH
				(armSeparation*Random(1, numArms_)) + // multiply the angle by a factor of armSeparation, putting the point into one of the arms
				Random() * fuzz_ * 2.0 - fuzz_); // add a further random factor, fuzzing the edge of the arms
			
			//  Convert to cartesian
			stars[i].x_ = cosd(angle) * dist;
			stars[i].y_ = sind(angle) * dist;
			maxim = Max(maxim, dist);
		}

		/// Center
		for (int i = numDisk_; i < numDisk_ + numHub_; ++i) 
		{
			dist =Random() * hubRad_;
			angle = Random() * 360.0f;
			stars[i].x_ = cosd(angle) * dist;
			stars[i].y_ = sind(angle) * dist;
			maxim = Max(maxim, dist);
		}

		int sx = 0, sy = 0;

		// Fit the galaxy to the requested size
		float factor = size_ / (maxim * 2);
		for (int i = 0; i < numHub_ + numDisk_; ++i) 
		{
			sx = MapRange(stars[i].x_, -maxim, maxim, 0, size_ - 1);
			sy = MapRange(stars[i].y_, -maxim, maxim, 0, size_ - 1);
			buffer[size_*sy + sx] = Min(255, floor(buffer[size_*sy + sx]) + 1);
		}

		struct osn_context *ctx;
		open_simplex_noise(77374, &ctx);

		// Create tiles
	//	var simplex_neb = new SimplexNoise(new Alea('galaxy_noise'));
// 		char STARS[] = { ' ', '✦', '★', '☀', '✶', '✳', '✷', '✸' }; // ✧✦☼☀✳☆★✶✷✸

		double value;
		uint32_t rgb;
		int block;
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		BillboardSet* billboardObject = GetNode()->CreateComponent<BillboardSet>();
		billboardObject->SetNumBillboards(numDisk_ + numHub_);
		billboardObject->SetMaterial(cache->GetResource<Material>("Materials/Particle.xml"));

		for (int j = 0; j < numDisk_ + numHub_; ++j)
		{
// 				value = open_simplex_noise2(ctx, (double)i*0.05, (double)j*0.05);				
// 				rgb = 0x010101 * (uint32_t)((value + 1) * 127.5); 
// 				uint32_t image3d = (0x0ff << 24) | (rgb);
 				Billboard* bb = billboardObject->GetBillboard( j );
				if (bb)
				{
					bb->position_.x_ = stars[j].x_;
					bb->position_.y_ = stars[j].y_;
					bb->size_ = Vector2(Random(2.0f) + 3.0f, Random(2.0f) + 3.0f);
					bb->rotation_ = Random() * 360.0f;
					bb->enabled_ = true;
				}


// 				int bg = (rgb *(int) nebulaFade_) | 0;
// 				int star = Min(100 + buffer[size_*j + i] * 20, 255);
//				block = isNaN(star) ? " " : STARS[~~mapRange(star, 100, 255, 0, STARS.length - 1)];
// 				buffer[size_*j+i] = new ut.Tile(block, star, star, star, bg, bg, bg);
// 				buffer[size_*j+i].desc = "Millions of stars";			
		}

// 		//	billboardObject->SetSorted(true);
// 		int index = 0;
// 		for (i = 0; i < numSunsMax; i++)
// 		{
// 		
// 
// 
// 				Billboard* bb = billboardObject->GetBillboard(index);
// 				if (bb)
// 				{
// 					bb->position_ = stellarPositions[i];
// 					bb->size_ = Vector2(Random(2.0f) + 3.0f, Random(2.0f) + 3.0f);
// 					bb->rotation_ = Random() * 360.0f;
// 					bb->enabled_ = true;
// 				}
// 				index++;
// 		}
// 
// 		// After modifying the billboards, they need to be "commited" so that the BillboardSet updates its internals
// 		billboardObject->Commit();

		stars.Clear();
		buffer.Clear();
		open_simplex_noise_free(ctx);
		*/
	}

	void GalaxyTwo::DelayedStart()
	{

	}

	void GalaxyTwo::Stop()
	{

	}

	void GalaxyTwo::Update(float timeStep)
	{

	}

	void GalaxyTwo::CreateSun(const Vector3& pos, float scale, int type)
	{

	}

	float GalaxyTwo::MapRange(float s, float a1, float a2, float b1, float b2)
	{
		return b1 + (s - a1)*(b2 - b1) / (a2 - a1);
	}

	void GalaxyTwo::generateGalaxy(float Size, int nrArms, int width, int beer, int colorType /*= -1*/, float centerPercent /*= 33*/, float centerScalePercent /*= 25*/, float axialScale /*= 0.75f*/)
	{
		float nx, ny, nz;
	
		int nrParticles = numDisk_ + numHub_;

		int k = 1;
		Vector<Vector3> stars;
		stars.Resize(nrParticles);

		ResourceCache* cache = GetSubsystem<ResourceCache>();
		BillboardSet* billboardObject = GetNode()->CreateComponent<BillboardSet>();
		billboardObject->SetNumBillboards(numDisk_ + numHub_);
		billboardObject->SetMaterial(cache->GetResource<Material>("Materials/Particle.xml"));

		for (int i = 0; i < nrParticles; i++)
		{

			float armAngle = 360 / nrArms;			//unghiul unui bratz

			float raza = rand() % (int)Size;		//raza maxima

			float unghi = rand() % width
				+
				raza			//sa "invarta" bratele
				+
				rand() % nrArms * armAngle
				+
				rand() % beer * 2 - beer;

			nx = raza * cos(unghi * M_PI / 180.0f);
			ny = raza * sin(unghi * M_PI / 180.0f);
			nz = 0;//(irr::f32)rand()/RAND_MAX;	

			float dist = Vector3(nx, ny, nz).Length();

			stars[i] = Vector3(nx * 2, ny * 2, nz * 2);

			if (i < centerPercent*nrParticles / 100)
			{
				float o, t, r;
				o = ((float)(rand() % 360));
				t = (90 - (float)(rand() % 180));
				r = ((float)rand() / RAND_MAX) * Size * centerScalePercent / 100;
				nx = r * cos(o)*sin(t);
				ny = r * sin(o)*sin(t);
				nz = r * cos(t);
				stars[i] = Vector3(nx * 2, ny * 2, nz * 2 * axialScale);
			}
		}

		for (int j = 0; j < nrParticles; ++j)
		{

			Billboard* bb = billboardObject->GetBillboard(j);
			if (bb)
			{
				bb->position_.x_ = stars[j].x_;
				bb->position_.y_ = stars[j].y_;
				bb->size_ = Vector2(Random(2.0f) + 3.0f, Random(2.0f) + 3.0f);
				bb->rotation_ = Random() * 360.0f;
				bb->enabled_ = true;
			}

	
		}

	}

}