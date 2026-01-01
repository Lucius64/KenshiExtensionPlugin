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
		void init();
	}
}