#include <boost/locale.hpp>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GlobalConstants.h>
#include <kenshi/Character.h>
#include <kenshi/RaceData.h>
#include <kenshi/Building.h>
#include <kenshi/Damages.h>

#include <extern/RobotLimbItem.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <bugfix/StatsFix.h>


namespace
{
	void (*CharStats_xpMelee_orig)(CharStats*, CharStats::DeadTimeState, Character*, const Damages&);
	void (*CharStats_xpLockpicking_orig)(CharStats*, int, bool);
	int (*CharStats_skillBonusAttack_unarmed_orig)(CharStats*, bool);
	std::string(*CharStats_getStatName_orig)(StatsEnumerated);


	StatsEnumerated getStatsEnumeratedFromWeaponCategory(WeaponCategory weaponCategory)
	{
		switch (weaponCategory)
		{
		case SKILL_KATANAS:
			return STAT_KATANAS;

		case SKILL_SABRES:
			return STAT_SABRES;

		case SKILL_BLUNT:
			return STAT_BLUNT;

		case SKILL_HEAVY:
			return STAT_HEAVYWEAPONS;

		case SKILL_HACKERS:
			return STAT_HACKERS;

		case SKILL_UNARMED:
			return STAT_MARTIALARTS;

		case SKILL_BOW:
			return STAT_CROSSBOWS;

		case SKILL_TURRET:
			return STAT_TURRETS;

		case ATTACK_POLEARMS:
			return STAT_POLEARMS;
		default:
			break;
		}
		return STAT_MARTIALARTS;
	}

	template<bool IsUnarmed, CharStats::DeadTimeState What>
	void CharStats_xpMelee_hook_helper(CharStats* self, Character* target, const Damages& damage)
	{
		KEP::externalGlobals->_gBaseXpCombat->_skillXp = static_cast<float>(con->EXPERIENCE_GAIN * 0.10000000149011612);
		KEP::externalGlobals->_gBaseXpCombat->_attributeXp = KEP::externalGlobals->_gBaseXpCombat->_skillXp;

		float strengthExp = static_cast<float>(con->STRENGTH_XP_RATE * 2.0);
		auto& skillBonusRace = self->me->myRace->statMods;

		if (What == CharStats::ATTACK_HIT || What == CharStats::ATTACK_WAS_BLOCKED || What == CharStats::ATTACK_MISSED)
		{
			float xpBonusSkillDiff = self->getSkillDifferenceRatio(self->getMeleeAttack(), target->stats->getMeleeDefence(false));

			float* pSkill;
			StatsEnumerated combatStatsEnum;
			if (IsUnarmed)
			{
				pSkill = &self->unarmed;
				combatStatsEnum = STAT_MARTIALARTS;
			}
			else
			{
				pSkill = &self->__meleeAttack;
				combatStatsEnum = STAT_MELEE_ATTACK;
			}

			float expMult;
			if (What == 0)
				expMult = 1.0f;
			else if (What == 1)
				expMult = 0.5f;
			else if (What == 6)
				expMult = 0.25f;

			float exp = KEP::externalGlobals->_gBaseXpCombat->_skillXp * expMult * xpBonusSkillDiff * skillBonusRace[combatStatsEnum] * KEP::externalGlobals->_optionsAdvanced->_gdm;

			KEP::externalFunctions->FUN_008C4F20(*pSkill, exp, 101.0f);

			float dexExpRate = 1.0f;
			if (!IsUnarmed)
			{
				if (What == CharStats::ATTACK_HIT || What == CharStats::ATTACK_WAS_BLOCKED)
					if (self->bleedDamageMult + self->cutDamageMult + self->pierceDamageMult > 0.0f)
						dexExpRate = (1.0f / (self->bluntDamageMult + self->cutDamageMult + self->pierceDamageMult)) * (self->pierceDamageMult + self->cutDamageMult);

				float xpBonusSkillDiffForWeapon = self->getSkillDifferenceRatio(self->getEquippedWeaponSkill(), target->stats->getMeleeDefence(false));

				float weaponExpMult = 1.0f;
				if (What == 1)
					weaponExpMult = 0.4f;
				else if (What == 6)
					weaponExpMult = 0.1f;

				exp = KEP::externalGlobals->_gBaseXpCombat->_skillXp * weaponExpMult * xpBonusSkillDiffForWeapon * skillBonusRace[getStatsEnumeratedFromWeaponCategory(self->currentWeaponType)] * KEP::externalGlobals->_optionsAdvanced->_gdm;
				KEP::externalFunctions->FUN_008C4F20(*self->pCurrentWeaponSkill, exp, 101.0f);
			}

			if (What == CharStats::ATTACK_HIT || What == CharStats::ATTACK_WAS_BLOCKED)
			{
				exp = KEP::externalGlobals->_gBaseXpCombat->_attributeXp * expMult * xpBonusSkillDiff * dexExpRate * skillBonusRace[STAT_DEXTERITY] * KEP::externalGlobals->_optionsAdvanced->_gdm;
				KEP::externalFunctions->FUN_008C4F20(self->_dexterity, exp, 101.0f);
			}
		}
		else if (What == CharStats::BLOCKED_IT)
		{
			float xpBonusSkillDiff = self->getSkillDifferenceRatio(self->getMeleeDefence(false), target->stats->getMeleeAttack());
			float exp = KEP::externalGlobals->_gBaseXpCombat->_skillXp * 0.25f * xpBonusSkillDiff * skillBonusRace[STAT_MELEE_DEFENCE] * KEP::externalGlobals->_optionsAdvanced->_gdm;
			KEP::externalFunctions->FUN_008C4F20(self->_meleeDefence, exp, 101.0f);

			float xpBonusSkillDiffForWeapon = self->getSkillDifferenceRatio(self->getEquippedWeaponSkill(), target->stats->getMeleeAttack());

			exp = KEP::externalGlobals->_gBaseXpCombat->_skillXp * 0.1f * xpBonusSkillDiffForWeapon * skillBonusRace[getStatsEnumeratedFromWeaponCategory(self->currentWeaponType)] * KEP::externalGlobals->_optionsAdvanced->_gdm;
			KEP::externalFunctions->FUN_008C4F20(*self->pCurrentWeaponSkill, exp, 101.0f);

			strengthExp *= 0.4f;
		}
		else if (What == CharStats::WAS_HIT)
		{
			if (IsUnarmed)
			{
				self->xpDodgeEvent(target->stats->getMeleeAttack(), false);
			}
			else
			{
				float xpBonusSkillDiff = self->getSkillDifferenceRatio(self->getMeleeDefence(false), target->stats->getMeleeAttack());

				float exp = KEP::externalGlobals->_gBaseXpCombat->_skillXp * 2.0f * xpBonusSkillDiff * skillBonusRace[STAT_MELEE_DEFENCE] * KEP::externalGlobals->_optionsAdvanced->_gdm;

				KEP::externalFunctions->FUN_008C4F20(self->_meleeDefence, exp, 101.0f);
			}

			float xpBonusSkillDiffForToughness = self->getSkillDifferenceRatio(self->ageMult * self->_toughness, damage.blunt + damage.cut + damage.pierce + damage.extraStun);

			float toughnessExp = con->XP_TOUGHNESS * KEP::externalGlobals->_gBaseXpCombat->_skillXp * xpBonusSkillDiffForToughness * skillBonusRace[STAT_TOUGHNESS] * KEP::externalGlobals->_optionsAdvanced->_gdm;

			KEP::externalFunctions->FUN_008C4F20(self->_toughness, toughnessExp, 101.0f);

			strengthExp *= 0.1f;
		}

		if (IsUnarmed)
		{
			strengthExp *= (1.0f - self->encumbranceMult);
			if (strengthExp <= 0.0f)
				return;

			strengthExp = KEP::externalGlobals->_gBaseXpCombat->_attributeXp * 0.55f * strengthExp;
		}
		else
		{
			strengthExp = KEP::externalGlobals->_gBaseXpCombat->_attributeXp * 0.55f * strengthExp * self->weaponWeightXPMult;
		}

		strengthExp = strengthExp * skillBonusRace[STAT_STRENGTH] * KEP::externalGlobals->_optionsAdvanced->_gdm;

		KEP::externalFunctions->FUN_008C4F20(self->_strength, strengthExp, 101.0f);
		return;
	}
}

void KEP::StatsFix::CharStats_xpMelee_hook(CharStats* self, CharStats::DeadTimeState what, Character* target, const Damages& damage)
{
	if (!settings._fixMeleeCombatXP)
	{
		CharStats_xpMelee_orig(self, what, target, damage);
		return;
	}

	if (target->getDataType() != CHARACTER)
		return;

	if (what == CharStats::ATTACK_HIT)
	{
		if (self->currentWeaponType == SKILL_UNARMED)
			CharStats_xpMelee_hook_helper<true, CharStats::ATTACK_HIT>(self, target, damage);
		else
			CharStats_xpMelee_hook_helper<false, CharStats::ATTACK_HIT>(self, target, damage);
	}
	else if (what == CharStats::ATTACK_WAS_BLOCKED)
	{
		if (self->currentWeaponType == SKILL_UNARMED)
			CharStats_xpMelee_hook_helper<true, CharStats::ATTACK_WAS_BLOCKED>(self, target, damage);
		else
			CharStats_xpMelee_hook_helper<false, CharStats::ATTACK_WAS_BLOCKED>(self, target, damage);
	}
	else if (what == CharStats::BLOCKED_IT)
	{
		CharStats_xpMelee_hook_helper<false, CharStats::BLOCKED_IT>(self, target, damage);
	}
	else if (what == CharStats::WAS_HIT)
	{
		if (self->currentWeaponType == SKILL_UNARMED)
			CharStats_xpMelee_hook_helper<true, CharStats::WAS_HIT>(self, target, damage);
		else
			CharStats_xpMelee_hook_helper<false, CharStats::WAS_HIT>(self, target, damage);
	}
	else if (what == CharStats::ATTACK_MISSED)
	{
		if (self->currentWeaponType == SKILL_UNARMED)
			CharStats_xpMelee_hook_helper<true, CharStats::ATTACK_MISSED>(self, target, damage);
		else
			CharStats_xpMelee_hook_helper<false, CharStats::ATTACK_MISSED>(self, target, damage);
	}
	else
	{
		if (self->currentWeaponType == SKILL_UNARMED)
			CharStats_xpMelee_hook_helper<true, CharStats::ATTACK_SLOT_OPEN>(self, target, damage);
		else
			CharStats_xpMelee_hook_helper<false, CharStats::ATTACK_SLOT_OPEN>(self, target, damage);
	}

	return;
}

void KEP::StatsFix::CharStats_xpLockpicking_hook(CharStats* self, int lockLevel, bool success)
{
	if (!settings._fixLockpickingXP)
	{
		CharStats_xpLockpicking_orig(self, lockLevel, success);
		return;
	}

	float baseXp = success ? 1.6f : 0.32f; // 1.2 * 4/3, 0.24 * 4/3
	float exp = static_cast<float>(self->me->myRace->statMods[STAT_LOCKPICKING] * baseXp * con->EXPERIENCE_GAIN);
	externalFunctions->FUN_008C4F20(self->lockpicking, exp, 101.0f);
}

int KEP::StatsFix::CharStats_skillBonusAttack_unarmed_hook(CharStats* self, bool factorEnvironment)
{
	if (!settings._fixUnarmedDamageBonus)
		return CharStats_skillBonusAttack_unarmed_orig(self, factorEnvironment);

	float bonus = static_cast<float>(self->skillBonusUnarmed);
	auto leftArm = self->me->medical.leftArm->robotLimb;
	if (leftArm != nullptr)
		bonus += leftArm->unarmedDamageBonus;

	auto rightArm = self->me->medical.rightArm->robotLimb;
	if (rightArm != nullptr)
		bonus += rightArm->unarmedDamageBonus;

	auto leftLeg = self->me->medical.leftLeg->robotLimb;
	if (leftLeg != nullptr)
		bonus += leftLeg->unarmedDamageBonus;

	auto rightLeg = self->me->medical.rightLeg->robotLimb;
	if (rightLeg != nullptr)
		bonus += rightLeg->unarmedDamageBonus;

	if (factorEnvironment)
	{
		auto& indoorHandle = self->me->isIndoors();
		if (indoorHandle.type != NULL_ITEM)
		{
			if (!self->me->isOnARoof())
			{
				auto building = indoorHandle.getBuilding();
				if (building != nullptr && !building->isCavernous)
				{
					if (bonus > 0.0f && self->skillBonusIndoors < 0)
						bonus = 0.0f;

					bonus += self->skillBonusIndoors;
				}
			}
		}

		bonus -= self->me->medical.getWeatherStatPenalty(1.0f);
	}

	return static_cast<int>(bonus);
}

std::string KEP::StatsFix::CharStats_getStatName_hook(StatsEnumerated what)
{
	if (!settings._fixStatNameDisplay)
		return CharStats_getStatName_orig(what);

	const char* id;
	switch (what)
	{
	case STAT_STRENGTH:
		id = "Strength";
		break;
	case STAT_MELEE_ATTACK:
		id = "Attack";
		break;
	case STAT_LABOURING:
		id = "Labouring";
		break;
	case STAT_SCIENCE:
		id = "Science";
		break;
	case STAT_ENGINEERING:
		id = "Engineering";
		break;
	case STAT_ROBOTICS:
		id = "Robotics";
		break;
	case STAT_SMITHING_WEAPON:
		id = "Weapon Smithing";
		break;
	case STAT_SMITHING_ARMOUR:
		id = "Armour Smithing";
		break;
	case STAT_MEDIC:
		id = "First Aid";
		break;
	case STAT_THIEVING:
		id = "Thievery";
		break;
	case STAT_TURRETS:
		id = "Turrets";
		break;
	case STAT_FARMING:
		id = "Farming";
		break;
	case STAT_COOKING:
		id = "Cooking";
		break;
	case STAT_HIVEMEDIC:
	case STAT_VET:
		id = "Vet";
		break;
	case STAT_STEALTH:
		id = "Stealth";
		break;
	case STAT_ATHLETICS:
		id = "Athletics";
		break;
	case STAT_DEXTERITY:
		id = "Dexterity";
		break;
	case STAT_MELEE_DEFENCE:
		id = "Defence";
		break;
	case STAT_WEAPONS:
		id = "Weaponry";
		break;
	case STAT_TOUGHNESS:
		id = "Toughness";
		break;
	case STAT_ASSASSINATION:
		id = "Assassination";
		break;
	case STAT_SWIMMING:
		id = "Swimming";
		break;
	case STAT_PERCEPTION:
		id = "Perception";
		break;
	case STAT_KATANAS:
		id = "Katanas";
		break;
	case STAT_SABRES:
		id = "Sabres";
		break;
	case STAT_HACKERS:
		id = "Hackers";
		break;
	case STAT_HEAVYWEAPONS:
		id = "Heavy weapons";
		break;
	case STAT_BLUNT:
		id = "Blunt";
		break;
	case STAT_MARTIALARTS:
		id = "Martial arts";
		break;
	case STAT_DODGE:
		id = "Dodging";
		break;
	case STAT_POLEARMS:
		id = "Polearms";
		break;
	case STAT_CROSSBOWS:
		id = "Crossbows";
		break;
	case STAT_FRIENDLY_FIRE:
		id = "Precision Shooting";
		break;
	case STAT_LOCKPICKING:
		id = "Lockpicking";
		break;
	case STAT_SMITHING_BOW:
		id = "Crossbow Smithing";
		break;
	case _encumbrance:
		id = "Encumbrance";
		break;
	case _combatSpeed:
		id = "Combat Speed";
		break;
	default:
		return "";
	}

	return (boost::locale::translate(id).*externalFunctions->FUN_000A9580)();
}

void KEP::StatsFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharStats::xpMelee), &CharStats_xpMelee_hook, &CharStats_xpMelee_orig))
		ErrorLog("KenshiExtensionPlugin: [xp melee] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharStats::xpLockpicking), &CharStats_xpLockpicking_hook, &CharStats_xpLockpicking_orig))
		ErrorLog("KenshiExtensionPlugin: [xp lockpicking] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharStats::skillBonusAttack_unarmed), &CharStats_skillBonusAttack_unarmed_hook, &CharStats_skillBonusAttack_unarmed_orig))
		ErrorLog("KenshiExtensionPlugin: [unarmed damage bonus] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharStats::getStatName), &CharStats_getStatName_hook, &CharStats_getStatName_orig))
		ErrorLog("KenshiExtensionPlugin: [stat name] could not install hook!");
}
