#pragma once
#include <ogre/OgreVector3.h>
#include <kenshi/util/hand.h>

class GunClass;

class RangedCombatClass
{
public:
	int _0x0;
	float _0x4;
	hand turret;
	GunClass* gunClass;
	StatsEnumerated currentSkillUsing;
	bool _0x34;
	bool _0x35;
	bool rangedAttackMode;
	Ogre::Vector3 position;
	hand target;
	Character* me;
	float _0x70;
	float reloadTime;
	float reloadTimeRest;
	float _0x7c;
	bool _0x80;
	size_t _0x88;
};
