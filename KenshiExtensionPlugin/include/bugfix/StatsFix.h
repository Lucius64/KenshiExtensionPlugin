#pragma once
#include <kenshi/CharStats.h>

namespace KEP
{
	namespace StatsFix
	{
		void CharStats_xpMelee_hook(CharStats* self, CharStats::DeadTimeState what, Character* target, const Damages& damage);
		void CharStats_xpLockpicking_hook(CharStats* self, int lockLevel, bool success);
		int CharStats_skillBonusAttack_unarmed_hook(CharStats* self, bool factorEnvironment);
		std::string CharStats_getStatName_hook(StatsEnumerated what);
		void Character_recalculateTotalEquipmentSkillBonus_hook(Character* self);
		void init();
	}
}