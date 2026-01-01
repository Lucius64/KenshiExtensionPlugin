#pragma once
#include <ogre/OgrePrerequisites.h>

class ActivePlatoon;
class Character;
class Faction;
class ManagementScreen;

class SquadManagementScreen
{
public:
	class SquadData;

	size_t _0x0;
	size_t _0x8;
	size_t _0x10;
	int _0x18;
	size_t _0x20;
	size_t _0x28;
	Character* character;
	Faction* faction;
	Ogre::map<ActivePlatoon*, SquadData*>::type _0x40;
};