#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/Faction.h>

#include <extern/TownBase.h>
#include <extern/FactionWarMgr.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <WarCampaignExtension.h>


namespace
{
	void (*TownBase_setFaction_orig)(TownBase*, Faction*, ActivePlatoon*);
}

void KEP::WarCampaignExtension::TownBase_setFaction_hook(TownBase* self, Faction* p, ActivePlatoon* a)
{
	if (!settings._enableNotRealWarCampaign)
	{
		TownBase_setFaction_orig(self, p, a);
		return;
	}

	if (p == nullptr)
		p = ou->factionMgr->getEmptyFaction();

	if (self->owner != p)
	{
		if (self->owner != nullptr)
			self->owner->warMgr->myTowns.remove(self);

		if (self->isTown() != nullptr || self->getDataType() == NEST)
			p->warMgr->myTowns.push_back_unique(self);
	}

	self->owner = p;
}

void KEP::WarCampaignExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_00927900, &TownBase_setFaction_hook, &TownBase_setFaction_orig))
		ErrorLog("KenshiExtensionPlugin: [Faction war extension] could not install hook!");
}
