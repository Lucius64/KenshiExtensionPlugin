#pragma once
#include <ogre/OgrePrerequisites.h>
#include <kenshi/util/hand.h>
#include <kenshi/Building.h>

class DoorLock;

// KenshiLibでは未定義
class UseableStuff : public Building
{
public:
	hand _0x360;
	hand _0x380;
	bool hasProcessBarWhenUsed;
	int _0x3a4;
	bool occupantSelection;
	bool requireOperator;
	float maxOperators;
	int hungerRate;
	bool getsBattery;
	bool powerOn;
	bool broken;
	int _0x3b8;
	float powerOutput;
	float _0x3c0;
	float batteryCharge;
	float powerCapacity;
	Ogre::set<hand>::type operators;
	StatsEnumerated statUsed;
	GameData* functionality;
	GameData* animation;
	GameData* animationKO;
	GameData* animationDazed;
	float useRange;
	float _0x424;
	float _0x428;
	Inventory* inventory;
	DoorLock* lock;

	virtual ~UseableStuff();
};
