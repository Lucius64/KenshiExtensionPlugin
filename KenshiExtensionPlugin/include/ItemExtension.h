#pragma once
#include <ogre/OgrePrerequisites.h>

class GameData;
class CharStats;
class Weapon;
class Sword;

namespace KEP
{
	namespace ItemExtension
	{
		void Sword_setupStats_hook(Sword* self, int _level, GameData* baseData, GameData* companyData, GameData* materialData);
		void CharStats_setWeapon_hook(CharStats* self, Weapon* _weapon);
		void init();
	}
}