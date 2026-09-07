/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#pragma once
#include <cstdint>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <ogre/OgrePrerequisites.h>
#include <kenshi/util/OgreUnordered.h>

template<typename T> class lektor;
template<typename T> class FitnessSelector;

class FunctionPointers
{
public:
	FunctionPointers()
		: EscMenu_openedOtherWindows(nullptr)
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
		, AreasList_getBiome(nullptr)
		, Task_FirstAid_updateItem(nullptr)
		, Task_FirstAid_removeItemAutoDestroy(nullptr)
		, Task_FirstAid_applyFirstAid(nullptr)
		, Task_FirstAid_runAction(nullptr)
		, Task_FirstAidRig_runAction(nullptr)
		, Task_MeleeAttack_startAction(nullptr)
		, Task_FocusedMeleeAttack_startAction(nullptr)
		, Task_RangedAttack_startAction(nullptr)
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

	void init(unsigned int platform, const std::string& version, uintptr_t baseAddr)
	{
		if (platform == 1)
		{
			if (version == "1.0.65")
			{
				*(uintptr_t*)&EscMenu_openedOtherWindows = baseAddr + 0x916250;
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
				*(uintptr_t*)&AreasList_getBiome = baseAddr + 0x8f48a0;
				*(uintptr_t*)&Task_FirstAid_updateItem = baseAddr + 0x33c780;
				*(uintptr_t*)&Task_FirstAid_removeItemAutoDestroy = baseAddr + 0x32dfb0;
				*(uintptr_t*)&Task_FirstAid_applyFirstAid = baseAddr + 0x33c540;
				*(uintptr_t*)&Task_FirstAid_runAction = baseAddr + 0x356340;
				*(uintptr_t*)&Task_FirstAidRig_runAction = baseAddr + 0x34eec0;
				*(uintptr_t*)&Task_MeleeAttack_startAction = baseAddr + 0x3341b0;
				*(uintptr_t*)&Task_FocusedMeleeAttack_startAction = baseAddr + 0x33c180;
				*(uintptr_t*)&Task_RangedAttack_startAction = baseAddr + 0x33bf30;
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
				*(uintptr_t*)&AreasList_getBiome = baseAddr + 0x8f3fc0;
				*(uintptr_t*)&Task_FirstAid_updateItem = baseAddr + 0x33c330;
				*(uintptr_t*)&Task_FirstAid_removeItemAutoDestroy = baseAddr + 0x32db40;
				*(uintptr_t*)&Task_FirstAid_applyFirstAid = baseAddr + 0x33c0f0;
				*(uintptr_t*)&Task_FirstAid_runAction = baseAddr + 0x355ef0;
				*(uintptr_t*)&Task_FirstAidRig_runAction = baseAddr + 0x34ea70;
				*(uintptr_t*)&Task_MeleeAttack_startAction = baseAddr + 0x333d40;
				*(uintptr_t*)&Task_FocusedMeleeAttack_startAction = baseAddr + 0x33bd30;
				*(uintptr_t*)&Task_RangedAttack_startAction = baseAddr + 0x33bac0;
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

	float getTime() const { return *timer; }
	const class hand& getNULL_HAND() const { return *NULL_HAND; }
	class BuildingInteriorDatasManager* getInteriorMgr() const { return _interior; }
	ogre_unordered_map<class GameData*, class DialogLineData*>::type& getDialogDataManager_allDatas() const { return *_DialogDataManager_allDatas; }
	class GameplayOptions* getGameplayOptions() const { return _GameplayOptions; }
	class lektor<class CombatTechniqueData*>& getAttacks() const { return *_attacks; }
	class lektor<class CombatTechniqueData*>& getBlocks() const { return *_blocks; }
	class HarpoonManager* getHarpoonManager() const { return _harpoon; }
	class NxShape**& getShapeBuffer() const { return *_shapeBuffer; }
	class HandleManager& getHandleManager() const { return *_handleMgr; }
	ogre_unordered_map<enum TaskType, class TaskData*>::type& getTaskDatas() const { return *_TaskDatas; }

	bool (*EscMenu_openedOtherWindows)(class EscMenu*);
	int (*getPrice)(class GameData*, float, bool, class GameData*, class GameData*);
	void (*MessageRoller_clear)(class MessageRoller*);
	class UniqueNPCManager* (*UniqueNPCManager_getSingleton)();
	void (*UniqueNPCManager_remove)(class UniqueNPCManager*, class GameData*, const class hand&);
	class AreaSector* (*AreasList_getAreaSector)(class AreasList*, const Ogre::Vector3&);
	bool (*BuildingInteriorDatasManager_hasLayout)(class BuildingInteriorDatasManager*, class GameData*, const std::string&, bool);
	class SubjectiveTags* (*CharacterMemory_getSubjectiveTags)(class CharacterMemory*, class Character*);
	bool (*VendorListManager_hasVendor)(class ActivePlatoon*);
	class InventoryManager* (*InventoryManager_getSingleton)();
	void (*InventoryManager_refreshBuildingInventory)(class InventoryManager*, class Building*, class GameData*, class Platoon*, bool, bool);
	void (*InventoryManager_getBuildingInventoryList)(class InventoryManager*, class Building*, void*);
	void (*InventoryManager_clearAllBuildingInventory)(class InventoryManager*, class Building*);
	void (*BuildingInterior_destroyAllInternalBuildings)(class BuildingInterior*, bool);
	void (*NodeList_destroyNodesByBuilding)(class NodeList*, const class hand&, bool);
	void (*CharacterMemory_getGUIData)(class CharacterMemory*, class DatapanelGUI*, int);
	void (*setTotalCoverage)(class GameData*);
	void (*loadPartmap)(Ogre::SharedPtr<Ogre::Mesh>, class GameData*, bool, bool);
	void (*Harpoon_destroy)(class Harpoon*);
	class Harpoon* (*HarpoonManager_create)(class HarpoonManager*, const std::string&, const std::string&);
	bool (*CharacterMemory_getCharacterMemoryTag)(class CharacterMemory*, class Character*, enum CharacterPerceptionTags_ShortTerm);
	void (*Harpoon_trace)(class Harpoon*, const Ogre::Vector3&, const Ogre::Vector3&);
	int (*convertRarityToLevel)(uint32_t);
	void (*crossbowLevelSelector)(class FitnessSelector<uint32_t>&, class GameData*);
	float (*calculateStatsMult)(int, float, float);
	void (*InventoryManager_refreshSquadInventory)(class InventoryManager*, class ActivePlatoon*);
	class AreaBiomeGroup* (*AreasList_getBiome)(class AreasList*, const Ogre::Vector3&);
	void (*Task_FirstAid_updateItem)(class Task_FirstAid*, enum ItemFunction, class Character*, class Character*);
	bool (*Task_FirstAid_removeItemAutoDestroy)(class Item*, class CharBody*);
	bool (*Task_FirstAid_applyFirstAid)(class Task_FirstAid*, class Task_FirstAid*, class Character*, float, class Character*, bool);
	void (*Task_FirstAid_runAction)(class Task_FirstAid*, class CharBody*);
	void (*Task_FirstAidRig_runAction)(class Task_FirstAidRig*, class CharBody*);
	void (*Task_MeleeAttack_startAction)(class Task_MeleeAttack*, class CharBody*);
	void (*Task_FocusedMeleeAttack_startAction)(class Task_FocusedMeleeAttack*, class CharBody*);
	void (*Task_RangedAttack_startAction)(class Task_RangedAttack*, class CharBody*);

private:
	float* timer;
	class hand* NULL_HAND;
	class BuildingInteriorDatasManager* _interior;
	ogre_unordered_map<class GameData*, class DialogLineData*>::type* _DialogDataManager_allDatas;
	class GameplayOptions* _GameplayOptions;
	class lektor<class CombatTechniqueData*>* _attacks;
	class lektor<class CombatTechniqueData*>* _blocks;
	class HarpoonManager* _harpoon;
	class NxShape*** _shapeBuffer;
	class HandleManager* _handleMgr;
	ogre_unordered_map<enum TaskType, class TaskData*>::type* _TaskDatas;
};

namespace KEP
{
	extern boost::scoped_ptr<FunctionPointers> functions;
}