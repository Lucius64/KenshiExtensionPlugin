#pragma once
#include <cstdint>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <ogre/OgrePrerequisites.h>
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

class LevelManager
{
public:
	BiomeManager* biomeMgr;
	TownList* townList;
	ZoneManager* zoneMgr;
};

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

		bool (*EscMenu_openedOtherWindows)(EscMenu*);
		bool (*Research_completed)(Research*, GameData*);
		void (*Research_complete)(Research*, GameData*);
		void (*Research_createBlueprint)(Research*, GameData*);
		void (*Research_addResearchProgress)(Research*, float);
		int (*getPrice)(GameData*, float, bool, GameData*, GameData*);
		void (*MessageRoller_clear)(MessageRoller*);
		UniqueNPCManager* (*UniqueNPCManager_getSingleton)();
		void (*UniqueNPCManager_remove)(UniqueNPCManager*, GameData*, const hand&);
		TownBase* (*TownList_getSafeTown)(TownList*, const Ogre::Vector3&, Faction*, TownBase*, Faction*, TownType);
		TownBase* (*TownList_getTownWithPlayer)(TownList*, const Ogre::Vector3&, bool, float);
		void (*StateBroadcastData_setSlaveState)(StateBroadcastData*, SlaveStateEnum);
		void (*OrdersReceiver_deleteAllTask)(OrdersReceiver*);
		void (*Blackboard_changeAI)(Blackboard*, GameData*);
		void (*Blackboard_setSquadPackage)(Blackboard*, GameData*);
		void (*Blackboard_deleteAllPackage)(Blackboard*);
		void (*Blackboard_setFallbackPackage)(Blackboard*, GameData*);
		void (*OrdersReceiver_deleteAllPassiveJob)(OrdersReceiver*);
		void (*ActionDeque_clear)(ActionDeque*);
		void (*AITaskSystem_clear)(AITaskSystem*);

	private:
		float* timer;
		LevelManager* _levelMgr;
		hand* NULL_HAND;
	};

	extern DECLSPEC boost::scoped_ptr<FunctionPointers> functions;
}