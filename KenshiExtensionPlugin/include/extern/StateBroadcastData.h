#pragma once
#include <cstdint>

#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreVector3.h>

#include <kenshi/Enums.h>
#include <kenshi/util/hand.h>

class Character;
class Faction;
class TownBase;

// KenshiLibでは未定義
enum PersonalityTags
{
	PT_NONE,
	PT_HONORABLE,
	PT_TRAITOROUS,
	PT___,
	PT__,
	PT_SMART,
	PT_DUMB,
	PT____,
	PT_____,
	PT_BRAVE,
	PT_FEARFUL,
	PT_WARM_KIND,
	PT_COLD_CRUEL,
	PT_NORMAL,
	PT_MANIACAL,
	PT_END
};

// KenshiLibでは未定義
class StateBroadcastData
{
public:
	SlaveStateEnum slaveState;
	double ssct;
	Character* character;
	Faction* _0x18;
	hand _0x20;
	bool _0x40;
	bool unavailble;
	hand _0x48;
	hand _0x68;
	TownBase* currentTown;
	float strong;
	int32_t _0x94;
	int32_t _0x98;
	float moveSpeed;
	float _0xa0;
	int32_t _0xa4;
	float _0xa8;
	float _0xac;
	int32_t _0xb0;
	int32_t sleepState;
	size_t _0xb8;
	double _0xc0;
	Ogre::Vector3 _0xc8;
	float _0xd4;
	PersonalityTags personality;
	CharacterTypeEnum npcClass;
	Faction* slaveMasterFaction;
	bool escap;
	bool kidn;
	bool tn;
	int32_t _0xec;
	float _0xf0;
	float slaveness;
	float _0xf8;
	int32_t _0xfc;
	float disguise;
	CharacterTypeEnum stigma;
	Faction* disguiseFaction;
	float disguiseblown;
	bool unprovoked;
};