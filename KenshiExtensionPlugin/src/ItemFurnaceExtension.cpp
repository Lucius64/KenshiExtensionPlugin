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
#include <kenshi/Building/FurnaceBuilding.h>

#include <extern/AI.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <ItemFurnaceExtension.h>


namespace
{
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

	void (*FurnaceBuilding_setupFromData_orig)(FurnaceBuilding*);
	void FurnaceBuilding_setupFromData_hook(FurnaceBuilding* self)
	{
		self->ProductionBuilding::setupFromData();

		auto& inItems = self->consumptionItems;
		if (inItems.size() == 0)
		{
			int width = 12;
			int height = 7;
			int stackingBonusMin = 10;
			float stackingBonusMult = 1.0f;

			auto bf_flags = self->functionalityData->idata["flags"];
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
				if (self->specialItemTypesOnly == WEAPON || self->specialItemTypesOnly == ARMOUR || self->specialItemTypesOnly == CONTAINER || self->specialItemTypesOnly == CROSSBOW || self->specialItemTypesOnly == LIMB_REPLACEMENT || self->specialItemTypesOnly == NEST_ITEM || self->specialItemTypesOnly == MAP_ITEM)
				{
					lektor<GameData*> itemdataList;
					ou->gamedata.getDataOfType(itemdataList, self->specialItemTypesOnly);
					for (auto iter = itemdataList.begin(); iter != itemdataList.end(); ++iter)
					{
						StorageBuilding::ConsumptionItem* limitItem = new StorageBuilding::ConsumptionItem(1.0f, *iter, section, -1);

						section->addVeryLimitedSlot(*iter);

						self->manyLimitItems.push_back(limitItem);
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
								StorageBuilding::ConsumptionItem* limitItem = new StorageBuilding::ConsumptionItem(1.0f, gamedata, section, -1);

								section->addVeryLimitedSlot(gamedata);

								self->manyLimitItems.push_back(limitItem);
							}
						}
					}
				}

				section->width = width;
				section->height = height;
			}

			auto& elem = inItems[inItems.size()];
			elem.amount = 0;
			elem.rate = 1.0f;
			elem.maxCapacity = 5;
			elem.item = nullptr;
			elem.inventorySection = section;
			inItems.push_back(elem);
		}
	}

	float (*FurnaceBuilding_getInputValue_orig)(FurnaceBuilding*, Item*);
	float FurnaceBuilding_getInputValue_hook(FurnaceBuilding* self, Item* input)
	{
		auto itemBaseData = input->getGameData();

		auto iter = self->functionalityData->objectReferences.find("furnace ingredients");
		if (iter != self->functionalityData->objectReferences.end() && iter->second.size() != 0)
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
				return self->getIronAmountInItem(itemBaseData) * input->quantity * 0.5f;
		}
		else
		{
			return FurnaceBuilding_getInputValue_orig(self, input);
		}

		return 0.0f;
	}

	void (*FurnaceBuilding_getResourcesNeededBecauseEmpty_orig)(FurnaceBuilding*, lektor<GameData*>&);
	void FurnaceBuilding_getResourcesNeededBecauseEmpty_hook(FurnaceBuilding* self, lektor<GameData*>& out)
	{
		if (self->specialItemTypesOnly == WEAPON || self->specialItemTypesOnly == ARMOUR || self->specialItemTypesOnly == CONTAINER || self->specialItemTypesOnly == CROSSBOW || self->specialItemTypesOnly == LIMB_REPLACEMENT || self->specialItemTypesOnly == NEST_ITEM || self->specialItemTypesOnly == MAP_ITEM)
		{
			ou->gamedata.getDataOfType(out, self->specialItemTypesOnly);
			return;
		}

		if (self->inventory != nullptr && self->manyLimitItems.size() != 0)
		{
			for (uint32_t i = 0; i < self->manyLimitItems.size(); i++)
			{
				out.push_back(self->manyLimitItems[i]->item);
			}
			return;
		}

		FurnaceBuilding_getResourcesNeededBecauseEmpty_orig(self, out);
	}

	bool (*AI_haveSomeResourcesFor_orig)(AI*, hand&);
	bool AI_haveSomeResourcesFor_hook(AI* self, hand& handle)
	{
		auto building = handle.getBuilding();
		if (building != nullptr)
		{
			auto storage = building->getFunctionStuff();
			if (storage != nullptr && storage->specialItemTypesOnly == ITEM && storage->getSpecialFunction() == BF_ITEM_FURNACE && storage->manyLimitItems.size() != 0)
			{
				for (auto iter = storage->manyLimitItems.begin(); iter != storage->manyLimitItems.end(); ++iter)
				{
					if (self->me->hasItem((*iter)->item))
						return true;
				}
				return false;
			}
		}

		return AI_haveSomeResourcesFor_orig(self, handle);
	}

	int (*Task_FillMachine_FUN_00340EB0_orig)(Task_FillMachine*, StorageBuilding*, Inventory*);
	int Task_FillMachine_FUN_00340EB0_hook(Task_FillMachine* self, StorageBuilding* storage, Inventory* inventory)
	{
		if (storage->getSpecialFunction() == BF_ITEM_FURNACE && storage->manyLimitItems.size() != 0)
		{
			int count = 0;
			for (auto iter = storage->manyLimitItems.begin(); iter != storage->manyLimitItems.end(); ++iter)
			{
				while ((*iter)->inventorySection->hasRoomForItem((*iter)->item, 1))
				{
					auto item = inventory->takeItem_EntireStack((*iter)->item);
					if (!(*iter)->inventorySection->addItem(item, 1))
					{
						inventory->addItem(item, 1, true, false);
						break;
					}
					++count;
				}
			}

			if (0 < count)
				reinterpret_cast<FurnaceBuilding*>(storage)->incinerate(nullptr);

			return count;
		}

		return Task_FillMachine_FUN_00340EB0_orig(self, storage, inventory);
	}

	int (*Task_FillMachine_FUN_00343720_orig)(Task_FillMachine*, StorageBuilding*, Inventory*);
	int Task_FillMachine_FUN_00343720_hook(Task_FillMachine* self, StorageBuilding* storage, Inventory* inventory)
	{
		auto count = Task_FillMachine_FUN_00343720_orig(self, storage, inventory);

		if (0 < count && storage->getSpecialFunction() == BF_ITEM_FURNACE && storage->manyLimitItems.size() != 0)
			reinterpret_cast<FurnaceBuilding*>(storage)->incinerate(nullptr);

		return count;
	}
}

void KEP::ItemFurnaceExtension::init()
{
	if (settings._furnaceExtension)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&FurnaceBuilding::_NV_setupFromData), &FurnaceBuilding_setupFromData_hook, &FurnaceBuilding_setupFromData_orig))
			ErrorLog("[FurnaceBuilding::setupFromData] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&FurnaceBuilding::_NV_getInputValue), &FurnaceBuilding_getInputValue_hook, &FurnaceBuilding_getInputValue_orig))
			ErrorLog("[FurnaceBuilding::getInputValue] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&FurnaceBuilding::_NV_getResourcesNeededBecauseEmpty), &FurnaceBuilding_getResourcesNeededBecauseEmpty_hook, &FurnaceBuilding_getResourcesNeededBecauseEmpty_orig))
			ErrorLog("[FurnaceBuilding::getResourcesNeededBecauseEmpty] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_005A3B60, &AI_haveSomeResourcesFor_hook, &AI_haveSomeResourcesFor_orig))
			ErrorLog("[FUN_005A3B60] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_00340EB0, &Task_FillMachine_FUN_00340EB0_hook, &Task_FillMachine_FUN_00340EB0_orig))
			ErrorLog("[FUN_00340EB0] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_00343720, &Task_FillMachine_FUN_00343720_hook, &Task_FillMachine_FUN_00343720_orig))
			ErrorLog("[FUN_00343720] could not install hook!");
	}
}
