#pragma once
#include <string>
#include <kenshi/util/lektor.h>
#include <kenshi/util/OgreUnordered.h>
#include <kenshi/util/hand.h>
#include <kenshi/util/TimeOfDay.h>

class Faction;
class GameData;
class TownBase;

class SpawnInfoList
{
public:
	class SpawnInfo
	{
	public:
		Faction* faction;
		TownBase* town;
		GameData* squadTemplate;
		int32_t chance;
		int32_t squadSize;
	};

	lektor<SpawnInfo> spanwInfos;
	int32_t chanceSum;

	virtual void addSpawnInfo(GameData*, std::string&, TownBase*);
};

class Poppulation
{
public:
	TimeOfDay lastUpdate;
	int32_t popd;
	float pop;
	SpawnInfoList spawnInfoList;
	ogre_unordered_set<hand>::type platoonHandleSet;
};

class BasePopulationManager
{
public:
	bool ded;
	bool stated;
	std::array<Poppulation, 3> populationInfoList;
	TownBase* town;
	size_t _0x180;

	virtual ~BasePopulationManager();
	virtual Poppulation* getPopulation(SquadType) = 0;
	virtual void serialize(GameData*);
	virtual void load(GameData*);
	virtual void update() = 0;
	virtual void regenerate();
};

class TownPopulationManager : public BasePopulationManager
{
	virtual ~TownPopulationManager();
	Poppulation* getPopulation(SquadType sqt) override;
	void update() override;
};

class NestPopulationManager : public BasePopulationManager
{
	virtual ~NestPopulationManager();
	Poppulation* getPopulation(SquadType sqt) override;
	void update() override;
};