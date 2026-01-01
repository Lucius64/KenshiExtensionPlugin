#pragma once

class TownBase;
class Faction;
class ActivePlatoon;

namespace KEP
{
	namespace WarCampaignExtension
	{
		void TownBase_setFaction_hook(TownBase* self, Faction* faction, ActivePlatoon* plat);
		void init();
	}
}