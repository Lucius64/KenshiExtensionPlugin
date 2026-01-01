#pragma once
#include <stdint.h>
#include <ogre/OgrePrerequisites.h>

class Faction;
class GameData;

// KenshiLibでは未定義
class FactionRelations
{
public:
	// KenshiLibでは未定義
	class RelationData
	{
	public:
		bool own;
		bool _0x1;
		bool _0x2;
		bool isCoexistence;
		float relation;
		float trust;
		float trustNeg;
		int _0x10;
		Ogre::map<std::string, int>::type _0x18;
	private:

	};

	Faction* owner;
	int rank;
	float trustworthy;
	float reputation;
	RelationData relation;
	float defaultRelation;

	virtual void saveState(GameData* state);
	virtual void loadState(GameData* state, bool isImport);
	virtual void setOwnFactionRelation();
	virtual void vfunc0x18(Faction*);
	virtual void addRelation(Faction* target, float amount, float mult);
	virtual void vfunc0x28(Faction* target, uint32_t type, float amount);
	virtual void addTrust(Faction* target, float amount, float mult);
	virtual void endWar(Faction* target);
	virtual void declareWar(Faction* target);
	virtual void addReputation(Faction* target, float amount);
	virtual RelationData* getRelationData(Faction* target);
};