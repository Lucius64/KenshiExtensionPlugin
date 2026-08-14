/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <ogre/OgrePrerequisites.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Logger.h>
#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/RootObjectFactory.h>
#include <kenshi/util/hand.h>
#include <kenshi/Gear.h>
#include <kenshi/Item.h>
#include <kenshi/CharacterHuman.h>
#include <kenshi/CharStats.h>
#include <kenshi/MedicalSystem.h>
#include <kenshi/RaceData.h>
#include <kenshi/Inventory.h>
#include <kenshi/Platoon.h>
#include <kenshi/GunClass.h>

#include <extern/TemporaryLimbInterface.h>
#include <kep/functions.h>

#include <UtilityFunction.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <RaceLimiterExtension.h>

namespace
{
	bool (*RaceLimiter_canEquip_orig)(RaceLimiter*, GameData*, RootObject*);
	bool RaceLimiter_canEquip_hook(RaceLimiter* self, GameData* item, RootObject* who)
	{
		if (KEP::settings._raceEquipmentLimitsExtension)
		{
			if (who->data->type == BUILDING) // 武器キャビネットは武器スロットになっているので、無条件でtrueを返す。
			return true;

		if (who->data == KEP::externalGlobals->_TemporaryLimbInterface) // インベントリの義肢スロットはwhoがTemporaryLimbInterfaceになっているので、所有者を参照する。
				who = static_cast<TemporaryLimbInterface*>(who)->owner;
		}

		bool isAnimal = who->getDataType() == CHARACTER && static_cast<Character*>(who)->isAnimal() != nullptr;

		return self->canEquip(item, who->getRace(), isAnimal);
	}

	RobotLimbItem* (*RobotLimbItem__CONSTRUCTOR_orig)(RobotLimbItem*, GameData*, GameData*, hand&, int);
	RobotLimbItem* RobotLimbItem__CONSTRUCTOR_hook(RobotLimbItem* self, GameData* baseData, GameData* materialData, hand _handle, int _level)
	{
		RobotLimbItem__CONSTRUCTOR_orig(self, baseData, materialData, _handle, _level);
		if (KEP::settings._raceEquipmentLimitsExtension)
			RaceLimiter::getSingleton()->addLimit(baseData);
		return self;
	}

	Weapon* (*Weapon__CONSTRUCTOR_orig)(Weapon*, GameData*, GameData*, GameData*, hand&, int);
	Weapon* Weapon__CONSTRUCTOR_hook(Weapon* self, GameData* baseData, GameData* companyData, GameData* materialData, hand _handle, int _level)
	{
		Weapon__CONSTRUCTOR_orig(self, baseData, companyData, materialData, _handle, _level);
		if (KEP::settings._raceEquipmentLimitsExtension)
			RaceLimiter::getSingleton()->addLimit(baseData);
		return self;
	}

	void _giveGearBlueprints(CharacterHuman* self, const char* listname)
	{
		auto bpList = self->data->getReferenceListIfExists(listname);
		if (bpList != nullptr)
		{
			for (auto iter = bpList->begin(); iter != bpList->end(); ++iter)
			{
				if (0 < iter->values.value[0])
				{
					auto baseData = ou->gamedata.getData(iter->sid + ".TECH.1", RESEARCH);
					if (baseData != nullptr)
					{
						auto item = ou->theFactory->createItem(baseData, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, 0, nullptr);
						if (item == nullptr)
						{
							Logger::logMessage("[CharacterHuman::setupInventorySections] Blueprints '" + iter->sid + "' does not exist.", Logger::Warning);
						}
						else
						{
							item->quantity = 1;
							self->giveItem(item, false, true);
						}
					}
				}
			}
		}
	}

	bool (*CharacterHuman_setupInventorySections_orig)(CharacterHuman*, GameSaveState*);
	bool CharacterHuman_setupInventorySections_hook(CharacterHuman* self, GameSaveState* state)
	{
		if (!KEP::settings._raceEquipmentLimitsExtension && !KEP::settings._extendInitInventory)
			return CharacterHuman_setupInventorySections_orig(self, state);

		bool lockedBootsSlot = false;
		if (self->medical.getLimbState(self->medical.rightLeg->getRobotLimbEnum()) != LIMB_ORIGINAL)
			if (self->medical.getLimbState(self->medical.leftLeg->getRobotLimbEnum()) != LIMB_ORIGINAL)
				lockedBootsSlot = true;

		self->inventory->initialiseNewSection("hip", 7, 1, ATTACH_WEAPON, true, false, true, 1);
		self->inventory->initialiseNewSection("back", 10, 2, ATTACH_WEAPON, true, false, true, 2);

		auto race = self->getRace();
		lockedBootsSlot = race->noShoes || lockedBootsSlot;

		self->inventory->initialiseNewSection("boots", 4, 2, ATTACH_BOOTS, true, false, !lockedBootsSlot, 1);
		self->inventory->initialiseNewSection("head", 4, 3, ATTACH_HAT, true, false, !race->noHats, 1);
		self->inventory->initialiseNewSection("backpack_attach", 5, 4, ATTACH_BACKPACK, true, true, true, 0);
		self->inventory->initialiseNewSection("shirt", 4, 2, ATTACH_SHIRT, true, false, !race->noShirts, 1);
		self->inventory->initialiseNewSection("armour", 4, 6, ATTACH_BODY, true, false, true, 1);
		self->inventory->initialiseNewSection("legs", 4, 5, ATTACH_LEGS, true, false, true, 1);
		self->inventory->initialiseNewSection("belt", 2, 2, ATTACH_BELT, true, false, true, 1);
		self->inventory->initialiseNewSection("main", 8, 10, ATTACH_NONE, false, false, true, 0);

		if (state != nullptr)
		{
			self->inventory->loadFrom(state);
			return self->Character::setupInventorySections(state);
		}

		auto raceLimiter = RaceLimiter::getSingleton();

		auto backpackList = self->data->getReferenceListIfExists("backpack");
		if (backpackList != nullptr)
		{
			FitnessSelector<GameData*> selector;
			for (auto iter = backpackList->begin(); iter != backpackList->end(); ++iter)
			{
				if (0 < iter->values.value[0])
				{
					float chance = static_cast<float>(iter->values.value[1]);
					if (chance == 0.0f)
						chance = 100.0f;

					auto baseData = ou->gamedata.getData(iter->sid, CONTAINER);
					if (baseData != nullptr)
					{
						raceLimiter->addLimit(baseData);
						if (raceLimiter->canEquip(baseData, self))
							selector.addItem(baseData, chance);
					}
				}
			}

			auto baseData = selector.chooseAnItem_absoluteScore();
			if (baseData != nullptr)
			{
				auto item = ou->theFactory->createItem(baseData, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, 0, nullptr);
				if (item == nullptr)
					Logger::logMessage("[CharacterHuman::setupInventorySections] Backpack '" + baseData->name + "' not found.", Logger::Warning);
				else
					self->inventory->addItem(item, 1, false, true);
			}
		}

		auto robotLimbList = self->data->getReferenceListIfExists("robot limbs");
		if (robotLimbList != nullptr)
		{
			std::array<FitnessSelector<int>, 4> selectors;

			int count = 0;
			for (auto iter = robotLimbList->begin(); iter != robotLimbList->end(); ++iter)
			{
				auto baseData = iter->getPtr(&ou->gamedata);
				if (baseData != nullptr && KEP::settings._raceEquipmentLimitsExtension)
					raceLimiter->addLimit(baseData);
				if (baseData != nullptr && (iter->values.value[0] <= -1 || baseData->listExistsAndNotEmpty("material") && raceLimiter->canEquip(baseData, race, false)))
				{
					int slot = baseData->idata["slot"] - 0x32;
					if (0 <= slot && slot < 4)
						selectors[slot].addItem(count, static_cast<float>(iter->values.value[1]));
				}
				++count;
			}

			for (uint32_t i = 0; i < 4; ++i)
			{
				if (selectors[i].size() != 0)
				{
					auto listIndex = selectors[i].chooseAnItem();
					auto level = (*robotLimbList)[listIndex].values.value[0];
					if (-1 < level)
					{
						auto baseData = (*robotLimbList)[listIndex].getPtr(&ou->gamedata); // nullチェック不要
						auto item = ou->theFactory->createItem(baseData, KEP::functions->getNULL_HAND(), nullptr, nullptr, level, nullptr);
						self->medical.setRobotLimbItem(static_cast<RobotLimbs::Limb>(i), item, false);
					}
				}

			}
			self->validateInventorySections();
		}

		lockedBootsSlot = self->medical.leftLeg->robotLimb != nullptr && self->medical.rightLeg->robotLimb != nullptr;

		auto armourGrade = self->data->idata["armour grade"];
		auto upgradeChance = self->data->idata["armour upgrade chance"];
		auto uniformFaction = self->data->bdata["wears uniform"] ? self->getFaction() : nullptr;

		lektor<GameData*> clothingList;
		RootObjectFactory::chooseMyClothing(clothingList, self->data, "clothing", race, lockedBootsSlot);
		for (auto iter = clothingList.begin(); iter != clothingList.end(); ++iter)
		{
			if (*iter != nullptr)
			{
				int upgrade = 0;
				if (armourGrade < 5)
					upgrade = UtilityT::randomInt(0, 100) < upgradeChance;
				Item* item = ou->theFactory->createItem(*iter, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, KEP::functions->convertRarityToLevel(armourGrade + upgrade), uniformFaction);
				if (item != nullptr)
					self->inventory->addItem(item, 1, false, true);
			}
		}

		bool hasCrossbow = false;

		auto crossbowList = self->data->getReferenceListIfExists("crossbows");
		if (crossbowList != nullptr)
		{
			FitnessSelector<GameData*> selector;

			for (auto iter = crossbowList->begin(); iter != crossbowList->end(); ++iter)
			{
				if (0 < iter->values.value[0])
				{
					auto baseData = ou->gamedata.getData(iter->sid, CROSSBOW);
					if (baseData != nullptr)
					{
						if (KEP::settings._raceEquipmentLimitsExtension)
							raceLimiter->addLimit(baseData);
						if (raceLimiter->canEquip(baseData, self))
							selector.addItem(baseData, static_cast<float>(iter->values.value[1]));
					}
				}
			}

			auto baseData = selector.chooseAnItem_absoluteScore();
			if (baseData != nullptr)
			{
				FitnessSelector<uint32_t> rairitySelector;
				KEP::functions->crossbowLevelSelector(rairitySelector, self->platoon->me->squadTemplate);

				auto item = ou->theFactory->createItem(baseData, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, KEP::functions->convertRarityToLevel(rairitySelector.chooseAnItem()), nullptr);
				if (item == nullptr)
				{
					Logger::logMessage("[CharacterHuman::setupInventorySections] Item '" + baseData->name + "' does not exist.", Logger::Warning);
				}
				else
				{
					if (self->giveItem(item, false, false))
					{
						hasCrossbow = true;
						auto crossbow = item->isCrossbow();
						if (crossbow != nullptr)
						{
							auto ammo = crossbow->gunClass->ammoType;
							if (ammo != nullptr)
							{
								auto ammoItem = ou->theFactory->createItem(ammo, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, 0, nullptr);
								self->giveItem(ammoItem, false, true);
							}
						}
					}
					else
					{
						ou->destroy(item, false, "undefined");
					}
				}
			}
		}

		auto weaponList = self->data->getReferenceList("weapons");
		auto weaponLevelList = self->data->getReferenceList("weapon level");
		FitnessSelector<const GameDataReference*> hipSlotSelector;
		FitnessSelector<const GameDataReference*> backSlotSelector;

		for (auto iter = weaponList->begin(); iter != weaponList->end(); ++iter)
		{
			auto& gameDataReference = *iter;
			if (0 < gameDataReference.values.value[0])
			{
				auto weaponGameData = ou->gamedata.getData(gameDataReference.sid, WEAPON);
				if (weaponGameData != nullptr)
				{
					if (KEP::settings._raceEquipmentLimitsExtension)
						raceLimiter->addLimit(weaponGameData);
					if (raceLimiter->canEquip(weaponGameData, self))
					{
						float chance = static_cast<float>(gameDataReference.values.value[2]);
						if (chance == 0.0f)
							chance = 100.0f;

						if (gameDataReference.values.value[1] == 0)
							hipSlotSelector.addItem(&gameDataReference, chance);
						else
							backSlotSelector.addItem(&gameDataReference, chance);
					}
				}
			}
		}

		lektor<const GameDataReference*> choosedWeapons;
		choosedWeapons.push_back(hipSlotSelector.chooseAnItem_absoluteScore());
		choosedWeapons.push_back(backSlotSelector.chooseAnItem_absoluteScore());

		for (uint32_t i = 0; i < 2; ++i)
		{
			if (choosedWeapons[i] != nullptr)
			{
				auto baseData = choosedWeapons[i]->getPtr(&ou->gamedata);

				if (baseData != nullptr && (baseData->idata["inventory footprint height"] < 2 || !hasCrossbow))
				{
					GameData* companyData = nullptr;
					if (weaponLevelList->size() == 0)
					{
						companyData = ou->gamedata.getData("917-gamedata.base", WEAPON_MANUFACTURER);
					}
					else
					{
						FitnessSelector<GameData*> selector;
						for (auto iter = weaponLevelList->begin(); iter != weaponLevelList->end(); ++iter)
						{
							float chance = static_cast<float>(iter->values.value[0]);
							if (chance == 0.0f)
								chance = 100.0f;

							auto companyGameData = ou->gamedata.getData(iter->sid, WEAPON_MANUFACTURER);
							if (companyGameData != nullptr)
								selector.addItem(companyGameData, chance);
						}
						companyData = selector.chooseAnItem();
					}

					if (companyData == nullptr)
					{
						Logger::logMessage("[CharacterHuman::setupInventorySections] Invalid weapon setup. No specified manufacturer contains " + baseData->name, Logger::Warning);
					}
					else
					{
						auto item = self->generateWeapon(baseData, companyData);
						if (item == nullptr)
						{
							Logger::logMessage("[CharacterHuman::setupInventorySections] Failed to create weapon '" + baseData->stringID + " : " + companyData->name + "'.", Logger::Warning);
						}
						else
						{
							InventorySection* section = self->inventory->getSection(choosedWeapons[i]->values.value[1] == 0 ? "hip" : "back");

							if (!section->addItem(item, 1) && !self->inventory->addItem(item, 1, false, false))
							{
								ou->destroy(item, false, "setupInventorySection");
								break;
							}
							if (1 < choosedWeapons[i]->values.value[0])
							{
								auto cloned = ou->theFactory->copyItem(item);
								if (!section->addItem(cloned, 1))
								{
									ou->destroy(cloned, false, "setupInventorySection");
									break;
								}
							}
						}
					}
				}
			}
		}

		auto blueprintList = self->data->getReferenceListIfExists("blueprints");
		if (blueprintList != nullptr)
		{
			for (auto iter = blueprintList->begin(); iter != blueprintList->end(); ++iter)
			{
				if (0 < iter->values.value[0])
				{
					auto baseData = ou->gamedata.getData(iter->sid, RESEARCH);
					if (baseData != nullptr)
					{
						auto item = ou->theFactory->createItem(baseData, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, 0, nullptr);
						if (item == nullptr)
						{
							Logger::logMessage("[CharacterHuman::setupInventorySections] Blueprints '" + iter->sid + "' does not exist.", Logger::Warning);
						}
						else
						{
							item->quantity = 1;
							self->giveItem(item, false, true);
						}
					}
				}
			}
		}

		if (KEP::settings._extendInitInventory)
		{
			_giveGearBlueprints(self, "armour blueprints");
			_giveGearBlueprints(self, "backpack blueprints");
			_giveGearBlueprints(self, "crossbow blueprints");
		}

		auto inventoryList = self->data->getReferenceListIfExists("inventory");
		if (inventoryList != nullptr)
		{
			for (auto iter = inventoryList->begin(); iter != inventoryList->end(); ++iter)
			{
				if (0 < iter->values.value[0])
				{
					auto baseData = ou->gamedata.getData(iter->sid, ITEM);
					if (baseData != nullptr)
					{
						auto item = ou->theFactory->createItem(baseData, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, 0, nullptr);
						if (item == nullptr)
						{
							Logger::logMessage("[CharacterHuman::setupInventorySections] Item '" + iter->sid + "' does not exist.", Logger::Warning);
						}
						else
						{
							item->quantity = iter->values.value[0];
							if (item->quantity == 1 && 1.0f < item->chargesLeft)
								item->resetCharges(true);

							self->giveItem(item, false, true);
						}
					}
				}
			}
		}

		if (KEP::settings._extendInitInventory)
		{
			auto backPack = self->hasABackpackOn();
			if (backPack != nullptr)
			{
				auto list = self->data->getReferenceListIfExists("backpack inventory");
				if (list != nullptr)
				{
					for (auto iter = list->begin(); iter != list->end(); ++iter)
					{
						backPack->getInventory()->fillFromVendorList(iter->getPtr(&ou->gamedata), self->getFaction());
					}
				}
			}
		}

		return self->Character::setupInventorySections(state);
	}

	void (*RaceLimiter_addLimit_orig)(RaceLimiter*, GameData*);
	void RaceLimiter_addLimit_hook(RaceLimiter* self, GameData* dat)
	{
		RaceLimiter_addLimit_orig(self, dat);
		if (!KEP::settings._raceEquipmentLimitsExtension)
			return;

		auto limiterIter = self->limits.find(dat);
		if (limiterIter == self->limits.end())
			return;

		auto excludeList = dat->getReferenceListIfExists("race groups exclude");
		if (excludeList != nullptr)
		{
			for (auto iter = excludeList->begin(); iter != excludeList->end(); ++iter)
			{
				auto raceGroup = iter->getPtr(&ou->gamedata);
				if (raceGroup == nullptr)
				continue;

				auto& races = raceGroup->objectReferences["races"];
				for (auto racesIter = races.begin(); racesIter != races.end(); ++racesIter)
			{
				auto gameData = ou->gamedata.getData(racesIter->sid, RACE);
				if (gameData == nullptr)
					continue;

				auto race = RaceData::getRaceData(gameData);
				if (race == nullptr)
					continue;

				if (limiterIter->second.racesInclude.count(race) == 0)
					limiterIter->second.racesExclude.insert(race);
			}
		}
		}

		auto includeList = dat->getReferenceListIfExists("race groups include");
		if (includeList != nullptr)
		{
			for (auto iter = includeList->begin(); iter != includeList->end(); ++iter)
			{
				auto raceGroup = iter->getPtr(&ou->gamedata);
				if (raceGroup == nullptr)
				continue;

				auto& races = raceGroup->objectReferences["races"];
				for (auto racesIter = races.begin(); racesIter != races.end(); ++racesIter)
			{
				auto gameData = ou->gamedata.getData(racesIter->sid, RACE);
				if (gameData == nullptr)
					continue;

				auto race = RaceData::getRaceData(gameData);
				if (race == nullptr)
					continue;

				limiterIter->second.racesInclude.insert(race);
			}
		}
	}
	}
}

void KEP::RaceLimiterExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress((bool (RaceLimiter::*)(GameData*, RootObject*)) & RaceLimiter::_NV_canEquip), &RaceLimiter_canEquip_hook, &RaceLimiter_canEquip_orig))
			ErrorLog("[RaceLimiter::canEquip] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&RobotLimbItem::_CONSTRUCTOR), &RobotLimbItem__CONSTRUCTOR_hook, &RobotLimbItem__CONSTRUCTOR_orig))
			ErrorLog("[RobotLimbItem::RobotLimbItem] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Weapon::_CONSTRUCTOR), &Weapon__CONSTRUCTOR_hook, &Weapon__CONSTRUCTOR_orig))
		ErrorLog("[Weapon::Weapon] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&CharacterHuman::_NV_setupInventorySections), &CharacterHuman_setupInventorySections_hook, &CharacterHuman_setupInventorySections_orig))
		ErrorLog("[CharacterHuman::setupInventorySections] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&RaceLimiter::addLimit), &RaceLimiter_addLimit_hook, &RaceLimiter_addLimit_orig))
		ErrorLog("[RaceLimiter::addLimit] could not install hook!");
}
