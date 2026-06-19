#include <ogre/OgrePrerequisites.h>

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

#include <kep/functions.h>
#include <Settings.h>
#include <AnimationExtension.h>

namespace
{
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

		if (self->state == 1)
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
}

void KEP::AnimationExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&AnimationData::_CONSTRUCTOR), AnimationData__CONTRUCTOR_hook, &AnimationData__CONTRUCTOR_orig))
		ErrorLog("[AnimationData::AnimationData] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CombatTechniqueData::_CONSTRUCTOR), CombatTechniqueData__CONTRUCTOR_hook, &CombatTechniqueData__CONTRUCTOR_orig))
		ErrorLog("[CombatTechniqueData::CombatTechniqueData] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&RangedCombatClass::animationUpdate), RangedCombatClass_animationUpdate_hook, &RangedCombatClass_animationUpdate_orig))
		ErrorLog("[RangedCombatClass::animationUpdate] Could not add hook!");
}
