#pragma once
#include <ogre/OgrePrerequisites.h>
#include <kenshi/util/OgreUnordered.h>
#include <kenshi/SensoryData.h>

enum SquadMemberType;
class Platoon;
class AITaskSystem;
class TaskRepertoire;
class TradingInfo;
class Character;
class CharStats;
class CharBody;
class StateBroadcastData;
class MedicalSystem;
class Inventory;
class Blackboard;
class GameData;
class Faction;
class Tasker;
class TaskData;


// KenshiLibでは未定義
enum TaskPriority
{
	TP_JUST_ACTION,
	TP_FLUFF,
	TP_NON_URGENT,
	TP_URGENT,
	TP_OBEDIENCE,
	TP_MAX_SIZE
};

// KenshiLibでは未定義
class AI
{
public:

	float _0x8;
	float _0xc;
	Platoon* _0x10;
	StateBroadcastData* _0x18;
	AITaskSystem* taskSystem;
	SensoryData _0x28;
	char _0x208[0xa0];
	TaskRepertoire* _0x2a8;
	TradingInfo* _0x2b0;
	int _0x2b8;
	int _0x2bc;
	Ogre::Vector3 _0x2c0;
	hand _0x2d0;
	double _0x2f0;
	Character* me;
	CharBody* body;
	Inventory* inventory;
	CharStats* stats;
	MedicalSystem* medical;

	virtual bool vfunc0x0();
};

// KenshiLibでは未定義
class ActionDeque
{
public:
	std::deque<Tasker*> deque;

	virtual ~ActionDeque();
};

// KenshiLibでは未定義
class TaskMatch
{
public:
	TaskData* taskData;
	hand _0x8;
	hand _0x28;
};

// KenshiLibでは未定義
class OrdersReceiver
{
public:
	// KenshiLibでは未定義
	class TaskRequestSimple;

	Character* me;
	hand handle;
	int _0x28;
	char _0x2c;
	bool jobs;
	ActionDeque actionDeque;
	lektor<Tasker*> taskJobs;
	lektor<Tasker*> passiveJobs;
	lektor<Tasker*> fundamentallyTasks[5];
	lektor<Tasker*> packageTasks[5];
	Tasker* _0x190;
	lektor<TaskRequestSimple*> _0x198;
	bool _0x1b0;
	bool _0x1b1;
	AI* ai;
	TaskMatch taskMatch;
	float _0x208;
	TaskPriority priority;

	virtual ~OrdersReceiver();
};

// KenshiLibでは未定義
class AITaskSystem : public OrdersReceiver
{
public:
	
	double _0x210;
	ogre_unordered_map<hand, float>::type _0x218;
	SquadMemberType memberType;
	int _0x260;
	EventTriggerEnum _0x264;
	EventTriggerEnum _0x268;
	float _0x26c;
	float _0x26d;
	CharBody* body;
	Character* _0x278;
	double _0x280;
	size_t _0x288;
	Tasker* _0x290;
	Tasker* _0x298;
	Tasker* _0x2a0;
	Tasker* _0x2a8;
	size_t _0x2b0;
	size_t _0x2b8;
	Ogre::map<float, Tasker*>::type _0x2c0;
	lektor<Tasker*> _0x2e8;
	ActionDeque _0x300;
	ActionDeque _0x338;
	std::string _0x370;
	Ogre::Vector3 _0x398;
	size_t _0x3a8;
	int _0x3b0;

	virtual ~AITaskSystem();
	virtual void serialize(GameData*);
	virtual void deserialize(GameData*);
};