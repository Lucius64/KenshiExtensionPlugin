#pragma once
#include <kenshi/RootObject.h>
#include <kenshi/SensoryData.h>

enum BuildingFunction;
enum BuildingDesignation;
class TradeCulture;
class Nest;
class Town;
class DistantTown;
class GatewayBuilding;
class InstanceManager;
class BasePopulationManager;
class PhysicsHullT;
class BuildingFinderClass;
class AlarmManager;
class NestBatcher;
class AreaBiomeGroup;

// KenshiLibでは未定義
enum TownType
{
	TOWN_NEST,
	TOWN_OUTPOST,
	TOWN_TOWN,
	TOWN_VILLAGE,
	TOWN_RUINS,
	TOWN_SLAVE_CAMP,
	TOWN_MILITARY,
	TOWN_PRISON,
	TOWN_NEST_MARKER,
	TOWN_POI,
	TOWN_NULL
};

// KenshiLibでは未定義
class TownBuildingsManager
{
public:
	// KenshiLibでは未定義
	struct BuildingInfo
	{
		size_t _0x0[13];
	};

	ogre_unordered_map<Building*, BuildingInfo>::type _0x8;
	ogre_unordered_map<std::string, InstanceManager*>::type _0x48;
	size_t _0x88;
	size_t _0x90;
	size_t _0x98;
	bool _0xa0;
	TownBase* owner;

	virtual void vfunc0x0();
	virtual ~TownBuildingsManager();
};

// KenshiLibでは未定義
class TownBase : public RootObject
{
public:
	// KenshiLibでは未定義
	struct ResidentData
	{
		GameData* squadTemplate;
		ogre_unordered_set<GameData*>::type dislikeBuildings;
		ogre_unordered_set<GameData*>::type likeBuildings;
		int num;
		int chance;
		int priority;
	};

	double _0xc0;
	float _0xc8;
	Ogre::Vector3 _0xcc;
	TownType townType;
	BasePopulationManager* populationMgr;
	Ogre::set<Faction*>::type factions;
	bool _0x110;
	bool isSecret;
	Ogre::set<hand>::type handles;
	double tod;
	float _0x148;
	NestBatcher* nestBatcher;
	std::string unexploredName;
	bool discovered;
	bool explored;
	bool recentlyDiscovered;
	size_t _0x188;
	size_t _0x190;
	PhysicsHullT* physicsHull;
	AreaBiomeGroup* areaBiomeGroup;
	int _0x1a8;
	ogre_unordered_set<ZoneMap*>::type belongZones;
	ogre_unordered_map<ZoneMap*, bool>::type activatedZones;
	size_t _0x230;
	size_t _0x238;
	size_t _0x240;
	char _0x248;
	TownBuildingsManager townBuildingMgr;
	lektor<ResidentData> residents;
	lektor<ResidentData> barSquads;
	GameData* defaultResident;

	virtual ~TownBase();
	virtual void func0x228(ZoneMap*);
	virtual bool isActivated();
	virtual void setupTown(GameData* basedata, Ogre::Vector3& pos, hand& handle);
	virtual void setupPopulationMgr();
	virtual int func0x248();
	virtual void func0x250(int);
	virtual void func0x258();
	virtual bool isDangerousTown();
	virtual Town* isTown();
	virtual Nest* isNest();
	virtual void setupSpawnMgs(GameData* basedata, Ogre::Vector3& pos, Building* building);
	virtual bool func0x280();
	virtual bool isDead();
	virtual Ogre::Vector3 getGatewayDoorPosition(float range);
	virtual bool isPublic();
	virtual float getTownRange();
	virtual void serializeTownState(GameData* savedata);
	virtual void deserializeTownState(GameData* savedata);
	virtual void setupPower();
	virtual bool func0x2c0(Faction*);
	virtual void func0x2c8(BuildingFinderClass*);
	virtual lektor<Building*>& getBuildingsByBuildingDesignation(BuildingDesignation bd, Character* character);
	virtual lektor<Building*>& getBuildingsByBuildingFunction(BuildingFunction bf, Character* character);
	virtual bool func0x2e0();
	virtual AlarmManager* getAlarmManager();
	virtual bool hasFortBuilding();
	virtual void addFortBuilding(hand& handle);
	virtual bool isLockedOrClosedAllGatewayBuilding();
	virtual GatewayBuilding* getNearestGatewayBuilding(Ogre::Vector3& pos);
	virtual AreaBiomeGroup* getBiomeGroup();
	virtual bool getRecentlyDiscovered();
	virtual void setRecentlyDiscovered(bool recentlyDiscovered);
	virtual bool getDiscovered();
	virtual void setDiscovered(bool discovered);
	virtual void destoryNest();
	virtual bool getExplored();
	virtual void setExplored(bool explored);
	virtual std::string& getUnexploredName();
	virtual int getItemArtifactsMinValue();
	virtual int getItemArtifactsMaxValue();
	virtual int getGearArtifactsMinValue();
	virtual int getGearArtifactsMaxValue();
	virtual std::string& getTownLevelName();
	virtual uint32_t func0x380();
	virtual void setupDebris(GameData* basedata, Ogre::Vector3& pos, RootObject* obj);
};

// KenshiLibでは未定義
class Town : public TownBase
{
public:
	// KenshiLibでは未定義
	struct NestSpot
	{
		Ogre::Vector3 pos;
		GameData* data;
		hand handle;
	};

	GameData* replacementTown;
	bool _0x340;
	AlarmManager* alarmMgr;
	std::string _0x350;
	uint16_t _0x378;
	uint16_t _0x37a;
	bool _0x37c;
	char _0x37d;
	char _0x37e;
	char _0x37f;
	bool isPublic;
	float noFoliageRange;
	int townLevel;
	ogre_unordered_set<hand>::type fortBuildings;
	bool _0x3d0;
	size_t _0x3d8;
	size_t _0x3e0;
	size_t _0x3e8;
	ogre_unordered_set<hand>::type _0x3f0;
	ogre_unordered_set<hand>::type _0x430;
	float _0x470[9];
	TagsClass<BuildingDesignation> bdtag;
	TradeCulture* tradeCulture;
	ogre_unordered_map<GameData*, float>::type tradeGoods;
	GameData* buildingMaterial;
	DistantTown* distantTown;
	lektor<NestSpot> nestSpots;
	bool _0x518;
	float playerTownRadius;
	float townRadiusMult;

	virtual ~Town();
	virtual void saveZoneInfo(GameData* savedata);
	virtual void loadZoneInfo(GameData* savedata);
};

// placeholder name
class DistantTown
{
public:
	Town* owner;
	uint32_t _0x8;
	bool visible;
	Ogre::Entity* _0x10;
};