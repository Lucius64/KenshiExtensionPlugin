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
#include <kenshi/AppearanceManager.h>
#include <kenshi/Appearance.h>
#include <kenshi/gui/PortraitManager.h>
#include <kenshi/Animation/AnimationClass.h>
#include <kenshi/StateBroadcastData.h>

#include <extern/AppearanceBase.h>

#include <UtilityFunction.h>
#include <Settings.h>
#include <AppearanceExtension.h>

namespace
{
	class IdleStanceList
	{
	public:
		const ogre_unordered_map<GameData*, Ogre::FastArray<std::pair<GameData*, int> > >::type& getRaceIdleStanceList() const { return _raceIdleStanceList; };
		const ogre_unordered_map<Faction*, ogre_unordered_map<GameData*, int>::type>::type& getFactionIdleStanceList() const { return _factionIdleStanceList; };

		static const IdleStanceList& getSingleton();
	private:
		IdleStanceList();

		ogre_unordered_map<GameData*, Ogre::FastArray<std::pair<GameData*, int> > >::type _raceIdleStanceList;
		ogre_unordered_map<Faction*, ogre_unordered_map<GameData*, int>::type>::type _factionIdleStanceList;
	};

	const IdleStanceList& IdleStanceList::getSingleton()
	{
		static IdleStanceList instance;
		return instance;
	}

	IdleStanceList::IdleStanceList()
	{
		auto appearanceManager = AppearanceManager::getInstance();

		ogre_unordered_set<std::string>::type animNameSet;
		for (auto iter = appearanceManager->characterIdleAnimations.begin(); iter != appearanceManager->characterIdleAnimations.end(); ++iter)
		{
			animNameSet.emplace((*iter)->dataName);
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

	void (*AppearanceHuman_updatePhysiqueMuscleMults_orig)(AppearanceHuman*);
	void AppearanceHuman_updatePhysiqueMuscleMults_hook(AppearanceHuman* self)
	{
		if (!KEP::settings._bulkLimitsExtension)
		{
			AppearanceHuman_updatePhysiqueMuscleMults_orig(self);
			return;
		}

		if (self->body == nullptr)
			return;

		float bulkMin;
		float bulkMax;
		if (self->female)
		{
			auto bulkMinIter = self->raceData->fdata.find("bulk min female");
			auto bulkMaxIter = self->raceData->fdata.find("bulk max female");
			bulkMin = bulkMinIter != self->raceData->fdata.end() ? bulkMinIter->second : KEP::settings._bulkStatMinFemale;
			bulkMax = bulkMaxIter != self->raceData->fdata.end() ? bulkMaxIter->second : KEP::settings._bulkStatMaxFemale;
		}
		else
		{
			auto bulkMinIter = self->raceData->fdata.find("bulk min male");
			auto bulkMaxIter = self->raceData->fdata.find("bulk max male");
			bulkMin = bulkMinIter != self->raceData->fdata.end() ? bulkMinIter->second : KEP::settings._bulkStatMinMale;
			bulkMax = bulkMaxIter != self->raceData->fdata.end() ? bulkMaxIter->second : KEP::settings._bulkStatMaxMale;
		}
		float bulkStat = std::max(std::max(self->me->stats->strengthBase(), self->me->stats->armourSmith), self->me->stats->weaponSmith);
		bulkStat = std::min(std::max(bulkStat, bulkMin), bulkMax);
		self->bulkMult = KEP::linear(bulkStat * 0.01f, 0.2f, 0.99f);

		float muscularStat = std::max(self->me->stats->_dexterity * self->me->stats->ageMult, self->me->stats->unarmed);
		float muscularMax = self->female ? 0.99f : 0.9f;
		float muscular = KEP::linear(((self->me->stats->swimming + self->me->stats->_athletics + muscularStat * 3.0f) - (self->me->stats->cooking + self->me->stats->science)) * 0.01f / 5.0f, 0.2f, muscularMax);
		self->muscleMult = std::min(std::max(muscular, 0.0f), 1.0f);

		float hung = std::min(std::max(self->me->medical.hunger - 1.0f, 0.0f), 1.0f);
		self->skinnyMult = std::min(std::max((1.0f - hung) * 1.5f, 0.0f), 1.0f);
	}

	void (*AppearanceManager_randomAnimations_orig)(AppearanceManager*, GameData*, Character*);
	void AppearanceManager_randomAnimations_hook(AppearanceManager* self, GameData* bodydata, Character* character)
	{
		if (!KEP::settings._idleStanceExtension)
		{
			AppearanceManager_randomAnimations_orig(self, bodydata, character);
			return;
		}

		if (character->isPlayerCharacter())
			return;

		if (character->isAnimal() != nullptr)
		{
			int32_t count = static_cast<int32_t>(character->animation->myAnimList->idleAnims.size());
			if (0 < count)
			{
				if (1 < count)
					bodydata->sdata["idle stance"] = character->animation->myAnimList->idleAnims[UtilityT::randomInt(0, count - 1)]->dataName;
				else
					bodydata->sdata["idle stance"] = character->animation->myAnimList->idleAnims[0]->dataName;
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

			auto personality = character->getStateBroadcast()->myPersonality;
			if (personality == PT_BRAVE)
				idleStanceName = "idle_stand_guard";
			else if (personality == PT_FEARFUL)
				idleStanceName = "idle_stand_timid";
			else
				idleStanceName = "idle_stand_normal";

			bodydata->sdata["idle stance"] = idleStanceName;
		}
	}

	PortraitManager* (*PortraitManager__CONSTRUCTOR_orig)(PortraitManager*);
	PortraitManager* PortraitManager__CONSTRUCTOR_hook(PortraitManager* self)
	{
		PortraitManager__CONSTRUCTOR_orig(self);

		self->textureSize.x = 0x1000;
		self->textureSize.y = 0x1000;

		self->maxIndex = static_cast<uint16_t>(self->textureSize.x / self->texturePortraitSize.x);

		if (self->imagesIndices != nullptr)
			delete[] self->imagesIndices;
		self->imagesIndices = new bool[self->maxIndex * self->maxIndex];

		return self;
	}
}

void KEP::AppearanceExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&AppearanceHuman::updatePhysiqueMuscleMults), &AppearanceHuman_updatePhysiqueMuscleMults_hook, &AppearanceHuman_updatePhysiqueMuscleMults_orig))
		ErrorLog("[AppearanceHuman::updatePhysiqueMuscleMults] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&AppearanceManager::randomAnimations), &AppearanceManager_randomAnimations_hook, &AppearanceManager_randomAnimations_orig))
		ErrorLog("[AppearanceManager::randomAnimations] could not install hook!");

	if (settings._portraitExtension > 0)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&PortraitManager::_CONSTRUCTOR), &PortraitManager__CONSTRUCTOR_hook, &PortraitManager__CONSTRUCTOR_orig))
			ErrorLog("[PortraitManager::PortraitManager] could not install hook!");
	}
}
