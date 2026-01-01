#pragma once
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreVector3.h>
#include <ogre/OgreQuaternion.h>
#include <kenshi/util/hand.h>

class GameData;
class ConstantTracerT;
class TurretBuilding;

// KenshiLibでは未定義
class GunClass
{
public:
	float range;
	float accuracyPerfectSkill;
	float accuracyDeviationAt0Skill;
	float aimSpeed;
	float shotSpeed;
	int numShots;
	int bulletCount;
	float reloadTimeMin;
	float reloadTimeMax;
	float userSkill;
	hand userHandle;
	GameData* reloadAnim;
	GameData* ammo;
	bool _0x60;
	bool isLoaded;
	GameData* data;
	size_t _0x70;
	size_t _0x78;
	ConstantTracerT* tracer1;
	ConstantTracerT* tracer2;
	Ogre::Vector3 barrelPos;
	lektor<Ogre::MovableObject*> objects;
	std::string liveAmmo;
	float animFrame;
	Ogre::Vector3 pos;
	Ogre::Quaternion rot;
	Ogre::MovableObject* loadedMesh;
	Ogre::MovableObject* unloadedMesh;
	bool _0x110;

	virtual ~GunClass();
	virtual void setLoadState(bool);
	virtual void setPosition(Ogre::Vector3&);
	virtual void vfunc0x18();
	virtual void vfunc0x20();
	virtual void vfunc0x28();
	virtual void vfunc0x30();
	virtual void vfunc0x38();
	virtual void vfunc0x40(bool);
};

// KenshiLibでは未定義
class GunClassTurret : public GunClass
{
public:
	Ogre::Vector3 _0x118;
	TurretBuilding* building;

	virtual ~GunClassTurret();
};

// KenshiLibでは未定義
class GunClassPersonal : public GunClass
{
public:
	int pierceDamageMin;
	int pierceDamageMax;
	bool _0x120;
	hand user;

	virtual ~GunClassPersonal();
};
