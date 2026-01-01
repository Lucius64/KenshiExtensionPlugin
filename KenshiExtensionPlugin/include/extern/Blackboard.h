#pragma once
#include <kenshi/Enums.h>
#include <kenshi/util/OgreUnordered.h>
#include <kenshi/util/lektor.h>
#include <kenshi/util/hand.h>


class AIPackage;
class ZoneMap;

// KenshiLibでは未定義
class TaskRequest
{
public:
	hand _0x0;
	hand _0x20;
	TaskType taskType;
	hand _0x48;
	uint32_t priority;
	float weight;
	float _0x70;
	uint32_t targetType;
	int startTime;
	int endTime;
	uint32_t endEvent;
};

// KenshiLibでは未定義
class Blackboard
{
public:
	class TaskInfo
	{
	public:
		TaskType taskType;
		hand _0x8;
		hand _0x28;
	};

	ogre_unordered_map<int, lektor<AIPackage*>>::type _0x0;
	AIPackage* currentPackage;
	int currentPriority;
	std::string replacementAI;
	hand handle;
	Platoon* platoon;
	ogre_unordered_map<TaskType, hand>::type tasks;
	bool homelok;
	TownBase* targetTown;
	ZoneMap* zone;
	GameData* missionData;
	char _0x108;
	hand homeBuilding;
	bool _0x130;
	TownBase* currentTown;
	TimeOfDay towntime;
	int maxPriority;
	bool runningAwayMode;
	int numCapacitatedMember;
	int numEnemy;
	ogre_unordered_map<hand, TaskRequest>::type taskRequests;
	lektor<TaskRequest> leaderTasks;
	lektor<TaskRequest> squad1Tasks;
	lektor<TaskRequest> squad2Tasks;
	lektor<TaskRequest> signalsPlanTasks;
	lektor<TaskRequest> slaveTasks;
	AIPackage* contractJob;
	double jobhr;
	ogre_unordered_map<hand, TaskInfo>::type taskInfos;
	bool _0x260;
	ogre_unordered_map<hand, Ogre::Vector3>::type positions;

	virtual void vfunc0x0();
};

class AIPackage
{
public:
	hand missionTown;
	hand missionTarget;
	hand missionEmployer;
	GameData* data;
	int priority;
	uint32_t signalFunc;
	uint32_t unloadedFunc;
	Blackboard* blackbord;
	Platoon* platoon;
	AIPackage* leadsTo;
	GameData* contractEndTalkPassive;
	GameData* contractEndDialogDelivery;
	bool isBetrayed;

	virtual ~AIPackage();
};
