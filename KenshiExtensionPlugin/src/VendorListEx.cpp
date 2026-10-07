/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreStringConverter.h>

#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/RootObjectFactory.h>
#include <kenshi/Faction.h>
#include <kenshi/Platoon.h>
#include <kenshi/Character.h>
#include <kenshi/Inventory.h>
#include <extern/VendorList.h>

#include <kep/functions.h>
#include <UtilityFunction.h>
#include <Settings.h>
#include <VendorListEx.h>

namespace
{
	void (*VendorList_crossbowLevelSelector_orig)(FitnessSelector<uint32_t>&, GameData*);
	void VendorList_crossbowLevelSelector_hook(FitnessSelector<uint32_t>& selector, GameData* squadtemplate)
	{
		if (KEP::settings._vendorListEx)
		{
			selector.addItem(0, static_cast<float>(squadtemplate->idata["crossbows 0"]));
			selector.addItem(1, static_cast<float>(squadtemplate->idata["crossbows 1"]));
			selector.addItem(2, static_cast<float>(squadtemplate->idata["crossbows 2"]));
			selector.addItem(3, static_cast<float>(squadtemplate->idata["crossbows 3"]));
			selector.addItem(4, static_cast<float>(squadtemplate->idata["crossbows 4"]));
			selector.addItem(5, static_cast<float>(squadtemplate->idata["crossbows 5"]));
			if (!selector.empty())
				return;
		}
		VendorList_crossbowLevelSelector_orig(selector, squadtemplate);
	}

	void (*VendorList_roboticsLevelSelector_orig)(FitnessSelector<uint32_t>&, GameData*);
	void VendorList_roboticsLevelSelector_hook(FitnessSelector<uint32_t>& selector, GameData* squadtemplate)
	{
		if (KEP::settings._vendorListEx)
		{
			selector.addItem(0, static_cast<float>(squadtemplate->idata["robotics 0"]));
			selector.addItem(1, static_cast<float>(squadtemplate->idata["robotics 1"]));
			selector.addItem(2, static_cast<float>(squadtemplate->idata["robotics 2"]));
			selector.addItem(3, static_cast<float>(squadtemplate->idata["robotics 3"]));
			selector.addItem(4, static_cast<float>(squadtemplate->idata["robotics 4"]));
			selector.addItem(5, static_cast<float>(squadtemplate->idata["robotics 5"]));
			if (!selector.empty())
				return;
		}
		VendorList_roboticsLevelSelector_orig(selector, squadtemplate);
	}

	void (*VendorList_createItem_orig)(VendorList*, lektor<Item*>&, int, GameData*);
	void VendorList_createItem_hook(VendorList* self, lektor<Item*>& items, int amount, GameData* squadtemplate)
	{
		if (KEP::settings._vendorListEx)
		{
			FitnessSelector<uint32_t> temp = self->armorLevelSelector;
			FitnessSelector<uint32_t> selector;
			selector.addItem(0, static_cast<float>(squadtemplate->idata["armors 0"]));
			selector.addItem(1, static_cast<float>(squadtemplate->idata["armors 1"]));
			selector.addItem(2, static_cast<float>(squadtemplate->idata["armors 2"]));
			selector.addItem(3, static_cast<float>(squadtemplate->idata["armors 3"]));
			selector.addItem(4, static_cast<float>(squadtemplate->idata["armors 4"]));
			selector.addItem(5, static_cast<float>(squadtemplate->idata["armors 5"]));
			if (!selector.empty())
			{
				self->armorLevelSelector = selector;
				VendorList_createItem_orig(self, items, amount, squadtemplate);
				self->armorLevelSelector = temp;
				return;
			}
		}
		VendorList_createItem_orig(self, items, amount, squadtemplate);
	}

	bool (*VendorListManager_hasSpecialItemsList_orig)(GameData*);
	bool VendorListManager_hasSpecialItemsList_hook(GameData* squadtemplate)
	{
		bool result = VendorListManager_hasSpecialItemsList_orig(squadtemplate);
		if (!KEP::settings._vendorListEx || squadtemplate == nullptr)
			return result;

		result = result || squadtemplate->getReferenceListIfExists("special containers") != nullptr;
		result = result || squadtemplate->getReferenceListIfExists("special armours") != nullptr;
		result = result || squadtemplate->getReferenceListIfExists("special crossbows") != nullptr;
		result = result || squadtemplate->getReferenceListIfExists("special robotics") != nullptr;
		result = result || squadtemplate->getReferenceListIfExists("special weapons") != nullptr;
		return result;
	}

	void (*VendorListManager_createSpecialItem_orig)(void*, GameData*, lektor<Item*>&);
	void VendorListManager_createSpecialItem_hook(void* self, GameData* squadtemplate, lektor<Item*>& items)
	{
		VendorListManager_createSpecialItem_orig(self, squadtemplate, items);
		if (KEP::settings._vendorListEx && squadtemplate != nullptr)
		{
			auto containers = squadtemplate->getReferenceListIfExists("special containers");
			if (containers != nullptr)
			{
				auto endContainers = containers->end();
				for (auto iter = containers->begin(); iter != endContainers; ++iter)
				{
					auto data = iter->getPtr(&ou->gamedata);
					int quantity = iter->values.value[0];
					if (data != nullptr && data->type == CONTAINER)
					{
						for (size_t i = 0; i < quantity; ++i)
						{
							Item* item = ou->theFactory->createItem(data, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, 0, nullptr);
							item->isUnique = true;
							items.push_back(item);
						}
					}
				}
			}

			auto armors = squadtemplate->getReferenceListIfExists("special armours");
			if (armors != nullptr)
			{
				auto endArmors = armors->end();
				for (auto iter = armors->begin(); iter != endArmors; ++iter)
				{
					auto data = iter->getPtr(&ou->gamedata);
					int quantityOfMasterwork = iter->values.value[0];
					int quantityOfSpecialist = iter->values.value[1];
					if (data != nullptr && data->type == ARMOUR)
					{
						for (size_t i = 0; i < quantityOfMasterwork; ++i)
						{
							Item* item = ou->theFactory->createItem(data, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, KEP::functions->convertRarityToLevel(5), nullptr);
							item->isUnique = true;
							items.push_back(item);
						}
						for (size_t i = 0; i < quantityOfSpecialist; ++i)
						{
							Item* item = ou->theFactory->createItem(data, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, KEP::functions->convertRarityToLevel(4), nullptr);
							item->isUnique = true;
							items.push_back(item);
						}
					}
				}
			}

			auto crossbows = squadtemplate->getReferenceListIfExists("special crossbows");
			if (crossbows != nullptr)
			{
				auto endCrossbows = crossbows->end();
				for (auto iter = crossbows->begin(); iter != endCrossbows; ++iter)
				{
					auto data = iter->getPtr(&ou->gamedata);
					int quantityOfMasterwork = iter->values.value[0];
					int quantityOfSpecialist = iter->values.value[1];
					if (data != nullptr && data->type == CROSSBOW)
					{
						for (size_t i = 0; i < quantityOfMasterwork; ++i)
						{
							Item* item = ou->theFactory->createItem(data, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, KEP::functions->convertRarityToLevel(5), nullptr);
							item->isUnique = true;
							items.push_back(item);
						}
						for (size_t i = 0; i < quantityOfSpecialist; ++i)
						{
							Item* item = ou->theFactory->createItem(data, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, KEP::functions->convertRarityToLevel(4), nullptr);
							item->isUnique = true;
							items.push_back(item);
						}
					}
				}
			}

			auto robotics = squadtemplate->getReferenceListIfExists("special robotics");
			if (robotics != nullptr)
			{
				auto endRobotics = robotics->end();
				for (auto iter = robotics->begin(); iter != endRobotics; ++iter)
				{
					auto data = iter->getPtr(&ou->gamedata);
					int quantityOfMasterwork = iter->values.value[0];
					int quantityOfSpecialist = iter->values.value[1];
					if (data != nullptr && data->type == LIMB_REPLACEMENT)
					{
						for (size_t i = 0; i < quantityOfMasterwork; ++i)
						{
							Item* item = ou->theFactory->createItem(data, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, KEP::functions->convertRarityToLevel(5), nullptr);
							item->isUnique = true;
							items.push_back(item);
						}
						for (size_t i = 0; i < quantityOfSpecialist; ++i)
						{
							Item* item = ou->theFactory->createItem(data, hand(0, 0, NULL_ITEM, 0, 0), nullptr, nullptr, KEP::functions->convertRarityToLevel(4), nullptr);
							item->isUnique = true;
							items.push_back(item);
						}
					}
				}
			}

			auto weaponManufacturers = squadtemplate->getReferenceListIfExists("special weapon manufacturers");
			FitnessSelector<GameData*> manufacturersSelector;
			if (weaponManufacturers != nullptr)
			{
				auto endWeaponManufacturers = weaponManufacturers->end();
				for (auto iter = weaponManufacturers->begin(); iter != endWeaponManufacturers; ++iter)
				{
					auto data = iter->getPtr(&ou->gamedata);
					if (data != nullptr && data->type == WEAPON_MANUFACTURER)
						manufacturersSelector.addItem(data, static_cast<float>(iter->values.value[0]));
				}
			}
			else
			{
				manufacturersSelector.addItem(ou->gamedata.getData("1070-gamedata.base", WEAPON_MANUFACTURER), 100.0f);
			}

			auto weapons = squadtemplate->getReferenceListIfExists("special weapons");
			if (weapons != nullptr)
			{
				auto endWeapons = weapons->end();
				for (auto iter = weapons->begin(); iter != endWeapons; ++iter)
				{
					auto data = iter->getPtr(&ou->gamedata);
					int quantity = iter->values.value[0];
					if (data->type == WEAPON)
					{
						for (size_t i = 0; i < quantity; ++i)
						{
							auto manufacturer = manufacturersSelector.chooseAnItem();
							if (manufacturer == nullptr)
								break;

							GameData* model = nullptr;

							auto specialWeaponModels = squadtemplate->getReferenceListIfExists("special weapon models");
							if (specialWeaponModels != nullptr)
							{
								FitnessSelector<GameData*> modelsSelector;
								auto models = manufacturer->getReferenceListIfExists("weapon models");
								if (models != nullptr)
								{
									auto endSpecialWeaponModels = specialWeaponModels->end();
									for (auto specialWeaponModelsIt = specialWeaponModels->begin(); specialWeaponModelsIt != endSpecialWeaponModels; ++specialWeaponModelsIt)
									{
										auto endModels = models->end();
										for (auto modelsIt = models->begin(); modelsIt != endModels; ++modelsIt)
										{
											if (modelsIt->sid == specialWeaponModelsIt->sid)
											{
												modelsSelector.addItem(specialWeaponModelsIt->getPtr(&ou->gamedata), static_cast<float>(specialWeaponModelsIt->values.value[0]));
												break;
											}
										}
									}
								}
								model = modelsSelector.chooseAnItem();
							}

							Item* item = ou->theFactory->createItem(manufacturer, hand(0, 0, NULL_ITEM, 0, 0), data, model, 0, nullptr);
							item->isUnique = true;
							items.push_back(item);
						}
					}
				}
			}
		}
	}
}

void KEP::VendorListEx::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->crossbowLevelSelector, &VendorList_crossbowLevelSelector_hook, &VendorList_crossbowLevelSelector_orig))
		ErrorLog("[VendorList::crossbowLevelSelector] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->roboticsLevelSelector, &VendorList_roboticsLevelSelector_hook, &VendorList_roboticsLevelSelector_orig))
		ErrorLog("[VendorList::roboticsLevelSelector] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->VendorList_createItem, &VendorList_createItem_hook, &VendorList_createItem_orig))
		ErrorLog("[VendorList::createItem] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->VendorListManager_hasSpecialItemsList, &VendorListManager_hasSpecialItemsList_hook, &VendorListManager_hasSpecialItemsList_orig))
		ErrorLog("[VendorListManager::hasSpecialItemsList] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->VendorListManager_createSpecialItem, &VendorListManager_createSpecialItem_hook, &VendorListManager_createSpecialItem_orig))
		ErrorLog("[VendorListManager::createSpecialItem] could not install hook!");
}
