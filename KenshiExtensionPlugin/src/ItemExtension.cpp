#include <boost/locale.hpp>

#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreStringConverter.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/KingOfRenderThread.h>
#include <kenshi/GlobalConstants.h>
#include <kenshi/GameData.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/CharStats.h>
#include <kenshi/Character.h>
#include <kenshi/Gear.h>
#include <kenshi/util/StringPair.h>
#include <kenshi/Faction.h>

#include <extern/Research.h>

#include <kep/utility.h>
#include <kep/functions.h>
#include <kep/translation.h>
#include <UtilityFunction.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <ItemExtension.h>

namespace
{
	const auto msg_craftedBy = "Crafted by {1}";
	const auto msg_kg = "{1,num} kg";

	void (*Sword_getTooltipData1_orig)(Sword*, Ogre::vector<StringPair>::type&);
	void Sword_getTooltipData1_hook(Sword* self, Ogre::vector<StringPair>::type& lines)
	{
		if (!KEP::settings._weaponExtension)
		{
			Sword_getTooltipData1_orig(self, lines);
			return;
		}

		lines.push_back(StringPair(KEP::GUIColor::getTitle() + self->getName(), ""));
		self->getBuyBackGUIInfo(lines);
		self->getStolenItemGUIInfo(lines);
		auto category = self->data->idata["skill category"];
		auto& mainColour = KEP::GUIColor::getMain();
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

		auto pierceDamage = self->data->fdata["pierce damage multiplier"];
		auto armourPenetration = self->data->fdata["armour penetration"] * 100.0f;
		auto robotMult = self->data->fdata["robot damage mult"];
		auto humanMult = self->data->fdata["human damage mult"];
		auto animalMult = self->data->fdata["animal damage mult"];

		ogre_unordered_map<GameData*, float>::type raceDamageMults;
		if (self->materialData != nullptr)
		{
			auto modPierceDamage = self->materialData->fdata["pierce damage mod"];
			if (modPierceDamage > 0.0f)
				pierceDamage *= modPierceDamage;

			armourPenetration += self->materialData->fdata["armour penetration mod"] * 100.0f;

			auto modRobotDamage = self->materialData->fdata["robot damage mod"];
			if (modRobotDamage > 0.0f)
				robotMult *= modRobotDamage;

			auto modHumanDamage = self->materialData->fdata["human damage mod"];
			if (modHumanDamage > 0.0f)
				humanMult *= modHumanDamage;

			auto modAnimalDamage = self->materialData->fdata["animal damage mod"];
			if (modAnimalDamage > 0.0f)
				animalMult *= modAnimalDamage;

			auto& raceDamage = self->materialData->objectReferences["race damage mod"];
			for (auto iter = raceDamage.begin(); iter != raceDamage.end(); ++iter)
			{
				auto referenceData = iter->getPtr(&ou->gamedata);
				if (referenceData != nullptr)
					raceDamageMults[referenceData] = iter->values.value[0] * 0.01f;
			}
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

		KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Damage vs robots"), robotMult * 100.0f - 100.0f, true);
		KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Damage vs humans"), humanMult * 100.0f - 100.0f, true);
		KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Damage vs animals"), animalMult * 100.0f - 100.0f, true);

		auto& raceGroupDamage = self->data->objectReferences["race group damage"];
		for (auto iter = raceGroupDamage.begin(); iter != raceGroupDamage.end(); ++iter)
		{
			auto referenceData = iter->getPtr(&ou->gamedata);
			if (referenceData == nullptr)
				continue;

			KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::format(boost::locale::format(boost::locale::translate("Damage vs {1} race")) % referenceData->name), static_cast<float>(iter->values.value[0] - 100), true);
		}

		auto& raceDamage = self->data->objectReferences["race damage"];
		for (auto iter = raceDamage.begin(); iter != raceDamage.end(); ++iter)
		{
			auto referenceData = iter->getPtr(&ou->gamedata);
			if (referenceData != nullptr)
			{
				auto bonusIter = raceDamageMults.find(referenceData);
				if (bonusIter == raceDamageMults.end())
					raceDamageMults[referenceData] = iter->values.value[0] * 0.01f;
				else
					bonusIter->second *= iter->values.value[0] * 0.01f;
			}
		}

		for (auto iter = raceDamageMults.begin(); iter != raceDamageMults.end(); ++iter)
		{
			KEP::externalFunctions->FUN_0079EAB0(lines, KEP::TranslationUtility::gettext_main("Damage vs") + " " + iter->first->name, iter->second * 100.0f - 100.0f, true);
		}

		auto& greyedColour = KEP::GUIColor::getGreyed();
		auto& secondaryColour = KEP::GUIColor::getSecondary();

		lines.push_back(StringPair(greyedColour + "----------------", ""));

		auto weight = self->getItemWeight();
		lines.push_back(
			StringPair(
				secondaryColour + "-" + KEP::TranslationUtility::gettext_main("Weight"),
				secondaryColour + KEP::TranslationUtility::format_main(boost::locale::format(boost::locale::translate(msg_kg)) % static_cast<int>(weight))
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

		if (KEP::settings._weaponExtension)
		{
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
	}

	void (*CharStats_setWeapon_orig)(CharStats*, Weapon*);
	void CharStats_setWeapon_hook(CharStats* self, Weapon* _weapon)
	{
		CharStats_setWeapon_orig(self, _weapon);

		if (KEP::settings._weaponExtension)
		{
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
							if (bonusIter == self->bonusRaces.end())
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
	}

	void (*Crossbow_getTooltipData2_orig)(Crossbow*, Ogre::vector<StringPair>::type&);
	void Crossbow_getTooltipData2_hook(Crossbow* self, Ogre::vector<StringPair>::type& lines)
	{
		Crossbow_getTooltipData2_orig(self, lines);
		if (KEP::settings._weaponExtension && !self->crafter.empty())
		{
			auto& mainColour = KEP::GUIColor::getMain();
			lines.push_back(StringPair("", ""));

			lines.push_back(
				StringPair(
					mainColour + "[" + KEP::TranslationUtility::format_main(boost::locale::format(boost::locale::translate(msg_craftedBy)) % self->crafter) + "]"
				)
			);

			auto& secondaryColour = KEP::GUIColor::getSecondary();
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
		if (KEP::settings._weaponExtension && !self->crafter.empty())
		{
			auto& mainColour = KEP::GUIColor::getMain();
			lines.push_back(StringPair("", ""));

			lines.push_back(
				StringPair(
					mainColour + "[" + KEP::TranslationUtility::format_main(boost::locale::format(boost::locale::translate(msg_craftedBy)) % self->crafter) + "]"
				)
			);

			auto& secondaryColour = KEP::GUIColor::getSecondary();
			auto id = self->level_0_100 < 70 ? "Crafted by the smith {1} of {2}." : "High quality armour crafted by the renowned smith {1} of {2}.";
			lines.push_back(
				StringPair(
					secondaryColour + KEP::TranslationUtility::format_main(boost::locale::format(boost::locale::translate(id)) % self->crafter % ou->player->participant->name)
				)
			);
		}
	}

	const ogre_unordered_map<GameData*, int>::type& _getSortKeys()
	{
		static ogre_unordered_map<GameData*, int>::type sortKeys;

		if (sortKeys.size() != 0)
			return sortKeys;

		sortKeys[nullptr] = 999;

		lektor<GameData*> list;
		ou->gamedata.getDataOfType(list, LOCATIONAL_DAMAGE);

		for (auto iter = list.begin(); iter != list.end(); ++iter)
		{
			auto dat = *iter;
			int sortkey = 80;
			if (dat->stringID == "32-gamedata.quack")
				sortkey = 0;
			else if (dat->stringID == "101-gamedata.quack")
				sortkey = 1;
			else if (dat->stringID == "100-gamedata.quack")
				sortkey = 2;
			else if (dat->stringID == "28-gamedata.quack")
				sortkey = 3;
			else if (dat->stringID == "4019-gamedata.base")
				sortkey = 4;
			else if (dat->stringID == "29-gamedata.quack")
				sortkey = 5;
			else if (dat->stringID == "4018-gamedata.base")
				sortkey = 6;
			else if (dat->stringID == "30-gamedata.quack")
				sortkey = 7;
			else if (dat->stringID == "31-gamedata.quack")
				sortkey = 8;
			else
			{
				int bodyPart = dat->idata["body part type"];
				if (bodyPart == 0)
				{
					sortkey = 20;
					if (!dat->bdata["is chest"])
						sortkey += 10;
				}
				else if (bodyPart == 1)
				{
					sortkey = 60;
					if (dat->idata["body part type"] == 0x10)
						sortkey += 10;
				}
				else if (bodyPart == 2)
				{
					sortkey = 40;
					if (dat->idata["body part type"] == 0x2)
						sortkey += 10;
				}
				else if (bodyPart == 3)
					sortkey = 10;
			}

			sortKeys[dat] = sortkey;
		}

		return sortKeys;
	}

	int _getSortKeyOfLocationalDamage(const GameDataReference& dataReference)
	{
		auto dat = dataReference.getPtr(&ou->gamedata);
		auto& sortKeys = _getSortKeys();
		auto it = sortKeys.find(dat);
		if (it != sortKeys.end())
			return it->second;
		return 999;
	}

	struct LessLocationalDamage
	{
		bool operator()(const GameDataReference& a, const GameDataReference& b) const
		{
			return _getSortKeyOfLocationalDamage(a) < _getSortKeyOfLocationalDamage(b);
		}
	};

	void (*setTotalCoverage_orig)(GameData*);
	void setTotalCoverage_hook(GameData* data)
	{
		setTotalCoverage_orig(data);
		if (KEP::settings._weaponExtension)
		{
			auto listIt = data->objectReferences.find("part coverage");
			if (listIt != data->objectReferences.end() && listIt->second.size() != 0)
				std::sort(listIt->second.begin(), listIt->second.end(), LessLocationalDamage());
		}
	}
}

void KEP::ItemExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Sword::setupStats), &Sword_setupStats_hook, &Sword_setupStats_orig))
		ErrorLog("[Sword::setupStats] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharStats::setWeapon), &CharStats_setWeapon_hook, &CharStats_setWeapon_orig))
		ErrorLog("[CharStats::setWeapon] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Sword::_NV_getTooltipData1), &Sword_getTooltipData1_hook, &Sword_getTooltipData1_orig))
		ErrorLog("[Sword::getTooltipData1] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Crossbow::_NV_getTooltipData2), &Crossbow_getTooltipData2_hook, &Crossbow_getTooltipData2_orig))
		ErrorLog("[Crossbow::getTooltipData2] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&RobotLimbItem::_NV_getTooltipData2), &RobotLimbItem_getTooltipData2_hook, &RobotLimbItem_getTooltipData2_orig))
		ErrorLog("[RobotLimbItem::getTooltipData2] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KEP::functions->setTotalCoverage, &setTotalCoverage_hook, &setTotalCoverage_orig))
		ErrorLog("[setTotalCoverage] could not install hook!");
}
