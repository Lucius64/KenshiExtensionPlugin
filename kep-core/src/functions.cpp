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
	, TownList_getTown(nullptr)
	, TownList_getTownWithPlayer(nullptr)
	, StateBroadcastData_setSlaveState(nullptr)
	, OrdersReceiver_deleteAllTask(nullptr)
	, Blackboard_changeAI(nullptr)
	, Blackboard_setSquadPackage(nullptr)
	, Blackboard_deleteAllPackage(nullptr)
	, Blackboard_setFallbackPackage(nullptr)
	, OrdersReceiver_deleteAllPassiveJob(nullptr)
	, ActionDeque_clear(nullptr)
	, AITaskSystem_clear(nullptr)
	, TownList_getNest(nullptr)
	, TownList_getTownBase(nullptr)
	, AreaManager_getAreaSector(nullptr)
	, BuildingInteriorDatasManager_hasLayout(nullptr)
	, CharacterMemory_getSubjectiveTags(nullptr)
	, VendorListManager_hasVendor(nullptr)
	, InventoryManager_getSingleton(nullptr)
	, InventoryManager_refreshBuildingInventory(nullptr)
	, InventoryManager_getBuildingInventoryList(nullptr)
	, InventoryManager_clearAllBuildingInventory(nullptr)
	, timer(nullptr)
	, _levelMgr(nullptr)
	, NULL_HAND(nullptr)
	, _interior(nullptr)
	, _DialogDataManager_allDatas(nullptr)
	, _GameplayOptions(nullptr)
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
			*(uintptr_t*)&TownList_getTown = baseAddr + 0x927040;
			*(uintptr_t*)&TownList_getTownWithPlayer = baseAddr + 0x926f70;
			*(uintptr_t*)&StateBroadcastData_setSlaveState = baseAddr + 0x5a3eb0;
			*(uintptr_t*)&OrdersReceiver_deleteAllTask = baseAddr + 0x506a10;
			*(uintptr_t*)&Blackboard_changeAI = baseAddr + 0x2723f0;
			*(uintptr_t*)&Blackboard_setSquadPackage = baseAddr + 0x2715e0;
			*(uintptr_t*)&Blackboard_deleteAllPackage = baseAddr + 0x26bba0;
			*(uintptr_t*)&Blackboard_setFallbackPackage = baseAddr + 0x271f90;
			*(uintptr_t*)&OrdersReceiver_deleteAllPassiveJob = baseAddr + 0x5067f0;
			*(uintptr_t*)&ActionDeque_clear = baseAddr + 0x50c720;
			*(uintptr_t*)&AITaskSystem_clear = baseAddr + 0x4ab930;
			*(uintptr_t*)&TownList_getNest = baseAddr + 0x926c00;
			*(uintptr_t*)&TownList_getTownBase = baseAddr + 0x926ce0;
			*(uintptr_t*)&AreaManager_getAreaSector = baseAddr + 0x8f47e0;
			*(uintptr_t*)&BuildingInteriorDatasManager_hasLayout = baseAddr + 0x55a2b0;
			*(uintptr_t*)&CharacterMemory_getSubjectiveTags = baseAddr + 0x0;
			*(uintptr_t*)&VendorListManager_hasVendor = baseAddr + 0x958550;
			*(uintptr_t*)&InventoryManager_getSingleton = baseAddr + 0x4fde00;
			*(uintptr_t*)&InventoryManager_refreshBuildingInventory = baseAddr + 0x95a340;
			*(uintptr_t*)&InventoryManager_getBuildingInventoryList = baseAddr + 0x955420;
			*(uintptr_t*)&InventoryManager_clearAllBuildingInventory = baseAddr + 0x955600;
			*(uintptr_t*)&timer = baseAddr + 0x2132730;
			*(uintptr_t*)&_levelMgr = baseAddr + 0x2133098;
			*(uintptr_t*)&NULL_HAND = baseAddr + 0x1e395f8;
			*(uintptr_t*)&_interior = baseAddr + 0x212ec50;
			*(uintptr_t*)&_DialogDataManager_allDatas = baseAddr + 0x1ef7c50;
			*(uintptr_t*)&_GameplayOptions = baseAddr + 0x2132528;
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
			*(uintptr_t*)&TownList_getTown = baseAddr + 0x926760;
			*(uintptr_t*)&TownList_getTownWithPlayer = baseAddr + 0x926690;
			*(uintptr_t*)&StateBroadcastData_setSlaveState = baseAddr + 0x5a41c0;
			*(uintptr_t*)&OrdersReceiver_deleteAllTask = baseAddr + 0x506d20;
			*(uintptr_t*)&Blackboard_changeAI = baseAddr + 0x271f80;
			*(uintptr_t*)&Blackboard_setSquadPackage = baseAddr + 0x271170;
			*(uintptr_t*)&Blackboard_deleteAllPackage = baseAddr + 0x26b730;
			*(uintptr_t*)&Blackboard_setFallbackPackage = baseAddr + 0x271b20;
			*(uintptr_t*)&OrdersReceiver_deleteAllPassiveJob = baseAddr + 0x506b00;
			*(uintptr_t*)&ActionDeque_clear = baseAddr + 0x50ca30;
			*(uintptr_t*)&AITaskSystem_clear = baseAddr + 0x4abc40;
			*(uintptr_t*)&TownList_getNest = baseAddr + 0x926320;
			*(uintptr_t*)&TownList_getTownBase = baseAddr + 0x926400;
			*(uintptr_t*)&AreaManager_getAreaSector = baseAddr + 0x8f3f00;
			*(uintptr_t*)&BuildingInteriorDatasManager_hasLayout = baseAddr + 0x55a5c0;
			*(uintptr_t*)&CharacterMemory_getSubjectiveTags = baseAddr + 0x0;
			*(uintptr_t*)&VendorListManager_hasVendor = baseAddr + 0x957c70;
			*(uintptr_t*)&InventoryManager_getSingleton = baseAddr + 0x4fe110;
			*(uintptr_t*)&InventoryManager_refreshBuildingInventory = baseAddr + 0x959a60;
			*(uintptr_t*)&InventoryManager_getBuildingInventoryList = baseAddr + 0x954b40;
			*(uintptr_t*)&InventoryManager_clearAllBuildingInventory = baseAddr + 0x954d20;
			*(uintptr_t*)&timer = baseAddr + 0x21306a0;
			*(uintptr_t*)&_levelMgr = baseAddr + 0x2131008;
			*(uintptr_t*)&NULL_HAND = baseAddr + 0x1e375f8;
			*(uintptr_t*)&_interior = baseAddr + 0x212cb90;
			*(uintptr_t*)&_DialogDataManager_allDatas = baseAddr + 0x1ef5c50;
			*(uintptr_t*)&_GameplayOptions = baseAddr + 0x2130498;
		}
	}
}

float KEP::FunctionPointers::getTime() const { return *timer; }
LevelManager* KEP::FunctionPointers::getLevelManager() const { return _levelMgr; }
const hand& KEP::FunctionPointers::getNULL_HAND() const { return *NULL_HAND; }
BuildingInteriorDatasManager* KEP::FunctionPointers::getInteriorMgr() const { return _interior; }
ogre_unordered_map<GameData*, DialogLineData*>::type& KEP::FunctionPointers::getDialogDataManager_allDatas() const { return *_DialogDataManager_allDatas; }
GameplayOptions* KEP::FunctionPointers::getGameplayOptions() const { return _GameplayOptions; }
