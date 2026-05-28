#include <boost/locale.hpp>

#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreStringConverter.h>

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
#include <kenshi/Faction.h>

#include <extern/RobotLimbItem.h>
#include <extern/Research.h>

#include <kep/translation.h>
#include <UtilityFunction.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <ItemExtension.h>

namespace
{
	void (*Sword_getTooltipData1_orig)(Sword*, Ogre::vector<StringPair>::type&);
	void Sword_getTooltipData1_hook(Sword* self, Ogre::vector<StringPair>::type& lines)
	{
		lines.push_back(StringPair(*KEP::externalGlobals->_TitleColorCode + self->getName(), ""));
		self->getBuyBackGUIInfo(lines);
		self->getStolenItemGUIInfo(lines);
		auto category = self->data->idata["skill category"];
		auto& mainColour = *KEP::externalGlobals->_MainColorCode;
		if (category == 0)
			lines.push_back(StringPair(mainColour + "[" + KEP::TranslationUtility::gettext_main("Katana class") + "]", ""));
		else if (category == 1)
			lines.push_back(StringPair(mainColour + "[" + KEP::TranslationUtility::gettext_main("Sabre class") + "]", ""));
		else if (category == 4)
			lines.push_back(StringPair(mainColour + "[" + KEP::TranslationUtility::gettext_main("Hacker class") + "]", ""));
		else if (category == 3)
			lines.push_back(StringPair(mainColour + "[" + KEP::TranslationUtility::gettext_main("Heavy weapons class") + "]", ""));
		else if (category == 6)
			lines.push_back(StringPair(mainColour + "[" + KEP::TranslationUtility::gettext_main("Crossbow class") + "]", ""));
		else if (category == 8)
			lines.push_back(StringPair(mainColour + "[" + KEP::TranslationUtility::gettext_main("Polearm class") + "]", ""));
		else if (category == 2)
			lines.push_back(StringPair(mainColour + "[" + KEP::TranslationUtility::gettext_main("Blunt weapon class") + "]", ""));
		else if (category == 5)
			lines.push_back(StringPair(mainColour + "[" + KEP::TranslationUtility::gettext("Martial arts class") + "]", ""));
		else if (category == 7)
			lines.push_back(StringPair(mainColour + "[" + KEP::TranslationUtility::gettext("Turret class") + "]", ""));

		lines.push_back(
			StringPair(
				mainColour + "-" + KEP::TranslationUtility::gettext_main("Cutting damage"),
				mainColour + KEP::TranslationUtility::format_main(boost::locale::format("{1,num=fix,p=2}") % self->cutDamage)
			)
		);
		lines.push_back(
			StringPair(
				mainColour + "-" + KEP::TranslationUtility::gettext_main("Blunt damage"),
				mainColour + KEP::TranslationUtility::format_main(boost::locale::format("{1,num=fix,p=2}") % self->bluntDamage)
			)
		);
		lines.push_back(
			StringPair(
				mainColour + "-" + KEP::TranslationUtility::gettext_main("Blood loss"),
				mainColour + KEP::TranslationUtility::format_main(boost::locale::format("{1,num=fix,p=2}") % self->bleedDamage) + "x"
			)
		);

		auto pierceDamage = self->data->fdata["pierce damage multiplier"] * 100.0f;
		auto armourPenetration = self->data->fdata["armour penetration"] * 100.0f;
		auto robotMult = self->data->fdata["robot damage mult"] * 100.0f;
		auto humanMult = self->data->fdata["human damage mult"] * 100.0f;
		auto animalMult = self->data->fdata["animal damage mult"] * 100.0f;

		if (self->materialData != nullptr)
		{
			auto modPierceDamage = self->materialData->fdata["pierce damage mod"] * 100.0f;
			if (modPierceDamage > 0.0f)
				pierceDamage *= modPierceDamage;

			armourPenetration += self->materialData->fdata["armour penetration mod"] * 100.0f;

			auto modRobotDamage = self->materialData->fdata["robot damage mod"] * 100.0f;
			if (modRobotDamage > 0.0f)
				robotMult *= modRobotDamage;

			auto modHumanDamage = self->materialData->fdata["human damage mod"] * 100.0f;
			if (modHumanDamage > 0.0f)
				humanMult *= modHumanDamage;

			auto modAnimalDamage = self->materialData->fdata["animal damage mod"] * 100.0f;
			if (modAnimalDamage > 0.0f)
				animalMult *= modAnimalDamage;
		}

		if (pierceDamage != 0.0f)
		{
			lines.push_back(
				StringPair(
					mainColour + "-" + KEP::TranslationUtility::gettext("Harpoon damage"),
					mainColour + KEP::TranslationUtility::format_main(boost::locale::format("{1,num=fix,p=2}") % pierceDamage)
				)
			);
		}

		if (armourPenetration != 0.0f)
			KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Armour penetration"), armourPenetration, true);

		if (self->modAttack != 0)
			KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Attack bonus"), static_cast<float>(self->modAttack), false);

		if (self->modDefence != 0)
			KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Defence bonus"), static_cast<float>(self->modDefence), false);

		auto modIndoors = self->getSkillModIndoors();
		if (modIndoors != 0)
			KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Indoors bonus"), static_cast<float>(modIndoors), false);

		KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Damage vs robots"), robotMult - 100.0f, true);
		KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Damage vs humans"), humanMult - 100.0f, true);
		KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Damage vs animals"), animalMult - 100.0f, true);

		auto& raceGroupDamage = self->data->objectReferences["race group damage"];
		for (auto iter = raceGroupDamage.begin(); iter != raceGroupDamage.end(); ++iter)
		{
			auto referenceData = iter->getPtr(&ou->gamedata);
			if (referenceData == nullptr)
				continue;

			KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Damage vs") + " " + referenceData->name + KEP::TranslationUtility::gettext_main("Race"), static_cast<float>(iter->values.value[0] - 100), true);
		}

		auto& raceDamage = self->data->objectReferences["race damage"];
		for (auto iter = raceDamage.begin(); iter != raceDamage.end(); ++iter)
		{
			auto referenceData = iter->getPtr(&ou->gamedata);
			if (referenceData == nullptr)
				continue;

			KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Damage vs") + " " + referenceData->name, static_cast<float>(iter->values.value[0] - 100), true);
		}

		auto& greyedColour = *KEP::externalGlobals->_GreyedColorCode;
		auto& secondaryColour = *KEP::externalGlobals->_SecondaryColorCode;

		lines.push_back(StringPair(greyedColour + "----------------", ""));

		auto weight = self->getItemWeight();
		lines.push_back(
			StringPair(
				secondaryColour + "-" + KEP::TranslationUtility::gettext_main("Weight"),
				secondaryColour + KEP::TranslationUtility::format_main(boost::locale::format("{1,num} kg") % static_cast<int>(weight))
			)
		);

		lines.push_back(
			StringPair(
				secondaryColour + "-" + KEP::TranslationUtility::gettext("Required Strength"),
				secondaryColour + KEP::TranslationUtility::format_main(boost::locale::format("{1,num=fix,p=2}") % self->combatWeight)
			)
		);

		float length = static_cast<float>(self->data->idata["length"]);
		if (self->materialData != nullptr)
		{
			float lengthMult = self->materialData->fdata["length mod"];
			if (lengthMult > 0.0f)
				length *= lengthMult;
		}

		lines.push_back(
			StringPair(
				secondaryColour + "-" + KEP::TranslationUtility::gettext("Length"),
				secondaryColour + KEP::TranslationUtility::format_main(boost::locale::format("{1,num=fix,p=2}") % length)
			)
		);

		self->InventoryItemBase::getTooltipTradeValue(lines);
	}

	void (*Sword_setupStats_orig)(Sword*, int, GameData*, GameData*, GameData*);
	void Sword_setupStats_hook(Sword* self, int _level, GameData* baseData, GameData* companyData, GameData* materialData)
	{
		Sword_setupStats_orig(self, _level, baseData, companyData, materialData);

		float weight = baseData->fdata["weight kg"];

		if (companyData != nullptr)
			weight *= companyData->fdata["weight mod"];

		float combatWeightMult = 1.0f;
		if (materialData != nullptr)
		{
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

	void (*CharStats_setWeapon_orig)(CharStats*, Weapon*);
	void CharStats_setWeapon_hook(CharStats* self, Weapon* _weapon)
	{
		CharStats_setWeapon_orig(self, _weapon);

		if (_weapon != nullptr)
		{
			auto baseData = _weapon->getGameData();
			auto& raceGroupDamage = baseData->objectReferences["race group damage"];
			for (auto iter = raceGroupDamage.begin(); iter != raceGroupDamage.end(); ++iter)
			{
				auto referenceData = iter->getPtr(&ou->gamedata);
				if (referenceData == nullptr)
					continue;

				auto& races = referenceData->objectReferences["races"];
				auto eRacesIter = races.end();
				for (auto racesIter = races.begin(); racesIter != eRacesIter; ++racesIter)
				{
					auto gameData = ou->gamedata.getData(racesIter->sid, RACE);
					if (gameData != nullptr)
					{
						auto bonusIter = self->bonusRaces.find(gameData);
						if (bonusIter == self->bonusRaces.end())
							self->bonusRaces[gameData] = iter->values.value[0] * 0.01f;
					}
				}
			}

			if (_weapon->isSword() != nullptr && _weapon->materialData != nullptr)
			{
				float pierceMult = _weapon->materialData->fdata["pierce damage mod"];
				if (pierceMult > 0.0f)
					self->pierceDamageMult *= pierceMult;

				float robotMult = _weapon->materialData->fdata["robot damage mod"];
				if (robotMult > 0.0f)
					self->bonusRobots *= robotMult;

				float humanMult = _weapon->materialData->fdata["human damage mod"];
				if (humanMult > 0.0f)
					self->bonusHumans *= humanMult;

				float animalMult = _weapon->materialData->fdata["animal damage mod"];
				if (animalMult > 0.0f)
					self->bonusAnimals *= animalMult;

				self->bonusArmourPenetration += _weapon->materialData->fdata["armour penetration mod"];

				auto& raceMult = _weapon->materialData->objectReferences["race damage mod"];
				for (auto iter = raceMult.begin(); iter != raceMult.end(); ++iter)
				{
					auto data = iter->getPtr(&ou->gamedata);
					if (data != nullptr)
					{
						auto bonusIter = self->bonusRaces.find(data);
						if (bonusIter != self->bonusRaces.end())
							self->bonusRaces[data] = iter->values.value[0] * 0.01f;
						else
							bonusIter->second *= iter->values.value[0] * 0.01f;
					}
				}

				float lengthMult = _weapon->materialData->fdata["length mod"];
				if (lengthMult > 0.0f)
					self->currentWeaponLength *= lengthMult;
			}
		}
		
		self->attackSpeed = self->calculateAttackOrBlockSpeed(self->weaponWeightSpeedMult, self->getMeleeAttack(), false);
		self->blockSpeed = self->calculateAttackOrBlockSpeed(self->weaponWeightSpeedMult, self->getMeleeDefence(true), true);
	}

	void (*Crossbow_getTooltipData2_orig)(Crossbow*, Ogre::vector<StringPair>::type&);
	void Crossbow_getTooltipData2_hook(Crossbow* self, Ogre::vector<StringPair>::type& lines)
	{
		Crossbow_getTooltipData2_orig(self, lines);
		if (!self->crafter.empty())
		{
			auto& mainColour = *KEP::externalGlobals->_MainColorCode;
			lines.push_back(StringPair("", ""));

			lines.push_back(
				StringPair(
					mainColour + "[" + KEP::TranslationUtility::format_main(boost::locale::format(boost::locale::translate("Crafted by {1}")) % self->crafter) + "]"
				)
			);

			auto& secondaryColour = *KEP::externalGlobals->_SecondaryColorCode;
			auto id = self->level_0_100 < 70 ? "Crafted by the smith {1} of {2}." : "A masterful weapon crafted by the renowned smith {1} of {2}.";
			lines.push_back(
				StringPair(
					secondaryColour + KEP::TranslationUtility::format_main(boost::locale::format(boost::locale::translate(id)) % self->crafter % ou->player->participant->name)
				)
			);
		}
	}

	void (*RobotLimbItem_getTooltipData2_orig)(RobotLimbItem*, Ogre::vector<StringPair>::type&);
	void RobotLimbItem_getTooltipData2_hook(RobotLimbItem* self, Ogre::vector<StringPair>::type& lines)
	{
		RobotLimbItem_getTooltipData2_orig(self, lines);
		if (!self->crafter.empty())
		{
			auto& mainColour = *KEP::externalGlobals->_MainColorCode;
			lines.push_back(StringPair("", ""));

			lines.push_back(
				StringPair(
					mainColour + "[" + KEP::TranslationUtility::format_main(boost::locale::format(boost::locale::translate("Crafted by {1}")) % self->crafter) + "]"
				)
			);

			auto& secondaryColour = *KEP::externalGlobals->_SecondaryColorCode;
			auto id = self->level_0_100 < 70 ? "Crafted by the smith {1} of {2}." : "High quality armour crafted by the renowned smith {1} of {2}.";
			lines.push_back(
				StringPair(
					secondaryColour + KEP::TranslationUtility::format_main(boost::locale::format(boost::locale::translate(id)) % self->crafter % ou->player->participant->name)
				)
			);
		}
	}
}

void KEP::ItemExtension::init()
{
	if (settings._weaponExtension)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Sword::setupStats), &Sword_setupStats_hook, &Sword_setupStats_orig))
			ErrorLog("[Sword::setupStats] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharStats::setWeapon), &CharStats_setWeapon_hook, &CharStats_setWeapon_orig))
			ErrorLog("[CharStats::setWeapon] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Sword::_NV_getTooltipData1), &Sword_getTooltipData1_hook, &Sword_getTooltipData1_orig))
			ErrorLog("[Sword::getTooltipData1] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Crossbow::_NV_getTooltipData2), &Crossbow_getTooltipData2_hook, &Crossbow_getTooltipData2_orig))
			ErrorLog("[Crossbow::getTooltipData2] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KEP::externalFunctions->RobotLimbItem_getTooltipData2, &RobotLimbItem_getTooltipData2_hook, &RobotLimbItem_getTooltipData2_orig))
			ErrorLog("[RobotLimbItem::getTooltipData2] could not install hook!");
	}
}
