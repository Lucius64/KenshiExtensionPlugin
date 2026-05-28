#include <boost/locale.hpp>

#include <ogre/OgrePrerequisites.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GlobalConstants.h>
#include <kenshi/GameData.h>
#include <kenshi/RootObjectFactory.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/CharStats.h>
#include <kenshi/Character.h>

#include <extern/ProductionBuilding.h>
#include <kenshi/Building/CraftingBuilding.h>
#include <extern/Research.h>

#include <kep/translation.h>
#include <UtilityFunction.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <CraftingExtension.h>


namespace
{
	struct GameDataGroupLess
	{
		bool operator()(const GameDataGroup& a, const GameDataGroup& b) const
		{
			int result = KEP::TranslationUtility::getCollator().compare(boost::locale::collator_base::secondary, a.g1->name, b.g1->name);
			if (result == 0)
			{
				if (a.g1->createdIndex == b.g1->createdIndex)
					return a.g1->stringID < b.g1->stringID;

				return a.g1->createdIndex < b.g1->createdIndex;
			}
			return result < 0;
		}
	};

	GameData* (*CraftingBuilding_getCriticalSuccessWeapon_orig)(CraftingBuilding*, int);
	GameData* CraftingBuilding_getCriticalSuccessWeapon_hook(CraftingBuilding* self, int level)
	{
		static GameData* defaultWeaponModel = ou->gamedata.getData("913-gamedata.base", MATERIAL_SPECS_WEAPON);

		auto playerWeapons = CraftingBuilding::playerManufacturerData();
		auto& playerWeaponModels = playerWeapons->objectReferences["weapon models"];

		int criticalSuccessLvel = 99999;
		GameData* criticalSuccessWeaponModel = nullptr;
		GameData* sameLevelWeaponModel = nullptr;

		auto unlimited = !self->functionalityData->bdata["limit weapon models"];

		for (auto iter = playerWeaponModels.begin(); iter != playerWeaponModels.end(); ++iter)
		{
			if (unlimited || self->functionalityData->findInList("weapon crafts", iter->sid))
			{
				int lvl = iter->values.value[0];
				if (level < lvl && lvl < criticalSuccessLvel)
				{
					criticalSuccessWeaponModel = iter->getPtr(&ou->gamedata);
					criticalSuccessLvel = lvl;
				}
				else if (level == lvl)
				{
					sameLevelWeaponModel = iter->getPtr(&ou->gamedata);
				}
			}
		}

		if (criticalSuccessWeaponModel == nullptr)
			criticalSuccessWeaponModel = sameLevelWeaponModel != nullptr ? sameLevelWeaponModel : defaultWeaponModel;

		return criticalSuccessWeaponModel;
	}

	void (*CraftingBuilding_getAvailableCrafts_orig)(CraftingBuilding*, lektor<GameDataGroup>&);
	void CraftingBuilding_getAvailableCrafts_hook(CraftingBuilding* self, lektor<GameDataGroup>& list)
	{
		lektor<GameData*> weaponCrafts;
		self->functionalityData->getAllFromListAsDatas("weapon crafts", weaponCrafts, &ou->gamedata, MATERIAL_SPECS_WEAPON);

		if (weaponCrafts.size() != 0)
		{
			lektor<GameDataGroup> enableWeaponList;
			lektor<WeaponTypeStruct> craftWeaponList;

			KEP::externalFunctions->FUN_0082E2E0(ou->player->technology, enableWeaponList, MATERIAL_SPECS_WEAPON, NULL_ITEM);

			uint32_t count = 0;
			for (auto enableWeaponIter = enableWeaponList.begin(); enableWeaponIter != enableWeaponList.end(); ++enableWeaponIter)
			{
				for (auto iter = weaponCrafts.begin(); iter != weaponCrafts.end(); ++iter)
				{
					if (*iter == enableWeaponIter->g1)
					{
						float lvl = self->getPlayerCraftedWeaponLevel(enableWeaponIter->g1);
						bool fixed = enableWeaponIter->g1->bdata["craft list fixed"];
						if (count == 0 || lvl < craftWeaponList[count - 1].level)
						{
							craftWeaponList.push_back(WeaponTypeStruct(lvl, fixed, enableWeaponIter->g1));
							count = craftWeaponList.size();
						}
						else
						{
							uint32_t insertIndex = 0;
							for (; insertIndex < craftWeaponList.size(); ++insertIndex)
							{
								if (lvl > craftWeaponList[insertIndex].level)
									break;
							}
							if (craftWeaponList.size() <= insertIndex)
								continue;

							craftWeaponList.insert(WeaponTypeStruct(lvl, fixed, enableWeaponIter->g1), insertIndex);
							count = craftWeaponList.size();
						}

						break;
					}
				}
			}

			enableWeaponList.clear();

			auto refList = self->functionalityData->getReferenceListIfExists("weapon types");
			if (refList != nullptr && refList->size() != 0)
			{
				KEP::externalFunctions->FUN_002CA350(refList, WEAPON, enableWeaponList);

				for (auto enableWeaponIter = enableWeaponList.begin(); enableWeaponIter != enableWeaponList.end(); ++enableWeaponIter)
				{
					for (uint32_t i = 0; i < craftWeaponList.size(); ++i)
					{
						if (i == 0 || craftWeaponList[i].craftListFixed)
						{
							GameDataGroup group;
							group.g1 = enableWeaponIter->g1;
							group.g2 = craftWeaponList[i].data;
							list.push_back(group);
						}
					}
				}
			}
			else
			{
				KEP::externalFunctions->FUN_0082E2E0(ou->player->technology, enableWeaponList, WEAPON, NULL_ITEM);
				std::sort(enableWeaponList.begin(), enableWeaponList.end(), GameDataGroupLess());

				for (auto enableWeaponIter = enableWeaponList.begin(); enableWeaponIter != enableWeaponList.end(); ++enableWeaponIter)
				{
					if (enableWeaponIter->g1->bdata["custom model"])
						continue;

					for (uint32_t i = 0; i < craftWeaponList.size(); ++i)
					{
						if (i == 0 || craftWeaponList[i].craftListFixed)
						{
							GameDataGroup group;
							group.g1 = enableWeaponIter->g1;
							group.g2 = craftWeaponList[i].data;
							list.push_back(group);
						}
					}
				}
			}
		}

		KEP::externalFunctions->FUN_002CA350(self->functionalityData->getReferenceListIfExists("item crafts"), ITEM, list);
		KEP::externalFunctions->FUN_002CA350(self->functionalityData->getReferenceListIfExists("armour crafts"), ARMOUR, list);
		KEP::externalFunctions->FUN_002CA350(self->functionalityData->getReferenceListIfExists("crossbow crafts"), CROSSBOW, list);
		KEP::externalFunctions->FUN_002CA350(self->functionalityData->getReferenceListIfExists("backpack crafts"), CONTAINER, list);
		KEP::externalFunctions->FUN_002CA350(self->functionalityData->getReferenceListIfExists("robotics crafts"), LIMB_REPLACEMENT, list);
	}

	void (*FUN_002CA350_orig)(const Ogre::vector<GameDataReference>::type*, itemType, lektor<GameDataGroup>&);
	void FUN_002CA350_hook(const Ogre::vector<GameDataReference>::type* craftingItems, itemType type, lektor<GameDataGroup>& list)
	{
		if (craftingItems == nullptr)
			return;

		lektor<GameDataGroup> enableItemList;
		KEP::externalFunctions->FUN_0082E2E0(ou->player->technology, enableItemList, type, NULL_ITEM);
		std::sort(enableItemList.begin(), enableItemList.end(), GameDataGroupLess());

		for (auto enableIter = enableItemList.begin(); enableIter != enableItemList.end(); ++enableIter)
		{
			for (auto iter = craftingItems->begin(); iter != craftingItems->end(); ++iter)
			{
				if (iter->sid == enableIter->g1->stringID)
				{
					list.push_back(*enableIter);
					break;
				}
			}
		}
	}
}

void KEP::CraftingExtension::init()
{
	if (settings._craftingItemExtension)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CraftingBuilding::getCriticalSuccessWeapon), &CraftingBuilding_getCriticalSuccessWeapon_hook, &CraftingBuilding_getCriticalSuccessWeapon_orig))
			ErrorLog("[CraftingBuilding::getCriticalSuccessWeapon] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CraftingBuilding::getAvailableCrafts), &CraftingBuilding_getAvailableCrafts_hook, &CraftingBuilding_getAvailableCrafts_orig))
			ErrorLog("[CraftingBuilding::getAvailableCrafts] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_002CA350, &FUN_002CA350_hook, &FUN_002CA350_orig))
			ErrorLog("[FUN_002CA350] could not install hook!");
	}
}
