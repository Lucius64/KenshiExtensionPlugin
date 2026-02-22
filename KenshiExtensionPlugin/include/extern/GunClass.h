#pragma once
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreVector3.h>
#include <ogre/OgreQuaternion.h>
#include <ogre/OgreSharedPtr.h>

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
	Ogre::MaterialPtr _0x70;
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
	virtual Ogre::Vector3 vfunc0x48();
	virtual Ogre::Vector3 vfunc0x50(float);
	virtual bool vfunc0x58(void*);
	virtual void vfunc0x60();
	virtual void vfunc0x68();
	virtual void vfunc0x70();
	virtual void vfunc0x78(float);
	virtual void vfunc0x80(float, const hand&);
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
