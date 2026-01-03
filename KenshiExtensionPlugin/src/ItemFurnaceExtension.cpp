#include <ogre/OgrePrerequisites.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/Inventory.h>
#include <kenshi/Gear.h>
#include <kenshi/Character.h>

#include <extern/ProductionBuilding.h>
#include <extern/AI.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <ItemFurnaceExtension.h>


namespace
{
	void (*FurnaceBuilding_setupFromData_orig)(FurnaceBuilding*);
	float (*FurnaceBuilding_FUN_002A5F00_orig)(FurnaceBuilding*, Item*);
	void (*FurnaceBuilding_FUN_0029C810_orig)(FurnaceBuilding*, lektor<GameData*>&);
	bool (*AI_haveSomeResourcesFor_orig)(AI*, hand&);
	int (*Task_FillMachine_FUN_00340EB0_orig)(Task_FillMachine*, StorageBuilding*, Inventory*);
	int (*Task_FillMachine_FUN_00343720_orig)(Task_FillMachine*, StorageBuilding*, Inventory*);

	float getCostOfIngredients(FurnaceBuilding* self, GameData* itemGameData, const std::string& stringID)
	{
		float amount = 0.0f;

		auto ingredientsIt = itemGameData->objectReferences.find("ingredients");
		if (ingredientsIt != itemGameData->objectReferences.end())
		{
			for (auto iter = ingredientsIt->second.begin(); iter != ingredientsIt->second.end(); ++iter)
			{
				if (iter->sid == stringID)
					amount += iter->values.value[0] * 0.01f;
				else if (iter->ptr != nullptr)
					amount += getCostOfIngredients(self, iter->ptr, stringID) * iter->values.value[0] * 0.01f;
			}
		}

		return amount;
	}
}

void KEP::ItemFurnaceExtension::FurnaceBuilding_setupFromData_hook(FurnaceBuilding* self)
{
	externalFunctions->FUN_0029E770(self);

	auto& inItems = self->inputItems;
	if (inItems.size() == 0)
	{
		int width = 12;
		int height = 7;
		int stackingBonusMin = 10;
		float stackingBonusMult = 1.0f;

		auto bf_flags = self->functionality->idata["flags"];
		if (bf_flags == 1)
		{
			width = self->data->idata["storage size width"];
			if (width < 1)
				width = 12;

			height = self->data->idata["storage size height"];
			if (height < 1)
				height = 7;

			stackingBonusMin = self->data->idata["stackable bonus minimum"];
			if (stackingBonusMin < 1)
				stackingBonusMin = 10;

			stackingBonusMult = self->data->fdata["stackable bonus mult"];
			if (stackingBonusMult < 0.0f)
				stackingBonusMult = 1.0f;
		}

		std::string inventoryName = "in1";
		self->inventory->initialiseNewSection(inventoryName, width, height, ATTACH_NONE, false, false, true, 0);

		auto section = self->inventory->getSection(inventoryName);

		section->stackingBonusMin = stackingBonusMin;
		section->stackingBonusMult = stackingBonusMult;

		if (bf_flags == 1)
		{
			if (self->itemtypeLimit == WEAPON || self->itemtypeLimit == ARMOUR || self->itemtypeLimit == CONTAINER || self->itemtypeLimit == CROSSBOW || self->itemtypeLimit == LIMB_REPLACEMENT || self->itemtypeLimit == NEST_ITEM || self->itemtypeLimit == MAP_ITEM)
			{
				lektor<GameData*> itemdataList;
				ou->gamedata.getDataOfType(itemdataList, self->itemtypeLimit);
				for (auto iter = itemdataList.begin(); iter != itemdataList.end(); ++iter)
				{
					StorageBuilding::ConsumptionItem* limitItem = new StorageBuilding::ConsumptionItem();
					limitItem->_0x4 = 1.0f;
					limitItem->numStack = -1;
					limitItem->data = *iter;
					limitItem->section = section;

					section->addVeryLimitedSlot(*iter);

					self->storageItems.push_back(limitItem);
				}
			}
			else
			{
				auto limitInventoryIter = self->data->objectReferences.find("limit inventory");
				if (limitInventoryIter != self->data->objectReferences.end())
				{
					for (auto iter = limitInventoryIter->second.begin(); iter != limitInventoryIter->second.end(); ++iter)
					{
						auto gamedata = ou->gamedata.getData(iter->sid);
						if (gamedata != nullptr)
						{
							StorageBuilding::ConsumptionItem* limitItem = new StorageBuilding::ConsumptionItem();
							limitItem->_0x4 = 1.0f;
							limitItem->numStack = -1;
							limitItem->data = gamedata;
							limitItem->section = section;

							section->addVeryLimitedSlot(gamedata);

							self->storageItems.push_back(limitItem);
						}
					}
				}
			}

			section->width = width;
			section->height = height;
		}

		auto& elem = inItems[inItems.size()];
		elem.amount = 0;
		elem._0x4 = 1.0f;
		elem.numStack = 5;
		elem.data = nullptr;
		elem.section = section;
		inItems.push_back(elem);
	}
}

float KEP::ItemFurnaceExtension::FurnaceBuilding_FUN_002A5F00_hook(FurnaceBuilding* self, Item* input)
{
	auto itemBaseData = input->getGameData();

	auto iter = self->functionality->objectReferences.find("furnace ingredients");
	if (iter != self->functionality->objectReferences.end() && iter->second.size() != 0)
	{
		auto ingredientsData = ou->gamedata.getData(iter->second[0].sid);
		if (ingredientsData != nullptr && iter->second[0].sid != "1866-gamedata.base")
		{
			float amount = 0.0f;

			if (input->objectType == WEAPON)
			{
				float weight = input->getItemWeightSingle() * 0.2f;
				if (weight > 5.0f)
					weight = 5.0f;
				else if (weight < 0.5f)
					weight = 0.5f;

				amount = weight;
			}
			else if (input->objectType == ARMOUR)
			{
				amount = Armour::getArmourCraftingMaterialConsumptionRate(input->data) * 2.0f;
			}
			else if (input->objectType == ITEM || input->objectType == CROSSBOW || input->objectType == LIMB_REPLACEMENT || input->objectType == CONTAINER)
			{
				auto ingredientsIt = itemBaseData->objectReferences.find("ingredients");
				if (ingredientsIt != itemBaseData->objectReferences.end() && ingredientsIt->second.size() != 0)
					amount = getCostOfIngredients(self, itemBaseData, iter->second[0].sid) * input->quantity * 0.5f;
			}
			return amount;
		}
	}

	if (input->objectType == CROSSBOW || input->objectType == LIMB_REPLACEMENT || input->objectType == CONTAINER)
	{
		auto ingredientsIt = itemBaseData->objectReferences.find("ingredients");
		if (ingredientsIt != itemBaseData->objectReferences.end() && ingredientsIt->second.size() != 0)
			return externalFunctions->FUN_002A5710(self, itemBaseData) * input->quantity * 0.5f;
	}
	else
	{
		return FurnaceBuilding_FUN_002A5F00_orig(self, input);
	}

	return 0.0f;
}

void KEP::ItemFurnaceExtension::FurnaceBuilding_FUN_0029C810_hook(FurnaceBuilding* self, lektor<GameData*>& out)
{
	if (self->itemtypeLimit == WEAPON || self->itemtypeLimit == ARMOUR || self->itemtypeLimit == CONTAINER || self->itemtypeLimit == CROSSBOW || self->itemtypeLimit == LIMB_REPLACEMENT || self->itemtypeLimit == NEST_ITEM || self->itemtypeLimit == MAP_ITEM)
	{
		ou->gamedata.getDataOfType(out, self->itemtypeLimit);
		return;
	}

	if (self->inventory != nullptr && self->storageItems.size() != 0)
	{
		for (uint32_t i = 0; i < self->storageItems.size(); i++)
		{
			out.push_back(self->storageItems[i]->data);
		}
		return;
	}

	FurnaceBuilding_FUN_0029C810_orig(self, out);
}

bool KEP::ItemFurnaceExtension::AI_haveSomeResourcesFor_hook(AI* self, hand& handle)
{
	auto building = handle.getBuilding();
	if (building != nullptr)
	{
		auto storage = building->getFunctionStuff();
		if (storage != nullptr && storage->itemtypeLimit == ITEM && storage->getSpecialFunction() == BF_ITEM_FURNACE && storage->storageItems.size() != 0)
		{
			for (auto iter = storage->storageItems.begin(); iter != storage->storageItems.end(); ++iter)
			{
				if (self->me->hasItem((*iter)->data))
					return true;
			}
			return false;
		}
	}
	
	return AI_haveSomeResourcesFor_orig(self, handle);
}

int KEP::ItemFurnaceExtension::Task_FillMachine_FUN_00340EB0_hook(Task_FillMachine* self, StorageBuilding* storage, Inventory* inventory)
{
	if (storage->getSpecialFunction() == BF_ITEM_FURNACE && storage->storageItems.size() != 0)
	{
		int count = 0;
		for (auto iter = storage->storageItems.begin(); iter != storage->storageItems.end(); ++iter)
		{
			while ((*iter)->section->hasRoomForItem((*iter)->data, 1))
			{
				auto item = inventory->takeItem_EntireStack((*iter)->data);
				if (!(*iter)->section->addItem(item, 1))
				{
					inventory->addItem(item, 1, true, false);
					break;
				}
				++count;
			}
		}

		if (0 < count)
			externalFunctions->FUN_00299750(reinterpret_cast<FurnaceBuilding*>(storage), nullptr);

		return count;
	}

	return Task_FillMachine_FUN_00340EB0_orig(self, storage, inventory);
}

int KEP::ItemFurnaceExtension::Task_FillMachine_FUN_00343720_hook(Task_FillMachine* self, StorageBuilding* storage, Inventory* inventory)
{
	auto count = Task_FillMachine_FUN_00343720_orig(self, storage, inventory);

	if (0 < count && storage->getSpecialFunction() == BF_ITEM_FURNACE && storage->storageItems.size() != 0)
		externalFunctions->FUN_00299750(reinterpret_cast<FurnaceBuilding*>(storage), nullptr);

	return count;
}

void KEP::ItemFurnaceExtension::init()
{
	if (settings._furnaceExtension)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_0029F940, &FurnaceBuilding_setupFromData_hook, &FurnaceBuilding_setupFromData_orig))
			ErrorLog("KenshiExtensionPlugin: [item furnace extension pt1] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_002A5F00, &FurnaceBuilding_FUN_002A5F00_hook, &FurnaceBuilding_FUN_002A5F00_orig))
			ErrorLog("KenshiExtensionPlugin: [item furnace extension pt2] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_0029C810, &FurnaceBuilding_FUN_0029C810_hook, &FurnaceBuilding_FUN_0029C810_orig))
			ErrorLog("KenshiExtensionPlugin: [item furnace extension pt3] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_005A3B60, &AI_haveSomeResourcesFor_hook, &AI_haveSomeResourcesFor_orig))
			ErrorLog("KenshiExtensionPlugin: [item furnace extension pt4] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_00340EB0, &Task_FillMachine_FUN_00340EB0_hook, &Task_FillMachine_FUN_00340EB0_orig))
			ErrorLog("KenshiExtensionPlugin: [item furnace extension pt5] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_00343720, &Task_FillMachine_FUN_00343720_hook, &Task_FillMachine_FUN_00343720_orig))
			ErrorLog("KenshiExtensionPlugin: [item furnace extension pt6] could not install hook!");
	}
}
