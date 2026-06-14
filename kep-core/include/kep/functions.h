#pragma once
#include <cstdint>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <ogre/OgrePrerequisites.h>
#include <kenshi/util/OgreUnordered.h>
#include <kep/common.h>

enum TownType;
enum SlaveStateEnum;
class GameData;
class Faction;
class TownBase;
class Research;
class MessageRoller;
class UniqueNPCManager;
class hand;
class BiomeManager;
class TownList;
class ZoneManager;
class StateBroadcastData;
class EscMenu;
class OrdersReceiver;
class Blackboard;
class ActionDeque;
class AITaskSystem;
class Town;
class Nest;
class AreaSector;
class AreaManager;
class LevelManager;
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
class KingOfRenderThread;

namespace KEP
{
	class FunctionPointers
	{
	public:
		FunctionPointers();
		void init(unsigned int platform, const std::string& version, uintptr_t baseAddr);
		DECLSPEC float getTime() const;
		DECLSPEC LevelManager* getLevelManager() const;
		DECLSPEC const hand& getNULL_HAND() const;
		DECLSPEC BuildingInteriorDatasManager* getInteriorMgr() const;
		DECLSPEC ogre_unordered_map<GameData*, DialogLineData*>::type& getDialogDataManager_allDatas() const;
		DECLSPEC GameplayOptions* getGameplayOptions() const;

		bool (*EscMenu_openedOtherWindows)(EscMenu*);
		bool (*Research_completed)(Research*, GameData*);
		void (*Research_complete)(Research*, GameData*);
		void (*Research_createBlueprint)(Research*, GameData*);
		void (*Research_addResearchProgress)(Research*, float);
		int (*getPrice)(GameData*, float, bool, GameData*, GameData*);
		void (*MessageRoller_clear)(MessageRoller*);
		UniqueNPCManager* (*UniqueNPCManager_getSingleton)();
		void (*UniqueNPCManager_remove)(UniqueNPCManager*, GameData*, const hand&);
		Town* (*TownList_getTown)(TownList*, const Ogre::Vector3&, Faction*, TownBase*, Faction*, TownType);
		Town* (*TownList_getTownWithPlayer)(TownList*, const Ogre::Vector3&, bool, float);
		void (*StateBroadcastData_setSlaveState)(StateBroadcastData*, SlaveStateEnum);
		void (*OrdersReceiver_deleteAllTask)(OrdersReceiver*);
		void (*Blackboard_changeAI)(Blackboard*, GameData*);
		void (*Blackboard_setSquadPackage)(Blackboard*, GameData*);
		void (*Blackboard_deleteAllPackage)(Blackboard*);
		void (*Blackboard_setFallbackPackage)(Blackboard*, GameData*);
		void (*OrdersReceiver_deleteAllPassiveJob)(OrdersReceiver*);
		void (*ActionDeque_clear)(ActionDeque*);
		void (*AITaskSystem_clear)(AITaskSystem*);
		Nest* (*TownList_getNest)(TownList*, const Ogre::Vector3&, Faction*, TownBase*);
		TownBase* (*TownList_getTownBase)(TownList*, const Ogre::Vector3&, Faction*, TownBase*);
		AreaSector* (*AreaManager_getAreaSector)(AreaManager*, const Ogre::Vector3&);
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
		void (*KingOfRenderThread_newGameWithCharEdit)(KingOfRenderThread*, GameData*);

	private:
		float* timer;
		LevelManager* _levelMgr;
		hand* NULL_HAND;
		BuildingInteriorDatasManager* _interior;
		ogre_unordered_map<GameData*, DialogLineData*>::type* _DialogDataManager_allDatas;
		GameplayOptions* _GameplayOptions;
	};

	extern DECLSPEC boost::scoped_ptr<FunctionPointers> functions;
}