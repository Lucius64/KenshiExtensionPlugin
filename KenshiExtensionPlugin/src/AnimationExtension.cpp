/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreAnimationState.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/combat/CombatTechniqueData.h>
#include <kenshi/Character.h>
#include <kenshi/CharMovement.h>
#include <kenshi/Animation/AnimationClass.h>
#include <kenshi/GunClass.h>
#include <kenshi/combat/RangedCombatClass.h>
#include <kenshi/CharStats.h>
#include <kenshi/Gear.h>
#include <kenshi/Faction.h>
#include <kenshi/RaceData.h>
#include <kenshi/combat/CombatClass.h>

#include <kep/functions.h>
#include <Settings.h>
#include <UtilityFunction.h>
#include <AnimationExtension.h>

namespace
{
	ogre_unordered_map<CombatTechniqueData*, Ogre::set<GameData*>::type>::type specialAttacks;
	ogre_unordered_map<CombatTechniqueData*, Ogre::set<GameData*>::type>::type specialBlocks;
	ogre_unordered_map<GameData*, lektor<CombatTechniqueData*>>::type specialWeaponAttacks;
	ogre_unordered_map<GameData*, lektor<CombatTechniqueData*>>::type specialWeaponBlocks;
	bool once = false;

	void(*CharStats_setupCombatTechniques_orig)();
	void CharStats_setupCombatTechniques_hook()
	{
		if (!KEP::settings._combatTechniquesEx)
		{
			CharStats_setupCombatTechniques_orig();
			return;
		}

		if (once)
			return;
		once = true;
		auto& attacks = KEP::functions->getAttacks();
		auto& blocks = KEP::functions->getBlocks();

		lektor<GameData*> list;
		ou->gamedata.getDataOfType(list, COMBAT_TECHNIQUE);

		for (auto iter = list.begin(); iter != list.end(); ++iter)
		{
			auto dat = *iter;
			if (dat->bdata["disabled"])
				continue;

			if (dat->bdata["is block"] || dat->bdata["is dodge"])
			{
				if (dat->bdata["is special"])
				{
					lektor<GameData*> specialActionUsers;
					ou->gamedata.findAllDataThatReferencesThis(specialActionUsers, dat, CHARACTER, "combat techniques");
					ou->gamedata.findAllDataThatReferencesThis(specialActionUsers, dat, ANIMAL_CHARACTER, "combat techniques");
					ou->gamedata.findAllDataThatReferencesThis(specialActionUsers, dat, FACTION, "combat techniques");
					ou->gamedata.findAllDataThatReferencesThis(specialActionUsers, dat, RACE, "combat techniques");
					if (specialActionUsers.size() != 0)
					{
						auto combatTechnique = new CombatTechniqueData(dat);
						specialBlocks[combatTechnique].clear();
						for (auto userIter = specialActionUsers.begin(); userIter != specialActionUsers.end(); ++userIter)
						{
							specialBlocks[combatTechnique].insert(*userIter);
						}
					}

					lektor<GameData*> specialActionWeapons;
					ou->gamedata.findAllDataThatReferencesThis(specialActionWeapons, dat, WEAPON, "combat techniques");
					if (specialActionWeapons.size() != 0)
					{
						auto combatTechnique = new CombatTechniqueData(dat);
						for (auto weaponIter = specialActionWeapons.begin(); weaponIter = specialActionWeapons.end(); ++weaponIter)
						{
							specialWeaponBlocks[*weaponIter].push_back(combatTechnique);
						}
					}
				}
				else
					blocks.push_back(new CombatTechniqueData(dat));
			}
			else
			{
				if (dat->bdata["is special"])
				{
					lektor<GameData*> specialActionUsers;
					ou->gamedata.findAllDataThatReferencesThis(specialActionUsers, dat, CHARACTER, "combat techniques");
					ou->gamedata.findAllDataThatReferencesThis(specialActionUsers, dat, ANIMAL_CHARACTER, "combat techniques");
					ou->gamedata.findAllDataThatReferencesThis(specialActionUsers, dat, FACTION, "combat techniques");
					ou->gamedata.findAllDataThatReferencesThis(specialActionUsers, dat, RACE, "combat techniques");
					if (specialActionUsers.size() != 0)
					{
						auto combatTechnique = new CombatTechniqueData(dat);
						specialAttacks[combatTechnique].clear();
						for (auto userIter = specialActionUsers.begin(); userIter != specialActionUsers.end(); ++userIter)
						{
							specialAttacks[combatTechnique].insert(*userIter);
						}
					}

					lektor<GameData*> specialActionWeapons;
					ou->gamedata.findAllDataThatReferencesThis(specialActionWeapons, dat, WEAPON, "combat techniques");
					if (specialActionWeapons.size() != 0)
					{
						auto combatTechnique = new CombatTechniqueData(dat);
						for (auto weaponIter = specialActionWeapons.begin(); weaponIter != specialActionWeapons.end(); ++weaponIter)
						{
							specialWeaponAttacks[*weaponIter].push_back(combatTechnique);
						}
					}
				}
				else
					attacks.push_back(new CombatTechniqueData(dat));
			}
		}
	}

	bool _rearCutDirection(CutDirection dir)
	{
		return dir == CUT_REAR_DOWNWARD || dir == CUT_REAR_LEFT || dir == CUT_REAR_RIGHT;
	}

	void _addBlock(CombatTechniqueData* combatTechnique, CharStats* stats, FitnessSelector<CombatTechniqueData*>& possibleBlocks, CutDirection dir, bool failed, bool isDodge)
	{
		if ((stats->me->getProneState() == PS_NORMAL) != combatTechnique->isProne)
		{
			if (!combatTechnique->isDodge)
			{
				if (!isDodge)
				{
					if (combatTechnique->impactPoints.size() == 0)
						return;

					auto blockDirection = combatTechnique->impactPoints[0].direction;
					if (!failed)
					{
						if (blockDirection == dir)
							possibleBlocks.addItem(combatTechnique, combatTechnique->chanceMult);
					}
					else
					{
						if (blockDirection == dir)
							return;

						if (_rearCutDirection(blockDirection) == _rearCutDirection(dir))
							possibleBlocks.addItem(combatTechnique, combatTechnique->chanceMult);
					}
				}
			}
			else if (isDodge && combatTechnique->stumbleDodge == stats->me->stumbleState() && 0.0f < combatTechnique->chanceMult)
			{
				possibleBlocks.addItem(combatTechnique, combatTechnique->chanceMult);
			}
		}
	}

	CombatTechniqueData* (*CharStats_chooseBlock_orig)(CharStats*, CutDirection, float, CutOrigination, Character*);
	CombatTechniqueData* CharStats_chooseBlock_hook(CharStats* self, CutDirection dir, float opponentAttackSkill, CutOrigination from, Character* opponent)
	{
		if (!KEP::settings._combatTechniquesEx && !KEP::settings._fixUnarmedBlockChance)
			return CharStats_chooseBlock_orig(self, dir, opponentAttackSkill, from, opponent);

		auto direction = self->me->convertCutDirection(dir, from);
		float blockChance = 1.0f;
		bool blockFailed = false;

		if (opponent->isAnimal() == nullptr && opponent->stats->currentWeaponType == SKILL_UNARMED)
		{
			if (KEP::settings._fixUnarmedBlockChance)
				blockChance = KEP::clamp(KEP::lerp(self->unarmed * 0.01f, 0.5f, 1.1f), 0.5f, 1.1f);
			else
				blockChance = KEP::clamp(KEP::linear(self->unarmed, 0.75f, 1.1f), 0.5f, 1.1f);
		}

		blockChance *= self->_calculateBlockChance(opponentAttackSkill);
		if (blockChance < UtilityT::random() * 100.0f)
			blockFailed = true;

		bool isDodge = self->me->stumbleState();

		auto weaponType = self->medical->rightArmOk ? self->currentWeaponType : SKILL_UNARMED;
		if (weaponType == SKILL_UNARMED)
		{
			if (UtilityT::random() * 100.0f < self->calculateDodgeChance(opponentAttackSkill, false))
				isDodge = true;
		}

		FitnessSelector<CombatTechniqueData*> possibleBlocks;
		if (KEP::settings._combatTechniquesEx)
		{
			auto weapon = self->me->getCurrentWeapon();
			if (weapon != nullptr && specialWeaponBlocks.count(weapon->data))
			{
				auto& blocks = specialWeaponBlocks[weapon->data];
				for (auto iter = blocks.begin(); iter != blocks.end(); ++iter)
				{
					_addBlock(*iter, self, possibleBlocks, direction, blockFailed, isDodge);
				}

				if (possibleBlocks.size() != 0)
					return possibleBlocks.chooseAnItem();
			}

			for (auto iter = specialBlocks.begin(); iter != specialBlocks.end(); ++iter)
			{
				if (iter->second.count(self->me->getGameData()) || iter->second.count(self->me->getFaction()->data) || iter->second.count(self->me->getRace()->data))
					if (iter->first->skillTypes[weaponType])
						_addBlock(iter->first, self, possibleBlocks, direction, blockFailed, isDodge);
			}
		}
		
		auto& blocks = KEP::functions->getBlocks();
		for (auto iter = blocks.begin(); iter != blocks.end(); ++iter)
		{
			if ((*iter)->skillTypes[weaponType])
				_addBlock(*iter, self, possibleBlocks, direction, blockFailed, isDodge);
		}

		return possibleBlocks.chooseAnItem();
	}

	void _addAttack(CombatTechniqueData* combatTechnique, CharStats* stats, FitnessSelector<CombatTechniqueData*>& possibleAttacks, float range, float weaponReach, CombatTechniqueData* lastAttack, bool opponentIsStationary, bool skipMedicals, WeaponCategory _weaponType, float _weaponSkill, float encumbranceMult)
	{
		if (_weaponSkill <= combatTechnique->maxSkill && combatTechnique->minSkill <= _weaponSkill
			&& encumbranceMult <= combatTechnique->maxEncumbrance
			&& (stats->me->getProneState() == PS_NORMAL) != combatTechnique->isProne)
		{
			float medicalScore = stats->medical->scoreTechnique(combatTechnique);
			if (0.0f < medicalScore && (skipMedicals || 1.0f <= medicalScore))
			{
				float reach = stats->me->getAge() * (weaponReach * 0.5f + combatTechnique->initialDistance) + 3.0f;
				float distanceScore = range + 3.0f;
				if (reach <= distanceScore)
					distanceScore = (1.0f / distanceScore) * reach;
				else
					distanceScore = (1.0f / reach) * distanceScore;

				float longestReach = combatTechnique->initialDistance + weaponReach;
				if (stats->longestReachingAttack < longestReach)
					stats->longestReachingAttack = longestReach;

				if (range <= weaponReach + combatTechnique->initialDistance)
				{
					float score = distanceScore * medicalScore;
					if (range < combatTechnique->minDistanceVsStatic)
						score *= 0.02f;

					if (combatTechnique == lastAttack && !opponentIsStationary)
						score *= 0.2f;

					possibleAttacks.addItem(combatTechnique, score * combatTechnique->chanceMult);
				}
			}
		}
	}

	void (*CharStats__chooseAttacks_orig)(CharStats*, FitnessSelector<CombatTechniqueData*>&, float, float, CombatTechniqueData*, bool, bool, WeaponCategory, float);
	void CharStats__chooseAttacks_hook(CharStats* self, FitnessSelector<CombatTechniqueData*>& possibleAttacks, float range, float weaponReach, CombatTechniqueData* lastAttack, bool opponentIsStationary, bool skipMedicals, WeaponCategory _weaponType, float _weaponSkill)
	{
		if (KEP::settings._combatTechniquesEx)
		{
			float encumbranceMult = (1.0f - self->encumbranceMult) * 100.0f;

			auto weapon = self->me->getCurrentWeapon();
			if (weapon != nullptr && specialWeaponAttacks.count(weapon->data))
			{
				auto& attacks = specialWeaponAttacks[weapon->data];
				for (auto iter = attacks.begin(); iter != attacks.end(); ++iter)
				{
					_addAttack(*iter, self, possibleAttacks, range, weaponReach, lastAttack, opponentIsStationary, skipMedicals, _weaponType, _weaponSkill, encumbranceMult);
				}

				if (possibleAttacks.size() != 0)
					return;
			}

			for (auto iter = specialAttacks.begin(); iter != specialAttacks.end(); ++iter)
			{
				if (iter->second.count(self->me->getGameData()) || iter->second.count(self->me->getFaction()->data) || iter->second.count(self->me->getRace()->data))
					if (iter->first->skillTypes[_weaponType])
						_addAttack(iter->first, self, possibleAttacks, range, weaponReach, lastAttack, opponentIsStationary, skipMedicals, _weaponType, _weaponSkill, encumbranceMult);
			}
		}
		CharStats__chooseAttacks_orig(self, possibleAttacks, range, weaponReach, lastAttack, opponentIsStationary, skipMedicals, _weaponType, _weaponSkill);
	}

	AnimationData* (*AnimationData__CONTRUCTOR_orig)(AnimationData*, GameData*);
	AnimationData* AnimationData__CONTRUCTOR_hook(AnimationData* self, GameData* dat)
	{
		AnimationData__CONTRUCTOR_orig(self, dat);

		if (KEP::settings._animationEx && dat->bdata["turrets"])
			self->weaponTypeFlags |= 0x80;

		return self;
	}

	CombatTechniqueData* (*CombatTechniqueData__CONTRUCTOR_orig)(CombatTechniqueData*, GameData*);
	CombatTechniqueData* CombatTechniqueData__CONTRUCTOR_hook(CombatTechniqueData* self, GameData* data)
	{
		if (!KEP::settings._animationEx)
		{
			CombatTechniqueData__CONTRUCTOR_orig(self, data);
			return self;
		}

		self->skillTypes[0x16] = false;
		self->skillTypes[0x17] = false;

		CombatTechniqueData__CONTRUCTOR_orig(self, data);

		if (data->idata["animal"] < 0x9)
			self->skillTypes[0x7] = data->bdata["turrets"];

		return self;
	}

	void (*RangedCombatClass_animationUpdate_orig)(RangedCombatClass*, float, const Ogre::Vector3&, RootObject*);
	void RangedCombatClass_animationUpdate_hook(RangedCombatClass* self, float frameTime, const Ogre::Vector3& aimpos, RootObject* target)
	{
		if (!KEP::settings._animationEx)
		{
			RangedCombatClass_animationUpdate_orig(self, frameTime, aimpos, target);
			return;
		}

		if (self->turret.type != NULL_ITEM || self->gun == nullptr)
			return;

		self->gun->setAnimationReadyToShoot(false);

		std::string aimAnimationName = "aimH";

		auto iterator = self->gun->gunData->objectReferences.find("aim anim");
		if (iterator != self->gun->gunData->objectReferences.end() && iterator->second.size() != 0)
		{
			auto refdata = iterator->second[0].ptr;
			if (refdata != nullptr)
			{
				auto aimAnimationData = self->me->animation->getAnimationData(refdata->name);
				if (aimAnimationData != nullptr)
					aimAnimationName = aimAnimationData->dataName;
			}
		}

		if (self->state == RangedCombatClass::MOVING)
		{
			self->me->animation->stopAnimation(aimAnimationName);
		}
		else if (self->me->isOnScreen)
		{
			auto reloadAnimationData = self->me->animation->getAnimationData(self->gun->reloadAnimation);
			if (reloadAnimationData == nullptr)
				reloadAnimationData = self->me->animation->getAnimationData("reload 1 phase");

			auto aimAnimationData = self->me->animation->getAnimationData(aimAnimationName);

			if (self->_isReloading)
			{
				float frame = self->gun->reloadState;
				if (0.0f < frame && self->gun->numShotsCurrent == 0)
				{
					self->me->animation->runAnimation_manualTiming(reloadAnimationData, frame, 1.0f);
					self->me->animation->stopAnimation(aimAnimationName);
					return;
				}
			}
			self->me->animation->stopAnimation(reloadAnimationData);

			if (target == nullptr)
			{
				self->me->animation->stopAnimation(aimAnimationName);
			}
			else
			{
				if (aimpos != Ogre::Vector3::ZERO)
					self->me->movement->lookatPosition(aimpos);

				if (aimAnimationData != nullptr)
					self->me->animation->runAnimation(aimAnimationData, 0.0f, aimAnimationData->layername, 1.0f);

				float weight = self->me->animation->getAnimationCurrentWeight(aimAnimationData);
				self->gun->setAnimationReadyToShoot(0.9f < weight);
			}
		}
	}

	CombatTechniqueData* _chooseDogde(CharStats* self, CutDirection dir, float opponentAttackSkill, CutOrigination from, Character* opponent)
	{
		auto direction = self->me->convertCutDirection(dir, from);
		float dodgeChance = self->calculateDodgeChance(opponentAttackSkill, false);

		if (KEP::settings._fixUnarmedBlockChance && opponent != nullptr && opponent->isAnimal() == nullptr && self->currentWeaponType != SKILL_UNARMED && opponent->stats->currentWeaponType == SKILL_UNARMED)
		{
			dodgeChance *= KEP::clamp(KEP::lerp(self->unarmed * 0.01f, 0.5f, 1.1f), 0.5f, 1.1f);
			if (95.0f < dodgeChance)
				dodgeChance = 95.0f;
		}

		if (dodgeChance <= UtilityT::random() * 100.0f)
			return nullptr;

		auto weaponType = self->medical->rightArmOk ? self->currentWeaponType : SKILL_UNARMED;

		FitnessSelector<CombatTechniqueData*> possibleBlocks;
		if (KEP::settings._combatTechniquesEx)
		{
			auto weapon = self->me->getCurrentWeapon();
			if (weapon != nullptr && specialWeaponBlocks.count(weapon->data))
			{
				auto& blocks = specialWeaponBlocks[weapon->data];
				for (auto iter = blocks.begin(); iter != blocks.end(); ++iter)
				{
					_addBlock(*iter, self, possibleBlocks, CUT_DOWNWARD, true, true);
				}

				if (possibleBlocks.size() != 0)
					return possibleBlocks.chooseAnItem();
			}

			for (auto iter = specialBlocks.begin(); iter != specialBlocks.end(); ++iter)
			{
				if (iter->second.count(self->me->getGameData()) || iter->second.count(self->me->getFaction()->data) || iter->second.count(self->me->getRace()->data))
					if (iter->first->skillTypes[weaponType])
						_addBlock(iter->first, self, possibleBlocks, CUT_DOWNWARD, true, true);
			}
		}
		
		auto& blocks = KEP::functions->getBlocks();
		for (auto iter = blocks.begin(); iter != blocks.end(); ++iter)
		{
			if ((*iter)->skillTypes[weaponType])
				_addBlock(*iter, self, possibleBlocks, CUT_DOWNWARD, true, true);
		}

		return possibleBlocks.chooseAnItem();
	}

	HitMaterialType (*CombatClass__iHitYouAreYouHit_orig)(CombatClass*, CutDirection, Damages&, Character*);
	HitMaterialType CombatClass__iHitYouAreYouHit_hook(CombatClass* self, CutDirection dir, Damages& damage, Character* who)
	{
		if (!KEP::settings._combatEx)
			return CombatClass__iHitYouAreYouHit_orig(self, dir, damage, who);

		if (who != nullptr && who->getDataType() == CHARACTER)
			self->me->rememberCharacter(who, ST_TEMPORARY_ENEMY);

		if (!self->canBlock())
			return HIT_FLESH;

		bool canDodge = false;
		bool blockFailed = false;
		bool caughtInTheAttack = who != nullptr && who->getAttackTarget().getCharacter() != self->me;
		auto currentTechnique = self->currentTechnique;
		if (currentTechnique != nullptr)
		{
			canDodge = self->combatState != STUMBLE && self->combatState != CHOP_WEAPON && self->combatModeActive;
			if (currentTechnique->isDodge)
			{
				float progress = self->animation->getAnimationProgress(currentTechnique);
				if (0.1f <= progress && progress <= 0.98f)
					return HIT_MISSED;
			}
			else
			{
				if (currentTechnique->isBlock)
				{
					if (currentTechnique->impactPoints.size() != 0)
					{
						if (currentTechnique->impactPoints[0].direction == self->me->convertCutDirection(dir, who) && 0.5f < self->animation->getAnimationProgress(currentTechnique))
							return HIT_SWORD;
					}

					blockFailed = true;
				}
				else
				{
					canDodge = !self->animation->stillPlayingAnActionOrSomething();
				}
			}
		}
		else
		{
			canDodge = self->combatState != STUMBLE && self->combatModeActive;
		}

		if (who != nullptr && who->getDataType() == CHARACTER)
		{
			if (canDodge && caughtInTheAttack || blockFailed)
			{
				auto opponentAttackSkill = who->stats->getMeleeAttack();
				auto technique = _chooseDogde(self->stats, dir, opponentAttackSkill, self->me->getAttackOriginationDirection(who), who);
				if (technique != nullptr)
				{
					if (technique->isDodge)
					{
						self->currentTechnique = technique;
						self->animation->layer[1]->stopActionAnimations();
						self->combatState = BLOCK;
						self->nextMove = BLOCK;
						self->stats->xpDodgeEvent(opponentAttackSkill, true);
						self->animation->startCombatAnimation(technique, self->stats->blockSpeed * technique->animSpeedMultiplier, "");
						self->stateTimer = 0.2f;
						self->techniqueIntegrityCheckTimer = self->stats->calculateTechniqueInegrityCheckTimer();
						self->blockingTarget = who;
						self->blockingTargetH = who->getHandle();

						auto singleAnim = self->animation->getAnimationPlaying_animName(technique->animation);
						if (singleAnim != nullptr)
						{
							singleAnim->currentFrameTime01 = 0.1f;
							if (singleAnim->mainState != nullptr)
								singleAnim->currentFrameTime = 0.1f * singleAnim->mainState->getLength();
						}

						return HIT_MISSED;
					}
				}
				else if (self->stats->currentWeaponType != SKILL_UNARMED)
				{
					self->stats->xpDodgeEvent(opponentAttackSkill, false);
				}
			}

			if (self->me->isLawEnforcement())
			{
				auto faction = self->me->getFaction();
				who->crimes.notifyCrimeWitnessed(faction, self->me->handle, 30, CRIME_ASSAULT);
				who->crimes.assignBountyForCrimes(faction);
			}
			else
			{
				if (!who->isLawEnforcement())
					return HIT_FLESH;
				auto faction = who->getFaction();
				self->me->crimes.notifyCrimeWitnessed(faction, who->handle, 30, CRIME_ASSAULT);
				self->me->crimes.assignBountyForCrimes(faction);
			}
		}
		return HIT_FLESH;
	}

	bool (*Character_iShotYou_orig)(Character*, Character*, Harpoon*, bool);
	bool Character_iShotYou_hook(Character* self, Character* attacker, Harpoon* poon, bool onPurpose)
	{
		if (KEP::settings._combatEx)
		{
			auto combat = self->getCombatClass();
			if (combat->currentTechnique != nullptr && combat->currentTechnique->isDodge)
			{
				float progress = self->animation->getAnimationProgress(combat->currentTechnique);
				if (0.1f <= progress && progress <= 0.98f)
				{
					if (onPurpose)
						self->rememberCharacter(attacker, ST_TEMPORARY_ENEMY);
					return false;
				}
			}

			if (attacker != nullptr && combat->canBlock() && (combat->combatState != CHOP_WEAPON ||  !combat->animation->stillPlayingAnActionOrSomething()) && combat->combatModeActive)
			{
				auto opponentAttackSkill = attacker->stats->getStat(attacker->rangedCombat->currentStat, false);
				auto technique = _chooseDogde(self->stats, CUT_PIERCED, opponentAttackSkill, self->getAttackOriginationDirection(attacker), nullptr);
				if (technique != nullptr)
				{
					combat->currentTechnique = technique;
					if (technique->isDodge)
					{
						if (onPurpose)
							self->rememberCharacter(attacker, ST_TEMPORARY_ENEMY);

						self->animation->layer[1]->stopActionAnimations();
						combat->combatState = BLOCK;
						combat->nextMove = BLOCK;
						self->stats->xpDodgeEvent(opponentAttackSkill, true);
						self->animation->startCombatAnimation(technique, self->stats->blockSpeed * technique->animSpeedMultiplier, "");
						combat->stateTimer = 0.2f;
						combat->techniqueIntegrityCheckTimer = self->stats->calculateTechniqueInegrityCheckTimer();
						combat->blockingTarget = attacker;
						combat->blockingTargetH = attacker->getHandle();

						auto singleAnim = self->animation->getAnimationPlaying_animName(technique->animation);
						if (singleAnim != nullptr)
						{
							singleAnim->currentFrameTime01 = 0.1f;
							if (singleAnim->mainState != nullptr)
								singleAnim->currentFrameTime = 0.1f * singleAnim->mainState->getLength();
						}

						return false;
					}
				}
				else if (self->stats->currentWeaponType != SKILL_UNARMED)
				{
					self->stats->xpDodgeEvent(opponentAttackSkill, false);
				}
			}
		}

		return Character_iShotYou_orig(self, attacker, poon, onPurpose);
	}
}

void KEP::AnimationExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&AnimationData::_CONSTRUCTOR), AnimationData__CONTRUCTOR_hook, &AnimationData__CONTRUCTOR_orig))
		ErrorLog("[AnimationData::AnimationData] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&CombatTechniqueData::_CONSTRUCTOR), CombatTechniqueData__CONTRUCTOR_hook, &CombatTechniqueData__CONTRUCTOR_orig))
		ErrorLog("[CombatTechniqueData::CombatTechniqueData] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&RangedCombatClass::animationUpdate), RangedCombatClass_animationUpdate_hook, &RangedCombatClass_animationUpdate_orig))
		ErrorLog("[RangedCombatClass::animationUpdate] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&CharStats::setupCombatTechniques), CharStats_setupCombatTechniques_hook, &CharStats_setupCombatTechniques_orig))
		ErrorLog("[CharStats::setupCombatTechniques] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&CharStats::_chooseAttacks), CharStats__chooseAttacks_hook, &CharStats__chooseAttacks_orig))
		ErrorLog("[CharStats::_chooseAttacks] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&CharStats::chooseBlock), CharStats_chooseBlock_hook, &CharStats_chooseBlock_orig))
		ErrorLog("[CharStats::chooseBlock] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&CombatClass::_iHitYouAreYouHit), CombatClass__iHitYouAreYouHit_hook, &CombatClass__iHitYouAreYouHit_orig))
		ErrorLog("[CombatClass::_iHitYouAreYouHit] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Character::iShotYou), Character_iShotYou_hook, &Character_iShotYou_orig))
		ErrorLog("[Character::iShotYou] Could not add hook!");
}
