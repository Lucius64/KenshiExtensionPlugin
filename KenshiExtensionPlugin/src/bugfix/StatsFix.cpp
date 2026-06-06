#include <boost/locale.hpp>

#include <ogre/OgreStringConverter.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GlobalConstants.h>
#include <kenshi/Character.h>
#include <kenshi/CharStats.h>
#include <kenshi/RaceData.h>
#include <kenshi/Building/Building.h>
#include <kenshi/Damages.h>
#include <kenshi/GameplayOptions.h>

#include <extern/RobotLimbItem.h>

#include <kep/translation.h>
#include <UtilityFunction.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <bugfix/StatsFix.h>

namespace
{
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
			return STAT_MARTIALARTS;
		}
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

			float exp = KEP::externalGlobals->_gBaseXpCombat->_skillXp * expMult * xpBonusSkillDiff * skillBonusRace[combatStatsEnum] * KEP::externalGlobals->_optionsAdvanced->globalDamageMultiplier;

			increaseStat(*pSkill, exp, 101.0f);

			float dexExpRate = 1.0f;
			if (!IsUnarmed)
			{
				if (What == CharStats::ATTACK_HIT || What == CharStats::ATTACK_WAS_BLOCKED)
					if (self->bluntDamageMult + self->cutDamageMult + self->pierceDamageMult > 0.0f)
						dexExpRate = (1.0f / (self->bluntDamageMult + self->cutDamageMult + self->pierceDamageMult)) * (self->pierceDamageMult + self->cutDamageMult);

				float xpBonusSkillDiffForWeapon = self->getSkillDifferenceRatio(self->getEquippedWeaponSkill(), target->stats->getMeleeDefence(false));

				float weaponExpMult = 1.0f;
				if (What == 1)
					weaponExpMult = 0.4f;
				else if (What == 6)
					weaponExpMult = 0.1f;

				exp = KEP::externalGlobals->_gBaseXpCombat->_skillXp * weaponExpMult * xpBonusSkillDiffForWeapon * skillBonusRace[getStatsEnumeratedFromWeaponCategory(self->currentWeaponType)] * KEP::externalGlobals->_optionsAdvanced->globalDamageMultiplier;
				increaseStat(*self->pCurrentWeaponSkill, exp, 101.0f);
			}

			if (What == CharStats::ATTACK_HIT || What == CharStats::ATTACK_WAS_BLOCKED)
			{
				exp = KEP::externalGlobals->_gBaseXpCombat->_attributeXp * expMult * xpBonusSkillDiff * dexExpRate * skillBonusRace[STAT_DEXTERITY] * KEP::externalGlobals->_optionsAdvanced->globalDamageMultiplier;
				increaseStat(self->_dexterity, exp, 101.0f);
			}
		}
		else if (What == CharStats::BLOCKED_IT)
		{
			float xpBonusSkillDiff = self->getSkillDifferenceRatio(self->getMeleeDefence(false), target->stats->getMeleeAttack());
			float exp = KEP::externalGlobals->_gBaseXpCombat->_skillXp * 0.25f * xpBonusSkillDiff * skillBonusRace[STAT_MELEE_DEFENCE] * KEP::externalGlobals->_optionsAdvanced->globalDamageMultiplier;
			increaseStat(self->_meleeDefence, exp, 101.0f);

			float xpBonusSkillDiffForWeapon = self->getSkillDifferenceRatio(self->getEquippedWeaponSkill(), target->stats->getMeleeAttack());

			exp = KEP::externalGlobals->_gBaseXpCombat->_skillXp * 0.1f * xpBonusSkillDiffForWeapon * skillBonusRace[getStatsEnumeratedFromWeaponCategory(self->currentWeaponType)] * KEP::externalGlobals->_optionsAdvanced->globalDamageMultiplier;
			increaseStat(*self->pCurrentWeaponSkill, exp, 101.0f);

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

				float exp = KEP::externalGlobals->_gBaseXpCombat->_skillXp * 2.0f * xpBonusSkillDiff * skillBonusRace[STAT_MELEE_DEFENCE] * KEP::externalGlobals->_optionsAdvanced->globalDamageMultiplier;

				increaseStat(self->_meleeDefence, exp, 101.0f);
			}

			float xpBonusSkillDiffForToughness = self->getSkillDifferenceRatio(self->ageMult * self->_toughness, damage.blunt + damage.cut + damage.pierce + damage.extraStun);

			float toughnessExp = con->XP_TOUGHNESS * KEP::externalGlobals->_gBaseXpCombat->_skillXp * xpBonusSkillDiffForToughness * skillBonusRace[STAT_TOUGHNESS] * KEP::externalGlobals->_optionsAdvanced->globalDamageMultiplier;

			increaseStat(self->_toughness, toughnessExp, 101.0f);

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

		strengthExp = strengthExp * skillBonusRace[STAT_STRENGTH] * KEP::externalGlobals->_optionsAdvanced->globalDamageMultiplier;

		increaseStat(self->_strength, strengthExp, 101.0f);
		return;
	}

	void (*CharStats_xpMelee_orig)(CharStats*, CharStats::DeadTimeState, Character*, const Damages&);
	void CharStats_xpMelee_hook(CharStats* self, CharStats::DeadTimeState what, Character* target, const Damages& damage)
	{
		if (!KEP::settings._fixMeleeCombatXP)
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

	void (*CharStats_xpLockpicking_orig)(CharStats*, int, bool);
	void CharStats_xpLockpicking_hook(CharStats* self, int lockLevel, bool success)
	{
		if (!KEP::settings._fixLockpickingXP)
		{
			CharStats_xpLockpicking_orig(self, lockLevel, success);
			return;
		}

		float baseXp = success ? 1.6f : 0.32f; // 1.2 * 4/3, 0.24 * 4/3
		float exp = static_cast<float>(self->me->myRace->statMods[STAT_LOCKPICKING] * baseXp * con->EXPERIENCE_GAIN);
		increaseStat(self->lockpicking, exp, 101.0f);
	}

	class DetourCharStats : public CharStats
	{
	public:
		Damages CharStats_getTotalAttackDamageFor_hook(Character* target);
	};

	Damages(DetourCharStats::* CharStats_getTotalAttackDamageFor_orig)(Character*);
	Damages DetourCharStats::CharStats_getTotalAttackDamageFor_hook(Character* target)
	{
		if (!KEP::settings._fixUnarmedDamageBonus || this->currentWeaponType != SKILL_UNARMED)
			return (this->*CharStats_getTotalAttackDamageFor_orig)(target);

		int bonus = 0;
		auto leftArm = this->me->medical.leftArm->robotLimb;
		if (leftArm != nullptr)
			bonus += leftArm->unarmedDamageBonus;

		auto rightArm = this->me->medical.rightArm->robotLimb;
		if (rightArm != nullptr)
			bonus += rightArm->unarmedDamageBonus;

		auto leftLeg = this->me->medical.leftLeg->robotLimb;
		if (leftLeg != nullptr)
			bonus += leftLeg->unarmedDamageBonus;

		auto rightLeg = this->me->medical.rightLeg->robotLimb;
		if (rightLeg != nullptr)
			bonus += rightLeg->unarmedDamageBonus;

		auto unarmedSkill = this->getMeleeAttack_unarmed(true);
		auto baseDamage = KEP::lerp((unarmedSkill + static_cast<float>(bonus)) * 0.01f, 0.25f, 1.0f);
		auto strengthMult = this->medical->getHealthStatModifier(STAT_STRENGTH, true, true, false, true, false, true);

		baseDamage = baseDamage * ((this->_toughness * this->ageMult + strengthMult * this->strengthBase() / 0.8f) * con->UNARMED_DAMAGE_MULT + 10.0f);
		if (baseDamage < 1.0f)
			baseDamage = 1.0f;

		auto bluntDamageRate = KEP::lerp((unarmedSkill) * 0.01f, 0.25f, 1.0f);
		float blunt = bluntDamageRate * baseDamage;
		float cutting = (1.0f - bluntDamageRate) * baseDamage;
		float pierce = 0.0f;

		float raceDamageMult = 1.0f;
		if (target != nullptr)
		{
			auto race = target->getRace();
			if (race->robot)
			{
				raceDamageMult = this->bonusRobots;
			}
			else
			{
				if (target->isAnimal())
					raceDamageMult = this->bonusAnimals;
				else if (target->isHuman())
					raceDamageMult = this->bonusHumans;
			}

			auto iter = this->bonusRaces.find(race->data);
			if (iter != this->bonusRaces.end())
				raceDamageMult *= iter->second;
		}

		float damageMult = this->skillMultDamage;
		if (damageMult < 0.0f)
			damageMult = 1.0f;

		return Damages(cutting * raceDamageMult * damageMult, blunt * raceDamageMult * damageMult, pierce * raceDamageMult * damageMult, this->bleedDamageMult, this->bonusArmourPenetration);
	}

	std::string(*CharStats_getStatName_orig)(StatsEnumerated);
	std::string CharStats_getStatName_hook(StatsEnumerated what)
	{
		switch (what)
		{
		case STAT_POLEARMS:
			return KEP::TranslationUtility::gettext_main("Polearms");
		default:
			return CharStats_getStatName_orig(what);
		}
	}

	void (*Character_recalculateTotalEquipmentSkillBonus_orig)(Character*);
	void Character_recalculateTotalEquipmentSkillBonus_hook(Character* self)
	{
		if (!KEP::settings._fixAthleticsMultiplier)
		{
			Character_recalculateTotalEquipmentSkillBonus_orig(self);
			return;
		}

		float athleticsMultiplierMin = 1.0f;
		float athleticsMultiplierMax = 1.0f;
		float combatSpeedMultiplier = 1.0f;
		int skillBonusAttack = 0;
		int skillBonusDefence = 0;
		int skillBonusUnarmed = 0;
		int skillBonusPerception = 0;
		float skillMultDodge = 1.0f;
		float skillMultStealth = 1.0f;
		float skillMultAssassin = 1.0f;
		float skillMultDexterity = 1.0f;
		float skillMultDamage = 1.0f;
		float fistInjuryEquipmentMult = 1.0f;
		float skillMultRanged = 1.0f;

		auto backPack = self->hasABackpackOn();

		self->stats->_weatherProtections.clear();

		int combatSkillBonus = 0;
		if (backPack != nullptr)
		{
			skillMultDodge = backPack->stealthMult;
			combatSkillBonus = backPack->combatSkillBonus;
			combatSpeedMultiplier = backPack->combatSpeedMult;
			if (backPack->athleticsMult < athleticsMultiplierMin)
				athleticsMultiplierMin = backPack->athleticsMult;
			if (athleticsMultiplierMax < backPack->athleticsMult)
				athleticsMultiplierMax = backPack->athleticsMult;
		}

		skillBonusAttack = combatSkillBonus;
		skillBonusDefence = combatSkillBonus;
		skillMultStealth = skillMultDodge;

		if (self->medical.armourList.size() == 0)
		{
			if (backPack == nullptr)
			{
				skillMultDodge = 1.1f;
				skillMultStealth = 1.2f;
				skillMultDexterity = 1.1f;
			}
		}
		else
		{
			for (auto iter = self->medical.armourList.begin(); iter != self->medical.armourList.end(); ++iter)
			{
				skillBonusPerception += (*iter)->perceptionBonus;
				skillBonusDefence += (*iter)->combatSkillBonusDef;
				skillBonusAttack += (*iter)->combatSkillBonusAttk;
				fistInjuryEquipmentMult *= (*iter)->fistInjuryMult;
				skillMultStealth *= (*iter)->stealthMult;
				skillMultDodge *= (*iter)->dodgeMult;
				skillBonusUnarmed += (*iter)->unarmedBonus;
				skillMultRanged *= (*iter)->rangedSkillMult;
				skillMultAssassin *= (*iter)->assassinMult;
				skillMultDexterity *= (*iter)->dexterityMult;
				skillMultDamage *= (*iter)->damageMult;
				combatSpeedMultiplier *= (*iter)->combatSpeedMult;
				if ((*iter)->athleticsMult < athleticsMultiplierMin)
					athleticsMultiplierMin = (*iter)->athleticsMult;
				if (athleticsMultiplierMax < (*iter)->athleticsMult)
					athleticsMultiplierMax = (*iter)->athleticsMult;

				for (auto weatherProtectionIter = (*iter)->weatherProtections.begin(); weatherProtectionIter != (*iter)->weatherProtections.end(); ++weatherProtectionIter)
				{
					if (*weatherProtectionIter != WA_NONE)
						self->stats->_weatherProtections[*weatherProtectionIter] += (*iter)->weatherProtectionAmount;
				}
			}
		}

		self->stats->setEquipmentStatBonuses(
			athleticsMultiplierMax * athleticsMultiplierMin,
			combatSpeedMultiplier,
			skillBonusAttack,
			skillBonusDefence,
			skillMultStealth,
			skillBonusUnarmed,
			skillMultDodge,
			fistInjuryEquipmentMult,
			skillBonusPerception,
			skillMultRanged,
			skillMultDexterity,
			skillMultAssassin,
			skillMultDamage
		);
	}

	class DetourCharacter : public Character
	{
	public:
		std::string Character_getAgeString_hook() const;
	};

	std::string(DetourCharacter::* Character_getAgeString_orig)() const;
	std::string DetourCharacter::Character_getAgeString_hook() const
	{
		if (!KEP::settings._fixAnimalAge || this->getAge0to1() < 1.1f)
			return (this->*Character_getAgeString_orig)();

		return KEP::TranslationUtility::gettext_main("Elder");
	}
}

void KEP::StatsFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharStats::xpMelee), &CharStats_xpMelee_hook, &CharStats_xpMelee_orig))
		ErrorLog("[CharStats::xpMeleee] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharStats::xpLockpicking), &CharStats_xpLockpicking_hook, &CharStats_xpLockpicking_orig))
		ErrorLog("[CharStats::xpLockpicking] could not install hook!");

	auto pCharStats_getTotalAttackDamageFor_hook = &DetourCharStats::CharStats_getTotalAttackDamageFor_hook;
	auto pCharStats_getTotalAttackDamageFor_orig = &CharStats_getTotalAttackDamageFor_orig;
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharStats::getTotalAttackDamageFor), *(void**)&pCharStats_getTotalAttackDamageFor_hook, *(void***)&pCharStats_getTotalAttackDamageFor_orig))
		ErrorLog("&CharStats::skillBonusAttack_unarmed] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharStats::getStatName), &CharStats_getStatName_hook, &CharStats_getStatName_orig))
		ErrorLog("[CharStats::getStatName] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Character::recalculateTotalEquipmentSkillBonus), &Character_recalculateTotalEquipmentSkillBonus_hook, &Character_recalculateTotalEquipmentSkillBonus_orig))
		ErrorLog("[Character::recalculateTotalEquipmentSkillBonus] could not install hook!");

	auto pCharacter_getAgeString_hook = &DetourCharacter::Character_getAgeString_hook;
	auto pCharacter_getAgeString_orig = &Character_getAgeString_orig;
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Character::_NV_getAgeString), *(void**)&pCharacter_getAgeString_hook, *(void***)&pCharacter_getAgeString_orig))
		ErrorLog("[Character::getAgeString] could not install hook!");
}
