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
class Blackboard;
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
template<typename T>
class lektor;

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

		bool (*EscMenu_openedOtherWindows)(EscMenu*);
		bool (*Research_completed)(Research*, GameData*);
		void (*Research_complete)(Research*, GameData*);
		void (*Research_createBlueprint)(Research*, GameData*);
		void (*Research_addResearchProgress)(Research*, float);
		int (*getPrice)(GameData*, float, bool, GameData*, GameData*);
		void (*MessageRoller_clear)(MessageRoller*);
		UniqueNPCManager* (*UniqueNPCManager_getSingleton)();
		void (*UniqueNPCManager_remove)(UniqueNPCManager*, GameData*, const hand&);
		void (*Blackboard_changeAI)(Blackboard*, GameData*);
		void (*Blackboard_setSquadPackage)(Blackboard*, GameData*);
		void (*Blackboard_deleteAllPackage)(Blackboard*);
		void (*Blackboard_setFallbackPackage)(Blackboard*, GameData*);
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
		void (*Blackboard_getGUIData)(Blackboard*, DatapanelGUI*, int);
		void (*CharacterMemory_getGUIData)(CharacterMemory*, DatapanelGUI*, int);
		void (*Research_init)(Research*);
		void (*setTotalCoverage)(GameData*);
		void (*loadPartmap)(Ogre::SharedPtr<Ogre::Mesh>, GameData*, bool, bool);

	private:
		float* timer;
		hand* NULL_HAND;
		BuildingInteriorDatasManager* _interior;
		ogre_unordered_map<GameData*, DialogLineData*>::type* _DialogDataManager_allDatas;
		GameplayOptions* _GameplayOptions;
		lektor<CombatTechniqueData*>* _attacks;
		lektor<CombatTechniqueData*>* _blocks;
	};

	extern DECLSPEC boost::scoped_ptr<FunctionPointers> functions;
}