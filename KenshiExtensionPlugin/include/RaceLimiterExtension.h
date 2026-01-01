#pragma once

class RaceLimiter;
class RobotLimbItem;
class GameData;
class hand;
class Weapon;
class CharacterHuman;
class GameSaveState;

namespace KEP
{
	namespace RaceLimiterExtension
	{
		bool RaceLimiter_canEquip_hook(RaceLimiter* self, GameData* item, RootObject* who);
		RobotLimbItem* RobotLimbItem__CONTRUCTOR_hook(RobotLimbItem* self, GameData* baseData, GameData* materialData, hand _handle, int _level);
		void init();
	}
}