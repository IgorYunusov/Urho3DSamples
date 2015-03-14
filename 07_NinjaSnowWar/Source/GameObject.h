
#pragma once


#include  <Urho3D/Scene/LogicComponent.h>


namespace Urho3D
{
	class Scene;
}


// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;


///Game Events.
EVENT(GE_DIE, GameObjectDie)
{
	PARAM(P_NODE, Nodeptr);               
	PARAM(P_GAMEOBJECT, GameObjectptr);              
}
EVENT(GE_POINTS, GameObjectPoints)
{
	PARAM(P_POINTS, Points);                // int
	PARAM(P_RECIVER, Receiver);              // uint
	PARAM(P_DAMAGESIDE, DamageSide);		// int

}

EVENT(GE_KILL, GameObjectKill)
{
	PARAM(P_POINTS, Points);                // int
	PARAM(P_RECIVER, Receiver);              // uint
	PARAM(P_DAMAGESIDE, DamageSide);		// int

}


const int CTRL_UP = 1;
const int CTRL_DOWN = 2;
const int CTRL_LEFT = 4;
const int CTRL_RIGHT = 8;
const int CTRL_FIRE = 16;
const int CTRL_JUMP = 32;
const int CTRL_ALL = 63;

const int SIDE_NEUTRAL = 0;
const int SIDE_PLAYER = 1;
const int SIDE_ENEMY = 2;

class GameObject : public LogicComponent
{
	OBJECT(GameObject);
public:
    GameObject( Context* context );
	virtual ~GameObject();

	/// Register object factory.
	static void RegisterObject(Context* context);

	//////////////////////////////////////////////////////////////////////////
	/// LogicComponent
	//////////////////////////////////////////////////////////////////////////
	/// Called on physics update, fixed timestep.
	virtual void FixedUpdate(float timeStep);

	//////////////////////////////////////////////////////////////////////////
	/// GameObject Actions
	//////////////////////////////////////////////////////////////////////////
	virtual bool Damage(GameObject* origin, int amount);
	virtual bool Heal(int amount);
	virtual void PlaySound(const String& soundName);
	static Node* SpawnParticleEffect(const Node* node, const Vector3& pos, const String& effectName, float duration, CreateMode mode = REPLICATED);
	static Node* SpawnObject(const Node* node,const Vector3& pos, const Quaternion& rot, const String& className);
	static Node* SpawnSound(const Node* node, const Vector3& pos, const String& soundName, float duration);
	//////////////////////////////////////////////////////////////////////////
	/// Handle Collisions
	//////////////////////////////////////////////////////////////////////////
	virtual void HandleNodeCollision(StringHash eventType, VariantMap& eventData);
	virtual void WorldCollision(VariantMap& eventData);
	virtual void ObjectCollision(GameObject* otherObject, VariantMap& eventData);
	virtual void ResetWorldCollision();

	//////////////////////////////////////////////////////////////////////////
	/// Getters/Setters
	//////////////////////////////////////////////////////////////////////////
	void SetDuration(float d) { duration = d; }
	void SetSide(int s) { side = s; }
	void SetCreatorID(unsigned origin) { creatorID = origin; }
	void SetHealth(int hp);
	void SetMaxHealth(int maxhp) { maxHealth = maxhp; }
	void SetLastDamageSide(int last) { lastDamageSide = last; }


	float GetDuration() const { return duration; }
	int GetSide() { return side; }
	unsigned GetCreatorID() { return creatorID; }
	int GetHealth(){ return health; }
	int GetMaxHealth() { return maxHealth; }

	bool IsSliding() const { return isSliding; }
	bool OnGround() const { return onGround; }
protected:

	bool onGround;
	bool isSliding;
	float duration;

	int health;
	int maxHealth;

	int side;
	int lastDamageSide;
	unsigned int lastDamageCreatorID;
	unsigned int creatorID;


};

