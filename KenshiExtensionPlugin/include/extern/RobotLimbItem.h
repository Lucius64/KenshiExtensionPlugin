#pragma once
#include <kenshi/Gear.h>

// KenshiLibでは未定義
class RobotLimbItem : public Gear
{
public:
	ogre_unordered_map<StatsEnumerated, float>::type statMods;
	float _maxHealth;
	float flesh;
	float wearDamage;
	float fleshStun;
	int unarmedDamageBonus;

	virtual ~RobotLimbItem();
};
