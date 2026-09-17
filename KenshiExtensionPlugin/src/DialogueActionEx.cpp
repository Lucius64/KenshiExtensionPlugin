/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreStringConverter.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/Faction.h>
#include <kenshi/FactionRelations.h>
#include <kenshi/Platoon.h>
#include <kenshi/Character.h>
#include <kenshi/Dialogue.h>
#include <kenshi/FactionWarMgr.h>
#include <kenshi/Campaign.h>

#include <kep/utility.h>
#include <kep/translation.h>
#include <Settings.h>
#include <EnumExtended.h>

#include <DialogueActionEx.h>

class CampaignLock
{
public:
	~CampaignLock();
	void save(FactionWarMgr* warMgr, GameData* state);
	void load(FactionWarMgr* warMgr, GameData* state);
	void setLockState(FactionWarMgr* warMgr, CampaignData* campaign, bool on);
	void clearState();

private:
	Ogre::map<CampaignData*, bool>::type locked;
};

CampaignLock::~CampaignLock()
{
	clearState();
}

void CampaignLock::save(FactionWarMgr* warMgr, GameData* state)
{
	auto& possibleCampaigns = warMgr->possibleCampaigns;
	for (uint32_t i = 0; i < possibleCampaigns.size(); ++i)
	{
		auto it = locked.find(possibleCampaigns[i]);
		if (it != locked.end())
			state->bdata["poss lock" + Ogre::StringConverter::toString(i)] = it->second;
	}
}

void CampaignLock::load(FactionWarMgr* warMgr, GameData* state)
{
	clearState();
	locked.clear();
	int numPoss = state->idata["num poss"];
	for (int i = 0; i < numPoss; ++i)
	{
		auto& sid = state->sdata["poss sid" + Ogre::StringConverter::toString(i)];
		for (auto campaignIt = warMgr->possibleCampaigns.begin(); campaignIt != warMgr->possibleCampaigns.end(); ++campaignIt)
		{
			auto campaign = *campaignIt;
			if (campaign->data->stringID == sid)
			{
				auto it = state->bdata.find("poss lock" + Ogre::StringConverter::toString(i));
				if (it != state->bdata.end())
				{
					locked[campaign] = it->second;
					campaign->triggersRandomly_territorial = !it->second;
				}
				break;
			}
		}
	}
}

void CampaignLock::setLockState(FactionWarMgr* warMgr, CampaignData* campaign, bool on)
{
	for (auto iter = warMgr->possibleCampaigns.begin(); iter != warMgr->possibleCampaigns.end(); ++iter)
	{
		if (*iter == campaign)
		{
			locked[campaign] = on;
			(*iter)->triggersRandomly_territorial = !on;
			break;
		}
	}
}

void CampaignLock::clearState()
{
	for (auto iter = locked.begin(); iter != locked.end(); ++iter)
	{
		auto campaign = iter->first;
		if (campaign != nullptr)
			campaign->triggersRandomly_territorial = campaign->data->bdata["territorial triggers"];
	}
}

namespace
{
	ogre_unordered_map<FactionWarMgr*, CampaignLock>::type campaignLocks;

	bool (*GameWorld_initialisation_orig)(GameWorld* self);
	bool GameWorld_initialisation_hook(GameWorld* self)
	{
		bool ret = GameWorld_initialisation_orig(self);
		if (KEP::settings._dialogueActionEx && ret)
		{
			campaignLocks.clear();
			for (auto iter = self->factionMgr->participants.begin(); iter != self->factionMgr->participants.end(); ++iter)
			{
				campaignLocks.emplace(std::make_pair((*iter)->warMgr, CampaignLock()));
			}
		}
		return ret;
	}
	void (*FactionWarMgr_save_orig)(FactionWarMgr* self, GameData* s);
	void FactionWarMgr_save_hook(FactionWarMgr* self, GameData* s)
	{
		FactionWarMgr_save_orig(self, s);
		if (KEP::settings._dialogueActionEx)
		{
			auto it = campaignLocks.find(self);
			if (it != campaignLocks.end())
				it->second.save(self, s);
		}
	}

	void (*FactionWarMgr_load_orig)(FactionWarMgr* self, GameData* s);
	void FactionWarMgr_load_hook(FactionWarMgr* self, GameData* s)
	{
		FactionWarMgr_load_orig(self, s);
		if (KEP::settings._dialogueActionEx)
		{
			auto it = campaignLocks.find(self);
			if (it != campaignLocks.end())
				it->second.load(self, s);
		}
	}

	void (*Dialogue__doActons_orig)(Dialogue* self, DialogLineData* dialogLine);
	void Dialogue__doActons_hook(Dialogue* self, DialogLineData* dialogLine)
	{
		if (KEP::settings._dialogueActionEx)
		{
			auto warMgr = self->me->getFaction()->warMgr;
			auto campaignLocksIt = campaignLocks.find(warMgr);
			if (campaignLocksIt != campaignLocks.end())
			{
				auto locks = dialogLine->data->getReferenceListIfExists("lock campaign");
				if (locks != nullptr)
				{
					for (auto iter = locks->begin(); iter != locks->end(); ++iter)
					{
						auto data = iter->getPtr(&ou->gamedata);
						if (data != nullptr)
						{
							for (auto campaignIt = warMgr->possibleCampaigns.begin(); campaignIt != warMgr->possibleCampaigns.end(); ++campaignIt)
							{
								auto campaign = *campaignIt;
								if (campaign->data == data)
								{
									campaignLocksIt->second.setLockState(warMgr, campaign, true);
									break;
								}
							}
						}
					}
				}

				auto unlocks = dialogLine->data->getReferenceListIfExists("unlock campaign");
				if (unlocks != nullptr)
				{

					for (auto iter = unlocks->begin(); iter != unlocks->end(); ++iter)
					{
						auto data = iter->getPtr(&ou->gamedata);
						if (data != nullptr)
						{
							for (auto campaignIt = warMgr->possibleCampaigns.begin(); campaignIt != warMgr->possibleCampaigns.end(); ++campaignIt)
							{
								auto campaign = *campaignIt;
								if (campaign->data == data)
								{
									campaignLocksIt->second.setLockState(warMgr, campaign, false);
									break;
								}
							}
						}
					}
				}
			}
		}

		Dialogue__doActons_orig(self, dialogLine);
		if (KEP::settings._dialogueActionEx)
		{
			for (auto iter = dialogLine->actions.begin(); iter != dialogLine->actions.end(); ++iter)
			{
				auto action = *iter;
				if (action->key == DA_DECREASE_FACTION_RANK)
				{
					auto myFaction = self->me->getFaction();
					if (action->value < myFaction->relations->playerRank)
						--myFaction->relations->playerRank;
				}
			}
		}
	}
}

void KEP::DialogueActionEx::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&GameWorld::initialisation), &GameWorld_initialisation_hook, &GameWorld_initialisation_orig))
		ErrorLog("[GameWorld::initialisation] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&FactionWarMgr::save), &FactionWarMgr_save_hook, &FactionWarMgr_save_orig))
		ErrorLog("[FactionWarMgr::save] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&FactionWarMgr::load), &FactionWarMgr_load_hook, &FactionWarMgr_load_orig))
		ErrorLog("[FactionWarMgr::load] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Dialogue::_doActions), &Dialogue__doActons_hook, &Dialogue__doActons_orig))
		ErrorLog("[Dialogue::_doActions] could not install hook!");
}
