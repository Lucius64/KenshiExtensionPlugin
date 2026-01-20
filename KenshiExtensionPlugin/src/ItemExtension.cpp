#include <boost/locale.hpp>

#include <ogre/OgrePrerequisites.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GlobalConstants.h>
#include <kenshi/GameData.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/CharStats.h>
#include <kenshi/Character.h>
#include <kenshi/Gear.h>
#include <kenshi/util/StringPair.h>

#include <UtilityFunction.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <ItemExtension.h>


namespace
{
	void (*Sword_setupStats_orig)(Sword*, int, GameData*, GameData*, GameData*);
	void (*CharStats_setWeapon_orig)(CharStats*, Weapon*);
}

void KEP::ItemExtension::Sword_setupStats_hook(Sword* self, int _level, GameData* baseData, GameData* companyData, GameData* materialData)
{
	self->cutDamage = lerp(self->level, 0.3f, 2.0f) * baseData->fdata["cut damage multiplier"];
	self->bluntDamage = lerp(self->level, 0.0f, 2.0f) * baseData->fdata["blunt damage multiplier"];
	self->minCutDamage = baseData->fdata["min cut damage mult"];
	self->bleedDamage = baseData->fdata["bleed mult"];

	float weight = baseData->fdata["weight kg"];

	if (companyData != nullptr)
	{
		self->cutDamage *= companyData->fdata["cut damage mod"];
		self->bluntDamage *= companyData->fdata["blunt damage mod"];
		self->minCutDamage *= companyData->fdata["min cut damage"];
		weight *= companyData->fdata["weight mod"];
	}

	self->modDefence = baseData->idata["defence mod"];
	self->modAttack = baseData->idata["attack mod"];
	self->modIndoors = baseData->idata["indoors mod"];

	float combatWeightMult = 1.0f;
	if (materialData != nullptr)
	{
		self->modDefence += materialData->idata["defence mod"];
		self->modAttack += materialData->idata["attack mod"];

		self->modIndoors += materialData->idata["indoors mod"];

		float cutMult = materialData->fdata["cut damage mod"];
		if (cutMult > 0.0f)
			self->cutDamage *= cutMult;

		float bluntMult = materialData->fdata["blunt damage mod"];
		if (bluntMult > 0.0f)
			self->bluntDamage *= bluntMult;

		float bleedMult = materialData->fdata["bleed mod"];
		if (bleedMult > 0.0f)
			self->bleedDamage *= bleedMult;

		float combatWeightMod = materialData->fdata["combat weight mod"];
		if (combatWeightMod > 0.0f)
			combatWeightMult = combatWeightMod;
	}

	float combatWeight = self->bluntDamage * 40.0f * combatWeightMult;
	self->combatWeight = combatWeight;
	self->weight = combatWeight * con->WEAPON_INVENTORY_WEIGHT_MULT;

	self->weight *= baseData->fdata["weight mult"];

	if (self->combatWeight < weight)
		self->combatWeight = weight;

	if (self->weight < weight)
		self->weight = weight;
}

void KEP::ItemExtension::CharStats_setWeapon_hook(CharStats* self, Weapon* _weapon)
{
	CharStats_setWeapon_orig(self, _weapon);

	if (_weapon != nullptr && _weapon->isSword() != nullptr && _weapon->materialData != nullptr)
	{
		float lengthMult = _weapon->materialData->fdata["length mod"];
		if (lengthMult > 0.0f)
			self->currentWeaponLength *= lengthMult;
	}
	self->attackSpeed = self->calculateAttackOrBlockSpeed(self->weaponWeightSpeedMult, self->getMeleeAttack(), false);
	self->blockSpeed = self->calculateAttackOrBlockSpeed(self->weaponWeightSpeedMult, self->getMeleeDefence(true), true);
}

void KEP::ItemExtension::init()
{
	if (settings._weaponExtension)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Sword::setupStats), &Sword_setupStats_hook, &Sword_setupStats_orig))
			ErrorLog("KenshiExtensionPlugin: [weapon pt1] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharStats::setWeapon), &CharStats_setWeapon_hook, &CharStats_setWeapon_orig))
			ErrorLog("KenshiExtensionPlugin: [weapon pt2] could not install hook!");
	}
}
