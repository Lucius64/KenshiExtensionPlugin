#pragma once
#include <ogre/OgrePrerequisites.h>

class SquadManagementScreen;
class Character;
class GameDataCopyStandalone;
class GameSaveState;
class ActivePlatoon;
class Faction;
class PlayerInterface;
template<typename T>
class lektor;

namespace KEP
{
	namespace CharacterExtension
	{
		void SquadManagementScreen_FUN_0048EFA0_hook(SquadManagementScreen* self, int flag);
		bool Character_giveBirth_hook(Character* self, GameDataCopyStandalone* appearance, const Ogre::Vector3& position, const Ogre::Quaternion& rotation, GameSaveState* state, ActivePlatoon* tempplatoonptr, Faction* _faction);
		bool PlayerInterface_recruit_hook(PlayerInterface* self, const lektor<Character*>& characters, bool editor);
		void init();
	}
}