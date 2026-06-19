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
	, Blackboard_changeAI(nullptr)
	, Blackboard_setSquadPackage(nullptr)
	, Blackboard_deleteAllPackage(nullptr)
	, Blackboard_setFallbackPackage(nullptr)
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
	, Blackboard_getGUIData(nullptr)
	, CharacterMemory_getGUIData(nullptr)
	, Research_init(nullptr)
	, setTotalCoverage(nullptr)
	, loadPartmap(nullptr)
	, timer(nullptr)
	, NULL_HAND(nullptr)
	, _interior(nullptr)
	, _DialogDataManager_allDatas(nullptr)
	, _GameplayOptions(nullptr)
	, _attacks(nullptr)
	, _blocks(nullptr)
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
			*(uintptr_t*)&Blackboard_changeAI = baseAddr + 0x2723f0;
			*(uintptr_t*)&Blackboard_setSquadPackage = baseAddr + 0x2715e0;
			*(uintptr_t*)&Blackboard_deleteAllPackage = baseAddr + 0x26bba0;
			*(uintptr_t*)&Blackboard_setFallbackPackage = baseAddr + 0x271f90;
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
			*(uintptr_t*)&Blackboard_getGUIData = baseAddr + 0x26a4e0;
			*(uintptr_t*)&CharacterMemory_getGUIData = baseAddr + 0x674bf0;
			*(uintptr_t*)&Research_init = baseAddr + 0x833ca0;
			*(uintptr_t*)&setTotalCoverage = baseAddr + 0x82eeb0;
			*(uintptr_t*)&loadPartmap = baseAddr + 0x755c0;
			*(uintptr_t*)&timer = baseAddr + 0x2132730;
			*(uintptr_t*)&NULL_HAND = baseAddr + 0x1e395f8;
			*(uintptr_t*)&_interior = baseAddr + 0x212ec50;
			*(uintptr_t*)&_DialogDataManager_allDatas = baseAddr + 0x1ef7c50;
			*(uintptr_t*)&_GameplayOptions = baseAddr + 0x2132528;
			*(uintptr_t*)&_attacks = baseAddr + 0x2010f68;
			*(uintptr_t*)&_blocks = baseAddr + 0x2010f80;
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
			*(uintptr_t*)&Blackboard_changeAI = baseAddr + 0x271f80;
			*(uintptr_t*)&Blackboard_setSquadPackage = baseAddr + 0x271170;
			*(uintptr_t*)&Blackboard_deleteAllPackage = baseAddr + 0x26b730;
			*(uintptr_t*)&Blackboard_setFallbackPackage = baseAddr + 0x271b20;
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
			*(uintptr_t*)&Blackboard_getGUIData = baseAddr + 0x26a070;
			*(uintptr_t*)&CharacterMemory_getGUIData = baseAddr + 0x674f00;
			*(uintptr_t*)&Research_init = baseAddr + 0x8335e0;
			*(uintptr_t*)&setTotalCoverage = baseAddr + 0x82e7f0;
			*(uintptr_t*)&loadPartmap = baseAddr + 0x755c0;
			*(uintptr_t*)&timer = baseAddr + 0x21306a0;
			*(uintptr_t*)&NULL_HAND = baseAddr + 0x1e375f8;
			*(uintptr_t*)&_interior = baseAddr + 0x212cb90;
			*(uintptr_t*)&_DialogDataManager_allDatas = baseAddr + 0x1ef5c50;
			*(uintptr_t*)&_GameplayOptions = baseAddr + 0x2130498;
			*(uintptr_t*)&_attacks = baseAddr + 0x200ef78;
			*(uintptr_t*)&_blocks = baseAddr + 0x200ef90;
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
