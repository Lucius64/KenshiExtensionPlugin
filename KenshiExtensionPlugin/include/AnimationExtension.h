#pragma once
#include <ogre/OgrePrerequisites.h>

class AnimationData;
class CombatTechniqueData;
class GameData;
class RangedCombatClass;
class Character;
class hand;

namespace KEP
{
	namespace AnimationExtension
	{
		AnimationData* AnimationData__CONTRUCTOR_hook(AnimationData* self, GameData* baseData);
		CombatTechniqueData* CombatTechniqueData__CONTRUCTOR_hook(CombatTechniqueData* self, GameData* baseData);
		void RangedCombatClass_FUN_0051DA50_hook(RangedCombatClass* self, const hand& handle, const Ogre::Vector3& pos, Character* target);
		void init();
	}
}