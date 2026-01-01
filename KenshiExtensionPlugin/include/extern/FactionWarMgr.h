#pragma once
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreVector3.h>
#include <kenshi/util/lektor.h>
#include <kenshi/util/hand.h>
#include <kenshi/util/TimeOfDay.h>
#include <kenshi/WorldEventStateQuery.h>

enum MoveSpeed;
class Faction;
class TownBase;
class Platoon;
class AreaBiomeGroup;
class CampaignTriggerData;
class CampaignInstance;

// KenshiLibでは未定義
class CampaignData
{
public:
	Faction* owner;
	bool targetCharacters;
	bool territorialTriggers;
	bool isHostile;
	int targetTechLevel;
	int targetPopulationMin;
	float chancePerDay;
	int _0x20;
	int _0x24;
	float rangeNear;
	float rangeFar;
	WorldEventStateQueryList worldState;
	lektor<GameData*> specificTargetNPC;
	lektor<GameData*> specialLeaders;
	FitnessSelector<GameData*> squadToUse;
	float repeatLimit;
	uint32_t key;
	TimeOfDay possTime;
	bool hasPt2AI;
	Faction* factionOverride;
	lektor<GameData*> triggerAlly;
	GameData* data;
	Ogre::map<uint32_t, GameData*>::type leaderSquadAI;
	Ogre::map<uint32_t, GameData*>::type othersSquadAI;
	FitnessSelector<CampaignTriggerData*> lossTrigger;
	FitnessSelector<CampaignTriggerData*> victoryTrigger;

	virtual RootObject* vfunc0x0(Faction*, TownBase*);
	virtual float vfunc0x8(Faction*, TownBase*);
	virtual bool vfunc0x10();
	virtual CampaignInstance* vfunc0x18(int, Faction*, hand&, hand&);
};

// KenshiLibでは未定義
class CampaignInstance
{
public:
	int forces;
	CampaignData* campaginData;
	TimeOfDay update;
	TimeOfDay update2;
	MoveSpeed travelSpeedUnloaded;
	MoveSpeed travelSpeedLoaded;
	int numForcesMin;
	int numForcesMax;
	int numForces;
	bool rd;
	bool trig;
	hand handle;
	Ogre::Vector3 pos;
	itemType type;
	Faction* owner;
	Faction* enemy;
	hand leader;
	hand home;
	uint32_t phase;
	bool canTalkBeforeArrival;
	lektor<hand> squads;

	virtual ~CampaignInstance();
	virtual void func0x8();
	virtual void triggerVictory();
	virtual void triggerDefeated();
	virtual void setLeaderHandle(hand&);
	virtual Character* getLeaderCharacter();
	virtual Platoon* getLeaderPlatoon();
	virtual bool isEnemy(Character*);
	virtual std::string getDisplayName();
	virtual void func0x48();
	virtual void func0x50();
	virtual void func0x58();
	virtual uint32_t getUnloadedPlatoonJob(Platoon*);
	virtual GameData* getCampaginAI(Platoon*);
	virtual void vfunc0x70(Platoon*, bool);
	virtual bool func0x78(Character*);
	virtual void changeLeader(Platoon*);
};

// KenshiLibでは未定義
class FactionWarMgr
{
public:
	class CampaignRequest;

	Faction* owner;
	int fwcid;
	lektor<TownBase*> myTowns;
	Ogre::map<Platoon*, CampaignInstance*>::type platoons;
	lektor<hand> handles;
	lektor<CampaignInstance*> actives;
	lektor<AreaBiomeGroup*> biomes;
	lektor<AreaBiomeGroup*> noGoZones;
	TimeOfDay updateTime;
	lektor<CampaignRequest*> requests;
	lektor<CampaignData*> poss;
};