#include <algorithm>

#include <ogre/OgrePrerequisites.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/util/UtilityT.h>
#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/Faction.h>
#include <kenshi/Character.h>
#include <kenshi/CharStats.h>
#include <kenshi/SensoryData.h>
#include <kenshi/RaceData.h>

#include <extern/AnimationClass.h>
#include <extern/AppearanceManager.h>
#include <extern/AppearanceBase.h>
#include <extern/StateBroadcastData.h>
#include <extern/PortraitManager.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <AppearanceExtension.h>


namespace
{
	void (*AppearanceHuman_FUN_0052B1E0_orig)(AppearanceHuman*);
	void (*AppearanceManager_FUN_0006F8F0_orig)(AppearanceManager*, GameData*, Character*);
	PortraitManager* (*FUN_00413FE0_orig)(PortraitManager*);
}


const KEP::IdleStanceList& KEP::IdleStanceList::getSingleton()
{
	static IdleStanceList instance;
	return instance;
}

KEP::IdleStanceList::IdleStanceList()
{
	auto appearanceManager = externalFunctions->FUN_0053E320();

	ogre_unordered_set<std::string>::type animNameSet;
	for (auto iter = appearanceManager->idleStances.begin(); iter != appearanceManager->idleStances.end(); ++iter)
	{
		animNameSet.emplace((*iter)->name);
	}

	const std::string key = "idle stances";
	for (auto iter = ou->factionMgr->participants.begin(); iter != ou->factionMgr->participants.end(); ++iter)
	{
		auto idleStancesIter = (*iter)->data->objectReferences.find(key);
		if (idleStancesIter != (*iter)->data->objectReferences.end() && idleStancesIter->second.size() != 0)
		{
			auto& list = this->_factionIdleStanceList[*iter];
			for (auto objectReferenceIter = idleStancesIter->second.begin(); objectReferenceIter != idleStancesIter->second.end(); ++objectReferenceIter)
			{
				auto gameData = ou->gamedata.getData(objectReferenceIter->sid, ANIMATION);
				if (gameData != nullptr && objectReferenceIter->values.value[0] >= 0 && animNameSet.count(gameData->name) != 0)
					list[gameData] = objectReferenceIter->values.value[0];
			}
		}
	}

	lektor<GameData*> races;
	ou->gamedata.getDataOfType(races, RACE);

	for (auto iter = races.begin(); iter != races.end(); ++iter)
	{
		auto idleStancesIter = (*iter)->objectReferences.find(key);
		if (idleStancesIter != (*iter)->objectReferences.end() && idleStancesIter->second.size() != 0)
		{
			auto& list = this->_raceIdleStanceList[*iter];
			for (auto objectReferenceIter = idleStancesIter->second.begin(); objectReferenceIter != idleStancesIter->second.end(); ++objectReferenceIter)
			{
				auto gameData = ou->gamedata.getData(objectReferenceIter->sid, ANIMATION);
				if (gameData != nullptr && objectReferenceIter->values.value[0] >= 0 && animNameSet.count(gameData->name) != 0)
					list.push_back(std::make_pair(gameData, objectReferenceIter->values.value[0]));
			}
		}
	}
}

void KEP::AppearanceExtension::AppearanceHuman_FUN_0052B1E0_hook(AppearanceHuman* self)
{
	if (!settings._bulkLimitsExtension)
	{
		AppearanceHuman_FUN_0052B1E0_orig(self);
		return;
	}

	if (self->entity == nullptr)
		return;

	float bulkMin;
	float bulkMax;
	if (self->isFemale)
	{
		auto bulkMinIter = self->raceData->fdata.find("bulk min female");
		auto bulkMaxIter = self->raceData->fdata.find("bulk max female");
		bulkMin = bulkMinIter != self->raceData->fdata.end() ? bulkMinIter->second : settings._bulkStatMinFemale;
		bulkMax = bulkMaxIter != self->raceData->fdata.end() ? bulkMaxIter->second : settings._bulkStatMaxFemale;
	}
	else
	{
		auto bulkMinIter = self->raceData->fdata.find("bulk min male");
		auto bulkMaxIter = self->raceData->fdata.find("bulk max male");
		bulkMin = bulkMinIter != self->raceData->fdata.end() ? bulkMinIter->second : settings._bulkStatMinMale;
		bulkMax = bulkMaxIter != self->raceData->fdata.end() ? bulkMaxIter->second : settings._bulkStatMaxMale;
	}
	float bulkStat = std::max(std::max(self->me->stats->strengthBase(), self->me->stats->armourSmith), self->me->stats->weaponSmith);
	bulkStat = std::min(std::max(bulkStat, bulkMin), bulkMax);
	self->bodyBulk = linear(bulkStat * 0.01f, 0.2f, 0.99f);

	float muscularStat = std::max(self->me->stats->_dexterity * self->me->stats->ageMult, self->me->stats->unarmed);
	float muscularMax = self->isFemale ? 0.99f : 0.9f;
	float muscular = linear(((self->me->stats->swimming + self->me->stats->_athletics + muscularStat * 3.0f) - (self->me->stats->cooking + self->me->stats->science)) * 0.01f / 5.0f, 0.2f, muscularMax);
	self->muscular = std::min(std::max(muscular, 0.0f), 1.0f);

	float hung = std::min(std::max(self->me->medical.hunger - 1.0f, 0.0f), 1.0f);
	self->hunger = std::min(std::max((1.0f - hung) * 1.5f, 0.0f), 1.0f);
}

void KEP::AppearanceExtension::AppearanceManager_FUN_0006F8F0_hook(AppearanceManager* self, GameData* bodydata, Character* character)
{
	if (!settings._idleStanceExtension)
	{
		AppearanceManager_FUN_0006F8F0_orig(self, bodydata, character);
		return;
	}

	if (character->isPlayerCharacter())
		return;

	if (character->isAnimal() != nullptr)
	{
		int32_t count = static_cast<int32_t>(character->animation->animList->idleAnims.size());
		if (0 < count)
		{
			if (1 < count)
				bodydata->sdata["idle stance"] = character->animation->animList->idleAnims[UtilityT::randomInt(0, count - 1)]->name;
			else
				bodydata->sdata["idle stance"] = character->animation->animList->idleAnims[0]->name;
		}
		else
		{
			bodydata->sdata["idle stance"] = "";
		}
	}
	else
	{
		auto& idleStanceList = IdleStanceList::getSingleton();
		auto raceListIt = idleStanceList.getRaceIdleStanceList().find(character->getRace()->data);
		if (raceListIt != idleStanceList.getRaceIdleStanceList().end())
		{
			auto& raceIdleStanceList = raceListIt->second;
			if (0 < raceIdleStanceList.size())
			{
				FitnessSelector<GameData*> fitIdleStance;

				auto factionListIt = idleStanceList.getFactionIdleStanceList().find(character->getFaction());
				if (factionListIt != idleStanceList.getFactionIdleStanceList().end() && factionListIt->second.size() != 0)
				{
					for (auto iter = raceIdleStanceList.begin(); iter != raceIdleStanceList.end(); ++iter)
					{
						auto factionStanceIter = factionListIt->second.find(iter->first);
						if (factionStanceIter != factionListIt->second.end())
							fitIdleStance.addItem(factionStanceIter->first, static_cast<float>(factionStanceIter->second));
					}
				}
				else
				{
					for (auto iter = raceIdleStanceList.begin(); iter != raceIdleStanceList.end(); ++iter)
					{
						fitIdleStance.addItem(iter->first, static_cast<float>(iter->second));
					}
				}

				auto choiceGameData = fitIdleStance.chooseAnItem();
				if (choiceGameData != nullptr)
				{
					bodydata->sdata["idle stance"] = choiceGameData->name;
					return;
				}
			}
		}

		std::string idleStanceName;
		lektor<std::string> list;
		list.push_back("idle_stand_timid");
		list.push_back("idle_stand_stiff");
		list.push_back("idle_stand_relax");
		list.push_back("idle_stand_normal");
		list.push_back("idle_stand_guard");
		list.push_back("idle_stand_confident");
		if (list.size() != 0)
		{
			if (list.size() != 1)
				idleStanceName = list[UtilityT::randomInt(0, static_cast<int32_t>(list.size()) - 1)];
			else
				idleStanceName = list[0];
		}
		else
		{
			idleStanceName = "";
		}

		auto personality = character->getStateBroadcast()->personality;
		if (personality == PT_BRAVE)
			idleStanceName = "idle_stand_guard";
		else if (personality == PT_FEARFUL)
			idleStanceName = "idle_stand_timid";
		else
			idleStanceName = "idle_stand_normal";

		bodydata->sdata["idle stance"] = idleStanceName;
	}
}

PortraitManager* KEP::AppearanceExtension::FUN_00413FE0_hook(PortraitManager* self)
{
	FUN_00413FE0_orig(self);

	self->imageSize.x = 0x1000;
	self->imageSize.y = 0x1000;

	self->numWidth = static_cast<uint16_t>(self->imageSize.x / self->portraitSize.x);

	if (self->usedSlots != nullptr)
		delete[] self->usedSlots;
	self->usedSlots = new bool[self->numWidth * self->numWidth];

	return self;
}

void KEP::AppearanceExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_0052B1E0, &AppearanceHuman_FUN_0052B1E0_hook, &AppearanceHuman_FUN_0052B1E0_orig))
		ErrorLog("KenshiExtensionPlugin: [limit bulk] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_0006F8F0, &AppearanceManager_FUN_0006F8F0_hook, &AppearanceManager_FUN_0006F8F0_orig))
		ErrorLog("KenshiExtensionPlugin: [idle stance extension] could not install hook!");

	if (settings._portraitExtension > 0)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_00413FE0, &FUN_00413FE0_hook, &FUN_00413FE0_orig))
			ErrorLog("KenshiExtensionPlugin: [portrait extension] could not install hook!");
	}
}
