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
#include "common.h"

class GameData;
class Research;
class MessageRoller;
class UniqueNPCManager;
class hand;
class EscMenu;
class AreaSector;
class AreasList;
class BuildingInteriorDatasManager;
class Character;
class CharacterMemory;
class SubjectiveTags;
class InventoryManager;
class Building;
class Platoon;
class ActivePlatoon;
class DialogLineData;
class GameplayOptions;
class BuildingInterior;
class NodeList;
class DatapanelGUI;
class CombatTechniqueData;
class HarpoonManager;
class Harpoon;
template<typename T>
class lektor;
enum CharacterPerceptionTags_ShortTerm;
class NxShape;
template<typename T>
class FitnessSelector;
class RootObjectBase;
class Faction;
class TownBase;
class HandleManager;
enum TaskType;
class TaskData;

namespace KEP
{
	class FunctionPointers
	{
	public:
		FunctionPointers();
		void init(unsigned int platform, const std::string& version, uintptr_t baseAddr);
		DECLSPEC float getTime() const;
		DECLSPEC const hand& getNULL_HAND() const;
		DECLSPEC BuildingInteriorDatasManager* getInteriorMgr() const;
		DECLSPEC ogre_unordered_map<GameData*, DialogLineData*>::type& getDialogDataManager_allDatas() const;
		DECLSPEC GameplayOptions* getGameplayOptions() const;
		DECLSPEC lektor<CombatTechniqueData*>& getAttacks() const;
		DECLSPEC lektor<CombatTechniqueData*>& getBlocks() const;
		DECLSPEC HarpoonManager* getHarpoonManager() const;
		DECLSPEC NxShape**& getShapeBuffer() const;
		DECLSPEC HandleManager& getHandleManager() const;
		DECLSPEC ogre_unordered_map<TaskType, TaskData*>::type& getTaskDatas() const;

		bool (*EscMenu_openedOtherWindows)(EscMenu*);
		bool (*Research_completed)(Research*, GameData*);
		void (*Research_complete)(Research*, GameData*);
		void (*Research_createBlueprint)(Research*, GameData*);
		void (*Research_addResearchProgress)(Research*, float);
		int (*getPrice)(GameData*, float, bool, GameData*, GameData*);
		void (*MessageRoller_clear)(MessageRoller*);
		UniqueNPCManager* (*UniqueNPCManager_getSingleton)();
		void (*UniqueNPCManager_remove)(UniqueNPCManager*, GameData*, const hand&);
		AreaSector* (*AreasList_getAreaSector)(AreasList*, const Ogre::Vector3&);
		bool (*BuildingInteriorDatasManager_hasLayout)(BuildingInteriorDatasManager*, GameData*, const std::string&, bool);
		SubjectiveTags* (*CharacterMemory_getSubjectiveTags)(CharacterMemory*, Character*);
		bool (*VendorListManager_hasVendor)(ActivePlatoon*);
		InventoryManager* (*InventoryManager_getSingleton)();
		void (*InventoryManager_refreshBuildingInventory)(InventoryManager*, Building*, GameData*, Platoon*, bool, bool);
		void (*InventoryManager_getBuildingInventoryList)(InventoryManager*, Building*, void*);
		void (*InventoryManager_clearAllBuildingInventory)(InventoryManager*, Building*);
		void (*BuildingInterior_destroyAllInternalBuildings)(BuildingInterior*, bool);
		void (*NodeList_destroyNodesByBuilding)(NodeList*, const hand&, bool);
		void (*CharacterMemory_getGUIData)(CharacterMemory*, DatapanelGUI*, int);
		void (*Research_init)(Research*);
		void (*setTotalCoverage)(GameData*);
		void (*loadPartmap)(Ogre::SharedPtr<Ogre::Mesh>, GameData*, bool, bool);
		void (*Harpoon_destroy)(Harpoon*);
		Harpoon* (*HarpoonManager_create)(HarpoonManager*, const std::string&, const std::string&);
		bool (*CharacterMemory_getCharacterMemoryTag)(CharacterMemory*, Character*, CharacterPerceptionTags_ShortTerm);
		void (*Harpoon_trace)(Harpoon*, const Ogre::Vector3&, const Ogre::Vector3&);
		int (*convertRarityToLevel)(uint32_t);
		void (*crossbowLevelSelector)(FitnessSelector<uint32_t>&, GameData*);
		float (*calculateStatsMult)(int, float, float);
		void (*InventoryManager_refreshSquadInventory)(InventoryManager*, ActivePlatoon*);

	private:
		float* timer;
		hand* NULL_HAND;
		BuildingInteriorDatasManager* _interior;
		ogre_unordered_map<GameData*, DialogLineData*>::type* _DialogDataManager_allDatas;
		GameplayOptions* _GameplayOptions;
		lektor<CombatTechniqueData*>* _attacks;
		lektor<CombatTechniqueData*>* _blocks;
		HarpoonManager* _harpoon;
		NxShape*** _shapeBuffer;
		HandleManager* _handleMgr;
		ogre_unordered_map<TaskType, TaskData*>::type* _TaskDatas;
	};

	extern DECLSPEC boost::scoped_ptr<FunctionPointers> functions;
}