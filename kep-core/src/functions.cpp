/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include "pch.h"
#include <kep/functions.h>

boost::scoped_ptr<KEP::FunctionPointers> KEP::functions(new KEP::FunctionPointers());

KEP::FunctionPointers::FunctionPointers()
	: EscMenu_openedOtherWindows(nullptr)
	, Research_completed(nullptr)
	, Research_complete(nullptr)
	, Research_createBlueprint(nullptr)
	, Research_addResearchProgress(nullptr)
	, getPrice(nullptr)
	, MessageRoller_clear(nullptr)
	, UniqueNPCManager_getSingleton(nullptr)
	, UniqueNPCManager_remove(nullptr)
	, AreasList_getAreaSector(nullptr)
	, BuildingInteriorDatasManager_hasLayout(nullptr)
	, CharacterMemory_getSubjectiveTags(nullptr)
	, VendorListManager_hasVendor(nullptr)
	, InventoryManager_getSingleton(nullptr)
	, InventoryManager_refreshBuildingInventory(nullptr)
	, InventoryManager_getBuildingInventoryList(nullptr)
	, InventoryManager_clearAllBuildingInventory(nullptr)
	, BuildingInterior_destroyAllInternalBuildings(nullptr)
	, NodeList_destroyNodesByBuilding(nullptr)
	, CharacterMemory_getGUIData(nullptr)
	, Research_init(nullptr)
	, setTotalCoverage(nullptr)
	, loadPartmap(nullptr)
	, Harpoon_destroy(nullptr)
	, HarpoonManager_create(nullptr)
	, CharacterMemory_getCharacterMemoryTag(nullptr)
	, Harpoon_trace(nullptr)
	, convertRarityToLevel(nullptr)
	, crossbowLevelSelector(nullptr)
	, calculateStatsMult(nullptr)
	, InventoryManager_refreshSquadInventory(nullptr)
	, timer(nullptr)
	, NULL_HAND(nullptr)
	, _interior(nullptr)
	, _DialogDataManager_allDatas(nullptr)
	, _GameplayOptions(nullptr)
	, _attacks(nullptr)
	, _blocks(nullptr)
	, _harpoon(nullptr)
	, _shapeBuffer(nullptr)
	, _handleMgr(nullptr)
	, _TaskDatas(nullptr)
{
}

void KEP::FunctionPointers::init(unsigned int platform, const std::string& version, uintptr_t baseAddr)
{
	if (platform == 1)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&EscMenu_openedOtherWindows = baseAddr + 0x916250;
			*(uintptr_t*)&Research_completed = baseAddr + 0x82e430;
			*(uintptr_t*)&Research_complete = baseAddr + 0x833680;
			*(uintptr_t*)&Research_createBlueprint = baseAddr + 0x831620;
			*(uintptr_t*)&Research_addResearchProgress = baseAddr + 0x835ca0;
			*(uintptr_t*)&getPrice = baseAddr + 0x895f50;
			*(uintptr_t*)&MessageRoller_clear = baseAddr + 0x7239a0;
			*(uintptr_t*)&UniqueNPCManager_getSingleton = baseAddr + 0x354500;
			*(uintptr_t*)&UniqueNPCManager_remove = baseAddr + 0x505d50;
			*(uintptr_t*)&AreasList_getAreaSector = baseAddr + 0x8f47e0;
			*(uintptr_t*)&BuildingInteriorDatasManager_hasLayout = baseAddr + 0x55a2b0;
			*(uintptr_t*)&CharacterMemory_getSubjectiveTags = baseAddr + 0x673000;
			*(uintptr_t*)&VendorListManager_hasVendor = baseAddr + 0x958550;
			*(uintptr_t*)&InventoryManager_getSingleton = baseAddr + 0x4fde00;
			*(uintptr_t*)&InventoryManager_refreshBuildingInventory = baseAddr + 0x95a340;
			*(uintptr_t*)&InventoryManager_getBuildingInventoryList = baseAddr + 0x955420;
			*(uintptr_t*)&InventoryManager_clearAllBuildingInventory = baseAddr + 0x955600;
			*(uintptr_t*)&BuildingInterior_destroyAllInternalBuildings = baseAddr + 0x54ea40;
			*(uintptr_t*)&NodeList_destroyNodesByBuilding = baseAddr + 0x38a5e0;
			*(uintptr_t*)&CharacterMemory_getGUIData = baseAddr + 0x674bf0;
			*(uintptr_t*)&Research_init = baseAddr + 0x833ca0;
			*(uintptr_t*)&setTotalCoverage = baseAddr + 0x82eeb0;
			*(uintptr_t*)&loadPartmap = baseAddr + 0x755c0;
			*(uintptr_t*)&Harpoon_destroy = baseAddr + 0x435610;
			*(uintptr_t*)&HarpoonManager_create = baseAddr + 0x439f50;
			*(uintptr_t*)&CharacterMemory_getCharacterMemoryTag = baseAddr + 0x677da0;
			*(uintptr_t*)&Harpoon_trace = baseAddr + 0x4354d0;
			*(uintptr_t*)&convertRarityToLevel = baseAddr + 0x620660;
			*(uintptr_t*)&crossbowLevelSelector = baseAddr + 0x954340;
			*(uintptr_t*)&calculateStatsMult = baseAddr + 0xcd4c0;
			*(uintptr_t*)&InventoryManager_refreshSquadInventory = baseAddr + 0x959c40;
			*(uintptr_t*)&timer = baseAddr + 0x2132730;
			*(uintptr_t*)&NULL_HAND = baseAddr + 0x1e395f8;
			*(uintptr_t*)&_interior = baseAddr + 0x212ec50;
			*(uintptr_t*)&_DialogDataManager_allDatas = baseAddr + 0x1ef7c50;
			*(uintptr_t*)&_GameplayOptions = baseAddr + 0x2132528;
			*(uintptr_t*)&_attacks = baseAddr + 0x2010f68;
			*(uintptr_t*)&_blocks = baseAddr + 0x2010f80;
			*(uintptr_t*)&_harpoon = baseAddr + 0x212e1d0;
			*(uintptr_t*)&_shapeBuffer = baseAddr + 0x212de98;
			*(uintptr_t*)&_handleMgr = baseAddr + 0x2132f30;
			*(uintptr_t*)&_TaskDatas = baseAddr + 0x1ce80f0;
		}
	}
	else if (platform == 0)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&EscMenu_openedOtherWindows = baseAddr + 0x915970;
			*(uintptr_t*)&Research_completed = baseAddr + 0x82dd70;
			*(uintptr_t*)&Research_complete = baseAddr + 0x832fc0;
			*(uintptr_t*)&Research_createBlueprint = baseAddr + 0x830f60;
			*(uintptr_t*)&Research_addResearchProgress = baseAddr + 0x8355e0;
			*(uintptr_t*)&getPrice = baseAddr + 0x895670;
			*(uintptr_t*)&MessageRoller_clear = baseAddr + 0x723300;
			*(uintptr_t*)&UniqueNPCManager_getSingleton = baseAddr + 0x3540b0;
			*(uintptr_t*)&UniqueNPCManager_remove = baseAddr + 0x506060;
			*(uintptr_t*)&AreasList_getAreaSector = baseAddr + 0x8f3f00;
			*(uintptr_t*)&BuildingInteriorDatasManager_hasLayout = baseAddr + 0x55a5c0;
			*(uintptr_t*)&CharacterMemory_getSubjectiveTags = baseAddr + 0x673310;
			*(uintptr_t*)&VendorListManager_hasVendor = baseAddr + 0x957c70;
			*(uintptr_t*)&InventoryManager_getSingleton = baseAddr + 0x4fe110;
			*(uintptr_t*)&InventoryManager_refreshBuildingInventory = baseAddr + 0x959a60;
			*(uintptr_t*)&InventoryManager_getBuildingInventoryList = baseAddr + 0x954b40;
			*(uintptr_t*)&InventoryManager_clearAllBuildingInventory = baseAddr + 0x954d20;
			*(uintptr_t*)&BuildingInterior_destroyAllInternalBuildings = baseAddr + 0x54ed50;
			*(uintptr_t*)&NodeList_destroyNodesByBuilding = baseAddr + 0x38a200;
			*(uintptr_t*)&CharacterMemory_getGUIData = baseAddr + 0x674f00;
			*(uintptr_t*)&Research_init = baseAddr + 0x8335e0;
			*(uintptr_t*)&setTotalCoverage = baseAddr + 0x82e7f0;
			*(uintptr_t*)&loadPartmap = baseAddr + 0x755c0;
			*(uintptr_t*)&Harpoon_destroy = baseAddr + 0x435230;
			*(uintptr_t*)&HarpoonManager_create = baseAddr + 0x439b70;
			*(uintptr_t*)&CharacterMemory_getCharacterMemoryTag = baseAddr + 0x6780b0;
			*(uintptr_t*)&Harpoon_trace = baseAddr + 0x4350f0;
			*(uintptr_t*)&crossbowLevelSelector = baseAddr + 0x953a60;
			*(uintptr_t*)&convertRarityToLevel = baseAddr + 0x620970;
			*(uintptr_t*)&calculateStatsMult = baseAddr + 0xcd4c0;
			*(uintptr_t*)&InventoryManager_refreshSquadInventory = baseAddr + 0x959360;
			*(uintptr_t*)&timer = baseAddr + 0x21306a0;
			*(uintptr_t*)&NULL_HAND = baseAddr + 0x1e375f8;
			*(uintptr_t*)&_interior = baseAddr + 0x212cb90;
			*(uintptr_t*)&_DialogDataManager_allDatas = baseAddr + 0x1ef5c50;
			*(uintptr_t*)&_GameplayOptions = baseAddr + 0x2130498;
			*(uintptr_t*)&_attacks = baseAddr + 0x200ef78;
			*(uintptr_t*)&_blocks = baseAddr + 0x200ef90;
			*(uintptr_t*)&_harpoon = baseAddr + 0x212c110;
			*(uintptr_t*)&_shapeBuffer = baseAddr + 0x212bdd8;
			*(uintptr_t*)&_handleMgr = baseAddr + 0x2130ea0;
			*(uintptr_t*)&_TaskDatas = baseAddr + 0x1ce60f0;
		}
	}
}

float KEP::FunctionPointers::getTime() const { return *timer; }
const hand& KEP::FunctionPointers::getNULL_HAND() const { return *NULL_HAND; }
BuildingInteriorDatasManager* KEP::FunctionPointers::getInteriorMgr() const { return _interior; }
ogre_unordered_map<GameData*, DialogLineData*>::type& KEP::FunctionPointers::getDialogDataManager_allDatas() const { return *_DialogDataManager_allDatas; }
GameplayOptions* KEP::FunctionPointers::getGameplayOptions() const { return _GameplayOptions; }
lektor<CombatTechniqueData*>& KEP::FunctionPointers::getAttacks() const { return *_attacks; }
lektor<CombatTechniqueData*>& KEP::FunctionPointers::getBlocks() const { return *_blocks; }
HarpoonManager* KEP::FunctionPointers::getHarpoonManager() const { return _harpoon; }
NxShape**& KEP::FunctionPointers::getShapeBuffer() const { return *_shapeBuffer; }
HandleManager& KEP::FunctionPointers::getHandleManager() const { return *_handleMgr; }
ogre_unordered_map<TaskType, TaskData*>::type& KEP::FunctionPointers::getTaskDatas() const { return *_TaskDatas; }
