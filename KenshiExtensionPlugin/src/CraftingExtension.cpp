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
#include <extern/Research.h>

#include <UtilityFunction.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <CraftingExtension.h>


namespace
{
	GameData* (*FUN_002BB270_orig)(CraftingBuilding*, int);
	void (*FUN_002BBCE0_orig)(CraftingBuilding*, lektor<GameDataGroup>&);

	bool compare(const GameDataGroup& a, const GameDataGroup& b)
	{
		if (a.data1->createdIndex != b.data1->createdIndex)
			return a.data1->createdIndex < b.data1->createdIndex;
		return a.data1->stringID < b.data1->stringID;
	}
}

GameData* KEP::CraftingExtension::FUN_002BB270_hook(CraftingBuilding* self , int level)
{
	static GameData* defaultWeaponModel = ou->gamedata.getData("913-gamedata.base", MATERIAL_SPECS_WEAPON);

	auto playerWeapons = externalFunctions->FUN_002CE000();
	auto& playerWeaponModels = playerWeapons->objectReferences["weapon models"];

	int criticalSuccessLvel = 99999;
	GameData* criticalSuccessWeaponModel = nullptr;
	GameData* sameLevelWeaponModel = nullptr;

	auto unlimited = !self->functionality->bdata["limit weapon models"];

	for (auto iter = playerWeaponModels.begin(); iter != playerWeaponModels.end(); ++iter)
	{
		if (unlimited || self->functionality->findInList("weapon crafts", iter->sid))
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

void KEP::CraftingExtension::FUN_002BBCE0_hook(CraftingBuilding* self, lektor<GameDataGroup>& list)
{
	lektor<GameData*> weaponCrafts;
	self->functionality->getAllFromListAsDatas("weapon crafts", weaponCrafts, &ou->gamedata, MATERIAL_SPECS_WEAPON);

	if (weaponCrafts.size() != 0)
	{
		lektor<GameDataGroup> enableWeaponList;
		lektor<WeaponTypeStruct> craftWeaponList;

		externalFunctions->FUN_0082E2E0(ou->player->technology, enableWeaponList, MATERIAL_SPECS_WEAPON, NULL_ITEM);

		uint32_t count = 0;
		for (auto enableWeaponIter = enableWeaponList.begin(); enableWeaponIter != enableWeaponList.end(); ++enableWeaponIter)
		{
			for (auto iter = weaponCrafts.begin(); iter != weaponCrafts.end(); ++iter)
			{
				if (*iter == enableWeaponIter->data1)
				{
					float lvl = externalFunctions->FUN_002B8260(self, enableWeaponIter->data1);
					bool fixed = enableWeaponIter->data1->bdata["craft list fixed"];
					if (count == 0 || lvl < craftWeaponList[count-1].level)
					{
						craftWeaponList.push_back(WeaponTypeStruct(lvl, fixed, enableWeaponIter->data1));
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

						craftWeaponList.insert(WeaponTypeStruct(lvl, fixed, enableWeaponIter->data1), insertIndex);
						count = craftWeaponList.size();
					}

					break;
				}
			}
		}

		enableWeaponList.clear();

		auto refList = self->functionality->getReferenceList("weapon types");
		if (refList != nullptr && refList->size() != 0)
		{
			externalFunctions->FUN_002CA350(refList, WEAPON, enableWeaponList);

			for (auto enableWeaponIter = enableWeaponList.begin(); enableWeaponIter != enableWeaponList.end(); ++enableWeaponIter)
			{
				for (uint32_t i = 0; i < craftWeaponList.size(); ++i)
				{
					if (i == 0 || craftWeaponList[i].craftListFixed)
						list.push_back(GameDataGroup(enableWeaponIter->data1, craftWeaponList[i].data));
				}
			}
		}
		else
		{
			externalFunctions->FUN_0082E2E0(ou->player->technology, enableWeaponList, WEAPON, NULL_ITEM);
			std::sort(enableWeaponList.begin(), enableWeaponList.end(), compare);

			for (auto enableWeaponIter = enableWeaponList.begin(); enableWeaponIter != enableWeaponList.end(); ++enableWeaponIter)
			{
				if (enableWeaponIter->data1->bdata["custom model"])
					continue;

				for (uint32_t i = 0; i < craftWeaponList.size(); ++i)
				{
					if (i == 0 || craftWeaponList[i].craftListFixed)
						list.push_back(GameDataGroup(enableWeaponIter->data1, craftWeaponList[i].data));
				}
			}
		}
	}

	externalFunctions->FUN_002CA350(self->functionality->getReferenceList("item crafts"), ITEM, list);
	externalFunctions->FUN_002CA350(self->functionality->getReferenceList("armour crafts"), ARMOUR, list);
	externalFunctions->FUN_002CA350(self->functionality->getReferenceList("crossbow crafts"), CROSSBOW, list);
	externalFunctions->FUN_002CA350(self->functionality->getReferenceList("backpack crafts"), CONTAINER, list);
	externalFunctions->FUN_002CA350(self->functionality->getReferenceList("robotics crafts"), LIMB_REPLACEMENT, list);
}

void KEP::CraftingExtension::init()
{
	if (settings._craftingItemExtension)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_002BB270, &FUN_002BB270_hook, &FUN_002BB270_orig))
			ErrorLog("KenshiExtensionPlugin: [weapon craft pt1] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_002BBCE0, &FUN_002BBCE0_hook, &FUN_002BBCE0_orig))
			ErrorLog("KenshiExtensionPlugin: [weapon craft pt2] could not install hook!");
	}
}
