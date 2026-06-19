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

	ogre_unordered_map<int, lektor<AIPackage*>>::type _0x8; //0x8
	AIPackage* currentPackage; //0x48
	int currentPriority; //0x50
	std::string replacementAI; //0x58
	hand handle; //0x80
	Platoon* platoon; //0xa0
	ogre_unordered_map<TaskType, hand>::type tasks; //0xa8
	bool homelok; //0xe8
	TownBase* targetTown; //0xf0
	ZoneMap* zone; //0xf8
	GameData* missionData; //0x100
	char _0x108; //0x108
	hand homeBuilding; //0x110
	bool _0x130; //0x130
	TownBase* currentTown; //0x138
	TimeOfDay towntime; //0x140
	int maxPriority; //0x148
	bool runningAwayMode; //0x14c
	int numCapacitatedMember; //0x150
	int numEnemy; //0x154
	ogre_unordered_map<hand, TaskRequest>::type taskRequests; //0x158
	lektor<TaskRequest> leaderTasks; //0x198
	lektor<TaskRequest> squad1Tasks; //0x1b0
	lektor<TaskRequest> squad2Tasks; //0x1c8
	lektor<TaskRequest> signalsPlanTasks; //0x1e0
	lektor<TaskRequest> slaveTasks; //0x1f8
	AIPackage* contractJob; //0x210
	double jobhr; //0x218
	ogre_unordered_map<hand, TaskInfo>::type taskInfos; //0x220
	bool _0x260; //0x260
	ogre_unordered_map<hand, Ogre::Vector3>::type positions; //0x268

	virtual void vfunc0x0();
};
