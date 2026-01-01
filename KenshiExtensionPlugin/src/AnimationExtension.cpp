#include <ogre/OgrePrerequisites.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/CombatTechniqueData.h>
#include <kenshi/Character.h>
#include <kenshi/CharMovement.h>

#include <extern/AnimationClass.h>
#include <extern/RangedCombatClass.h>
#include <extern/GunClass.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <AnimationExtension.h>


namespace
{
	AnimationData* (*AnimationData__CONTRUCTOR_orig)(AnimationData*, GameData*);
	CombatTechniqueData* (*CombatTechniqueData__CONTRUCTOR_orig)(CombatTechniqueData*, GameData*);
	void (*RangedCombatClass_FUN_0051DA50_orig)(RangedCombatClass*, const hand&, const Ogre::Vector3&, Character*);
}

AnimationData* KEP::AnimationExtension::AnimationData__CONTRUCTOR_hook(AnimationData* self, GameData* baseData)
{
	AnimationData__CONTRUCTOR_orig(self, baseData);

	if (baseData->bdata["turrets"])
		self->skillTypes |= 0x80;

	return self;
}

CombatTechniqueData* KEP::AnimationExtension::CombatTechniqueData__CONTRUCTOR_hook(CombatTechniqueData* self, GameData* baseData)
{
	self->skillTypes[0x16] = false;
	self->skillTypes[0x17] = false;

	CombatTechniqueData__CONTRUCTOR_orig(self, baseData);

	if (baseData->idata["animal"] < 0x9)
		self->skillTypes[0x7] = baseData->bdata["turrets"];

	return self;
}

void KEP::AnimationExtension::RangedCombatClass_FUN_0051DA50_hook(RangedCombatClass* self, const hand& handle, const Ogre::Vector3& pos, Character* target)
{
	if (!settings._aimAnimationExtension)
	{
		RangedCombatClass_FUN_0051DA50_orig(self, handle, pos, target);
		return;
	}

	if (self->turret.type != NULL_ITEM || self->gunClass == nullptr)
		return;

	self->gunClass->vfunc0x40(false);

	std::string aimAnimationName = "aimH";

	auto iterator = self->gunClass->data->objectReferences.find("aim anim");
	if (iterator != self->gunClass->data->objectReferences.end() && iterator->second.size() != 0)
	{
		auto refdata = iterator->second[0].ptr;
		if (refdata != nullptr)
		{
			auto aimAnimationData = self->me->animation->getAnimationData(refdata->name);
			if (aimAnimationData != nullptr)
				aimAnimationName = aimAnimationData->name;
		}
	}

	if (self->_0x0 == 1)
	{
		externalFunctions->FUN_005B33B0(self->me->animation, aimAnimationName);
	}
	else if (self->me->isOnScreen)
	{
		auto reloadAnimationData = self->me->animation->getAnimationData(self->gunClass->reloadAnim);
		if (reloadAnimationData == nullptr)
			reloadAnimationData = self->me->animation->getAnimationData("reload 1 phase");

		auto aimAnimationData = self->me->animation->getAnimationData(aimAnimationName);

		if (self->_0x80)
		{
			float frame = self->gunClass->animFrame;
			if (0.0f < frame && self->gunClass->bulletCount == 0)
			{
				externalFunctions->FUN_005B7240(self->me->animation, reloadAnimationData, frame, 1.0f);
				externalFunctions->FUN_005B33B0(self->me->animation, aimAnimationName);
				return;
			}
		}
		externalFunctions->FUN_005B3380(self->me->animation, reloadAnimationData);

		if (target == nullptr)
		{
			externalFunctions->FUN_005B33B0(self->me->animation, aimAnimationName);
		}
		else
		{
			if (pos != Ogre::Vector3::ZERO)
				self->me->movement->lookatPosition(pos);

			if (aimAnimationData != nullptr)
				externalFunctions->FUN_005B7030(self->me->animation, aimAnimationData, 0.0f, aimAnimationData->layer, 1.0f);

			float weight = externalFunctions->FUN_005B3020(self->me->animation, aimAnimationData);
			self->gunClass->vfunc0x40(0.9f < weight);
		}
	}
}

void KEP::AnimationExtension::init()
{
	if (settings._animationSkillTypeExtension)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_005B97F0, AnimationData__CONTRUCTOR_hook, &AnimationData__CONTRUCTOR_orig))
			ErrorLog("KenshiExtensionPlugin: [animation skiil type extension] Could not add hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CombatTechniqueData::_CONSTRUCTOR), CombatTechniqueData__CONTRUCTOR_hook, &CombatTechniqueData__CONTRUCTOR_orig))
			ErrorLog("KenshiExtensionPlugin: [animation skiil type extension] Could not add hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_0051DA50, RangedCombatClass_FUN_0051DA50_hook, &RangedCombatClass_FUN_0051DA50_orig))
			ErrorLog("KenshiExtensionPlugin: [aim animation extension] Could not add hook!");
	}
}
