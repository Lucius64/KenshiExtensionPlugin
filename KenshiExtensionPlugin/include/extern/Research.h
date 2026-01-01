#pragma once
#include <kenshi/util/OgreUnordered.h>
#include <kenshi/util/lektor.h>

enum itemType;
class GameData;

class ResearchItem
{
public:
	GameData* gamedata;
	float _0x8;
	int _0xc;
private:

};

class Research
{
public:
	class BuildingUpgrades
	{
	public:
		float productionMult;
		float powerOutput;
		float powerCapacity;

		BuildingUpgrades() : productionMult(1.0f), powerOutput(0), powerCapacity(0) {}
	};

	size_t _0x0;
	int techLevel;
	Ogre::map<itemType, lektor<GameData*>>::type _0x10;
	Ogre::deque<ResearchItem>::type researchQue;
	Ogre::map<GameData*, BuildingUpgrades>::type buildingUpgradeResearchs;
	Ogre::map<std::string, Ogre::vector<ResearchItem>::type>::type _0x98;
	lektor<std::string> _0xc0;
	lektor<std::string> paid;
	ogre_unordered_set<GameData*>::type finished;
	ogre_unordered_set<GameData*>::type _0x130;
	int maxTechLevel;
	int _0x174;
	float _0x178;
};