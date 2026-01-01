#pragma once
#include <ogre/OgrePrerequisites.h>

enum SquadType;

class GameData;
class hand;
class RootObjectFactory;
class RootObject;
class RootObjectContainer;
class Faction;
class Platoon;
class ActivePlatoon;
class Building;
class TownBase;
class AreaBiomeGroup;
class YesNoMaybe;
class FactionWarMgr;
class CampaignInstance;
class CampaignData;
class UniqueNPCManager;

namespace KEP
{
	namespace UniqueCharacterFix
	{
		RootObject* RootObjectFactory_createRandomCharacter_hook(RootObjectFactory* self, Faction* faction, Ogre::Vector3 position, RootObjectContainer* owner, GameData* characterTemplate, Building* home, float age);
		RootObject* RootObjectFactory_createRandomSquad_hook(RootObjectFactory* self, Faction* faction, Ogre::Vector3 position, TownBase* homeTown, int maxnum, Building* home, GameData* squad, RootObjectContainer* ownr, AreaBiomeGroup* maparea, Platoon* _activePlatoon, bool permanentsquad, const hand& AItarget, TownBase* targetTown, float sizeMultiplier, SquadType squadType, bool isJustARefresh);
		YesNoMaybe* ActivePlatoon_setupCheck_hook(ActivePlatoon* self, YesNoMaybe* _rvo);
		void FactionWarMgr_FUN_009C4170_hook(FactionWarMgr* self, TownBase* dstTown, CampaignInstance* campaignInstance);
		bool CampaignData_FUN_009C4000_hook(CampaignData* self);
		void ActivePlatoon__checkForUniqueCharactersOnUnload_hook(ActivePlatoon* self);
		void UniqueNPCManager_FUN_009A93A0_hook(UniqueNPCManager* self, GameData* savedata, bool isImport);
		void init();
	}
}

