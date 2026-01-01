#include <ogre/OgreStringConverter.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/RootObject.h>
#include <kenshi/RootObjectFactory.h>
#include <kenshi/GameData.h>
#include <kenshi/Faction.h>
#include <kenshi/Character.h>
#include <kenshi/Platoon.h>
#include <kenshi/WorldEventStateQuery.h>

#include <extern/TownBase.h>
#include <extern/UniqueNPCManager.h>
#include <extern/FactionWarMgr.h>
#include <extern/Blackboard.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <bugfix/UniqueCharacterFix.h>


namespace
{
	RootObject* (*RootObjectFactory_createRandomCharacter_orig)(RootObjectFactory*, Faction*, Ogre::Vector3&, RootObjectContainer*, GameData*, Building*, float);
	RootObject* (*RootObjectFactory_createRandomSquad_hook_orig)(RootObjectFactory*, Faction*, Ogre::Vector3&, TownBase*, int, Building*, GameData*, RootObjectContainer*, AreaBiomeGroup*, Platoon*, bool, const hand&, TownBase*, float, SquadType, bool);
	YesNoMaybe* (*ActivePlatoon_setupCheck_orig)(ActivePlatoon*, YesNoMaybe*);
	void (*FactionWarMgr_FUN_009C4170_orig)(FactionWarMgr*, TownBase*, CampaignInstance*);
	bool (*CampaignData_FUN_009C4000_orig)(CampaignData*);
	void (*ActivePlatoon__checkForUniqueCharactersOnUnload_orig)(ActivePlatoon*);
	void (*UniqueNPCManager_FUN_009A93A0_orig)(UniqueNPCManager*, GameData*, bool);

	bool isBeforeTownOverride(Platoon* self)
	{
		if (self->isSeparatedSquad.type != NULL_ITEM || self->ownerships._homeTown == nullptr)
			return false;

		auto town = self->ownerships._homeTown->isTown();
		if (town == nullptr)
			return false;

		if ((self->myBaseHomeTownData == nullptr || self->myBaseHomeTownData == town->data))
			return town->replacementTown != nullptr;
		else
			return self->myBaseHomeTownData != town->replacementTown;
	}

	void UniqueNPCManager_removeStateForPlatoon(UniqueNPCManager& self, Platoon* platoon, bool includeImprisoned)
	{
		if (self.uniqueCharacterStates.size() == 0 || platoon == nullptr)
			return;

		std::vector<GameData*> removeList;
		for (auto it = self.uniqueCharacterStates.begin(); it != self.uniqueCharacterStates.end(); ++it)
		{
			auto& characterState = it->second;
			if ((characterState.state == ALIVE || includeImprisoned && characterState.state == IMPRISONED) &&
				characterState.handle.type != NULL_ITEM &&
				characterState.handle.container == platoon->handle.container &&
				characterState.handle.containerSerial == platoon->handle.containerSerial)
			{
				removeList.push_back(characterState.characterTemplate);
			}
		}

		for (auto iter = removeList.begin(); iter != removeList.end(); ++iter)
		{
			KEP::externalFunctions->FUN_00505D50(self, *iter, platoon->handle);
		}
	}

	void GameDataContainer_getAllPlatoonSaveData(GameDataContainer* self, ogre_unordered_map<std::pair<int, int>, GameData*>::type& out)
	{
		auto& list = self->gamedataCatSID[PLATOON];
		for (auto iter = list.begin(); iter != list.end(); ++iter)
		{
			const auto& key = std::make_pair(iter->second->idata["handleC"], iter->second->idata["handleCS"]);
			out[key] = iter->second;
		}
	}
}

RootObject* KEP::UniqueCharacterFix::RootObjectFactory_createRandomCharacter_hook(RootObjectFactory* self, Faction* faction, Ogre::Vector3 position, RootObjectContainer* owner, GameData* characterTemplate, Building* home, float age)
{
	if (!settings._fixspawningOfUniqueCharacters)
		return RootObjectFactory_createRandomCharacter_orig(self, faction, position, owner, characterTemplate, home, age);

	bool notRandom = true;
	if (characterTemplate == nullptr)
	{
		if (faction->isPlayer == nullptr)
		{
			characterTemplate = ou->gamedata.getDataByName("random character base-copy", CHARACTER);
		}
		else
		{
			characterTemplate = ou->gamedata.getDataByName("random character base", CHARACTER);
			notRandom = false;
		}
	}

	if (characterTemplate->bdata["unique"])
	{
		auto& uniqueNPCMgr = externalFunctions->FUN_00354500();

		if (externalFunctions->FUN_00591720(uniqueNPCMgr, characterTemplate))
		{
			auto& characterState = uniqueNPCMgr.uniqueCharacterStates[characterTemplate];
			characterState.characterTemplate = characterTemplate;
			characterState.handle.type = NULL_ITEM;
			characterState.handle.container = 0;
			characterState.handle.containerSerial = 0;
			characterState.handle.index = 0;
			characterState.handle.serial = 0;
		}
		else
		{
			bool needReplacement = true;

			auto platoon = (uniqueNPCMgr.*externalFunctions->FUN_009AFCA0)(characterTemplate).getPlatoon();
			if (platoon != nullptr && owner->getType() == DataObjectContainer::TYPE_PLATOON && reinterpret_cast<ActivePlatoon*>(owner)->me != platoon && isBeforeTownOverride(platoon)) // 現在の所有者が街のオーバーライドで消滅予定の場合は所有権を奪い取って強制的に出現させる
			{
				auto& characterState = uniqueNPCMgr.uniqueCharacterStates[characterTemplate];
				if (characterState.state == ALIVE || characterState.state == IMPRISONED)
				{
					characterState.characterTemplate = characterTemplate;
					characterState.handle.type = NULL_ITEM;
					characterState.handle.container = 0;
					characterState.handle.containerSerial = 0;
					characterState.handle.index = 0;
					characterState.handle.serial = 0;
					characterState.state = ALIVE;
					characterState.playerInvolvement = false;

					needReplacement = false;
					DebugLog("KenshiExtensionPlugin: " + characterTemplate->name + " was force spawned");
				}
			}

			if (needReplacement)
			{
				auto& replacementStringID = characterTemplate->getFromList("unique replacement spawn", 0);
				auto replacementCharacterTemplate = ou->gamedata.getData(replacementStringID, CHARACTER);

				if (replacementCharacterTemplate == nullptr)
					return nullptr;

				if (externalFunctions->FUN_00591720(uniqueNPCMgr, replacementCharacterTemplate)) // unique replacement spawnもUniqueCharacterStateをチェックするようにする
				{
					auto& characterState = uniqueNPCMgr.uniqueCharacterStates[replacementCharacterTemplate];
					characterState.characterTemplate = replacementCharacterTemplate;
					characterState.handle.type = NULL_ITEM;
					characterState.handle.container = 0;
					characterState.handle.containerSerial = 0;
					characterState.handle.index = 0;
					characterState.handle.serial = 0;
				}
				else if (characterTemplate != replacementCharacterTemplate)
				{
					return nullptr;
				}
				else // unique replacement spawnに自分自身を設定した場合は生存状態かつ別の街なら出現を許容する
				{
					platoon = (uniqueNPCMgr.*externalFunctions->FUN_009AFCA0)(replacementCharacterTemplate).getPlatoon();
					if (platoon != nullptr &&
						owner->getType() == DataObjectContainer::TYPE_PLATOON &&
						reinterpret_cast<ActivePlatoon*>(owner)->me != nullptr &&
						platoon->getOwnerships()->_homeTown == reinterpret_cast<ActivePlatoon*>(owner)->me->getOwnerships()->_homeTown || externalFunctions->FUN_005E7D60(uniqueNPCMgr, replacementCharacterTemplate) != ALIVE)
						return nullptr;
				}

				characterTemplate = replacementCharacterTemplate;
			}
		}
	}

	auto character = reinterpret_cast<Character*>(self->create(characterTemplate, position, false, faction, Ogre::Quaternion::IDENTITY, nullptr, owner, nullptr, false, home, age));

	if (character != nullptr)
	{
		if (notRandom)
			character->setStandingOrder(MessageForB::M_SET_ORDER_FAR, true);

		if (faction->isPlayer != nullptr)
			character->setStandingOrder(MessageForB::M_SET_ORDER_RUN, true);
	}

	return character;
}

RootObject* KEP::UniqueCharacterFix::RootObjectFactory_createRandomSquad_hook(
	RootObjectFactory* self,
	Faction* faction,
	Ogre::Vector3 position,
	TownBase* homeTown,
	int maxnum,
	Building* home,
	GameData* squad,
	RootObjectContainer* ownr,
	AreaBiomeGroup* maparea,
	Platoon* _activePlatoon,
	bool permanentsquad,
	const hand& AItarget,
	TownBase* targetTown,
	float sizeMultiplier,
	SquadType squadType,
	bool isJustARefresh
)
{
	if (settings._fixspawningOfUniqueCharacters && ownr != nullptr && ownr->getType() == DataObjectContainer::TYPE_PLATOON)
	{
		auto& uniqueNPCMgr = externalFunctions->FUN_00354500();
		UniqueNPCManager_removeStateForPlatoon(uniqueNPCMgr, reinterpret_cast<ActivePlatoon*>(ownr)->me, false); // 部隊を再生成する場合は事前にUniqueCharacterStateを削除する
	}

	return RootObjectFactory_createRandomSquad_hook_orig(self, faction, position, homeTown, maxnum, home, squad, ownr, maparea, _activePlatoon, permanentsquad, AItarget, targetTown, sizeMultiplier, squadType, isJustARefresh);
}

YesNoMaybe* KEP::UniqueCharacterFix::ActivePlatoon_setupCheck_hook(ActivePlatoon* self, YesNoMaybe* _rvo)
{
	ActivePlatoon_setupCheck_orig(self, _rvo);
	if (settings._fixspawningOfUniqueCharacters && _rvo->key == YesNoMaybe::NO)
	{
		auto& uniqueNPCMgr = externalFunctions->FUN_00354500();
		UniqueNPCManager_removeStateForPlatoon(uniqueNPCMgr, self->me, true); // 一部のキャラクターは街のオーバーライドでUniqueCharacterStateが削除されないので確実に削除する
	}
	return _rvo;
}

void KEP::UniqueCharacterFix::FactionWarMgr_FUN_009C4170_hook(FactionWarMgr* self, TownBase* dstTown, CampaignInstance* campaignInstance)
{
	if (!settings._fixSpecialLeader)
	{
		FactionWarMgr_FUN_009C4170_orig(self, dstTown, campaignInstance);
		return;
	}

	auto targetTownPos = dstTown->getPosition();
	auto nearlyTowns = externalFunctions->FUN_009429B0(externalGlobals->_UtilityT, targetTownPos, self->myTowns, 4);
	auto& uniqueNPCMgr = externalFunctions->FUN_00354500();

	const std::string refKey = "leader";

	for (auto leaderIter = campaignInstance->campaginData->specialLeaders.begin(); leaderIter != campaignInstance->campaginData->specialLeaders.end(); ++leaderIter)
	{
		hand uniqueNPCHandle = (uniqueNPCMgr.*externalFunctions->FUN_009AFCA0)(*leaderIter);
		if (uniqueNPCHandle.type != NULL_ITEM)
		{
			if (externalFunctions->FUN_005E7D60(uniqueNPCMgr, *leaderIter) == ALIVE) // 生存以外またはイベント発動元と別勢力の場合は選出しない
			{
				auto platoon = uniqueNPCHandle.getPlatoon();
				if (platoon != nullptr && platoon->owner == self->owner)
				{
					campaignInstance->vfunc0x70(platoon, true);
					break;
				}
			}
		}
		else
		{
			lektor<GameData*> squads;
			ou->gamedata.findAllDataThatReferencesThis(squads, *leaderIter, SQUAD_TEMPLATE, refKey);

			if (squads.size() != 0 && externalFunctions->FUN_00591720(uniqueNPCMgr, *leaderIter))
			{
				TownBase* homeTown = nullptr;
				auto spawnPos = dstTown->getPosition();

				if (nearlyTowns->size() == 0)
				{
					auto sector = externalFunctions->FUN_008F47E0(externalGlobals->_CLASS_02133098->_0x0, spawnPos);
					if (sector != nullptr)
						externalFunctions->FUN_008F4200(sector, spawnPos, true, self->owner, 1.3f, 0.001f);
				}
				else
				{
					homeTown = nearlyTowns->at(0);
					spawnPos = homeTown->getGatewayDoorPosition(700.0f);
				}

				hand handle;
				auto plat = ou->theFactory->createRandomSquad(self->owner, spawnPos, homeTown, 999, nullptr, squads[0], nullptr, nullptr, nullptr, true, handle, nullptr, 1.0f, SQ_ROAMING, false);
				campaignInstance->vfunc0x70(plat, false);
				break;
			}
		}
	}

	if (campaignInstance->numForces <= campaignInstance->numForcesMin)
	{
		for (auto platoonIter = self->owner->activePlatoons.begin(); platoonIter != self->owner->activePlatoons.end(); ++platoonIter)
		{
			if (externalFunctions->FUN_009C3510(*platoonIter, campaignInstance, nullptr))
			{
				campaignInstance->vfunc0x70(*platoonIter, false);
				if (campaignInstance->numForcesMin < campaignInstance->numForces) return;
			}
		}

		for (auto tonwIter = nearlyTowns->begin(); tonwIter != nearlyTowns->end(); ++tonwIter)
		{
			for (auto platoonsIter = self->platoons.begin(); platoonsIter != self->platoons.end(); ++platoonsIter)
			{
				if (platoonsIter->second == nullptr && (externalFunctions->FUN_009C3510(platoonsIter->first, campaignInstance, *tonwIter)))
					campaignInstance->vfunc0x70(platoonsIter->first, false);

				if (campaignInstance->numForcesMin < campaignInstance->numForces)
					return;
			}

			if (campaignInstance->numForcesMin < campaignInstance->numForces || (externalFunctions->FUN_009C3280(self, *tonwIter, campaignInstance), campaignInstance->numForcesMin < campaignInstance->numForces))
				break;
		}
	}
}

bool KEP::UniqueCharacterFix::CampaignData_FUN_009C4000_hook(CampaignData* self)
{
	if (!settings._fixSpecialLeader)
		return CampaignData_FUN_009C4000_orig(self);

	if (self->specialLeaders.size() != 0)
	{
		auto& uniqueNPCMgr = externalFunctions->FUN_00354500();
		bool isAllUnavailable = true;

		for (auto leaderIter = self->specialLeaders.begin(); leaderIter != self->specialLeaders.end(); ++leaderIter)
		{
			hand uniqueNPCHandle = (uniqueNPCMgr.*externalFunctions->FUN_009AFCA0)(*leaderIter);
			if (uniqueNPCHandle.type != NULL_ITEM)
			{
				auto platoon = uniqueNPCHandle.getPlatoon();
				if (platoon != nullptr && platoon->owner != self->owner) // イベント発動元と別勢力として出現済みの場合は生存チェックをスキップ
					continue;
			}
			if (externalFunctions->FUN_005E7D60(uniqueNPCMgr, *leaderIter) == ALIVE)
			{
				isAllUnavailable = false;
				break;
			}
		}
		if (isAllUnavailable)
			return false;
	}

	return self->worldState.isTrue();
}

void KEP::UniqueCharacterFix::ActivePlatoon__checkForUniqueCharactersOnUnload_hook(ActivePlatoon* self)
{
	if (settings._fixUnloadingOfUniqueCharacters)
	{
		auto platoon = self->me;
		auto campaign = externalFunctions->FUN_002834C0(platoon->owner->warMgr, platoon);
		if (campaign != nullptr && campaign->phase != 2) // 撤退フェーズに移行するまでは初期化判定の対象外とする
		{
			return;
		}
		else if (platoon->blackboard->contractJob != nullptr) // プレイヤーと契約中は初期化判定の対象外とする
		{
			auto rootObject = platoon->blackboard->contractJob->missionEmployer.getRootObject();
			if (rootObject != nullptr)
			{
				auto faction = rootObject->getFaction();
				if (faction != nullptr && faction->isPlayer != nullptr)
					return;
			}
		}
		else if (self->things.size() == 1) // プレイヤーに担がれている間は初期化判定の対象外とする
		{
			auto character = reinterpret_cast<Character*>(self->things[0]);
			if (character->_isBeingCarried)
			{
				auto carryingCharacter = character->carryingObject.getCharacter();
				if (carryingCharacter != nullptr && carryingCharacter->isPlayerCharacter())
					return;
			}
		}
	}
	
	ActivePlatoon__checkForUniqueCharactersOnUnload_orig(self);
}

void KEP::UniqueCharacterFix::UniqueNPCManager_FUN_009A93A0_hook(UniqueNPCManager* self, GameData* savedata, bool isImport)
{
	if (!settings._fixImportingOfUniquePrisoners)
	{
		UniqueNPCManager_FUN_009A93A0_orig(self, savedata, isImport);
		return;
	}

	externalFunctions->FUN_009A7AA0(*self);

	if (savedata == nullptr) return;

	ogre_unordered_map<std::pair<int, int>, GameData*>::type platoonMap;
	if (isImport)
		GameDataContainer_getAllPlatoonSaveData(&ou->savedata, platoonMap);

	std::string key = "num usedUniques";
	auto usedUniqueNum = savedata->idata[key];

	for (int i = 0; i < usedUniqueNum; ++i)
	{
		auto index = Ogre::StringConverter::toString(i);

		key = "usedUniques" + index;

		auto& characterSID = savedata->sdata[key];

		auto usedUniques = ou->gamedata.getData(characterSID);
		if (usedUniques == nullptr) continue;

		key = "usedUniquesState" + index;
		auto usedUniquesState = savedata->idata[key];

		if (isImport && usedUniquesState == 1) continue;

		auto& uniqueState = self->uniqueCharacterStates[usedUniques];
		uniqueState.characterTemplate = usedUniques;

		bool isDefaultPrisoner = uniqueState.state == IMPRISONED;

		key = "usedUniquesPlayer" + index;
		uniqueState.playerInvolvement = savedata->bdata[key];
		uniqueState.state = static_cast<WorldStateEnum>(usedUniquesState);

		key = "usedUniques" + index + "handle";
		savedata->getHandle(uniqueState.handle, key);

		if (isImport && !uniqueState.playerInvolvement && uniqueState.state == IMPRISONED)
		{
			if (uniqueState.handle == *externalGlobals->_hand_01E395F8)
			{
				uniqueState.characterTemplate = nullptr;
			}
			else if (isDefaultPrisoner)
			{
				auto it = platoonMap.find(std::make_pair(uniqueState.handle.container, uniqueState.handle.containerSerial));
				if (it != platoonMap.end())
				{
					auto gamedata = it->second;
					hand platoonHandle;
					gamedata->getHandle(platoonHandle, "handle");

					if (uniqueState.handle.container == platoonHandle.container && uniqueState.handle.containerSerial == platoonHandle.containerSerial && platoonHandle.type == PLATOON)
					{
						hand currentTownHandle;
						hand homeTownHandle;
						gamedata->getHandle(currentTownHandle, "currenttown");
						gamedata->getHandle(homeTownHandle, "hometown");
						if (currentTownHandle == homeTownHandle) // デフォルトで囚人状態かつ出現した街から移動していないキャラクターは再出現可能とする
						{
							uniqueState.characterTemplate = nullptr;
							uniqueState.handle.type = NULL_ITEM;
							uniqueState.handle.container = 0;
							uniqueState.handle.containerSerial = 0;
							uniqueState.handle.index = 0;
							uniqueState.handle.serial = 0;
						}
					}
				}
			}
		}
	}
}

void KEP::UniqueCharacterFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&RootObjectFactory::createRandomCharacter), &RootObjectFactory_createRandomCharacter_hook, &RootObjectFactory_createRandomCharacter_orig))
		ErrorLog("KenshiExtensionPlugin: [spawn unique NPC pt1] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&RootObjectFactory::createRandomSquad), &RootObjectFactory_createRandomSquad_hook, &RootObjectFactory_createRandomSquad_hook_orig))
		ErrorLog("KenshiExtensionPlugin: [spawn unique NPC pt2] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&ActivePlatoon::setupCheck), &ActivePlatoon_setupCheck_hook, &ActivePlatoon_setupCheck_orig))
		ErrorLog("KenshiExtensionPlugin: [spawn unique NPC pt3] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&ActivePlatoon::_checkForUniqueCharactersOnUnload), &ActivePlatoon__checkForUniqueCharactersOnUnload_hook, &ActivePlatoon__checkForUniqueCharactersOnUnload_orig))
		ErrorLog("KenshiExtensionPlugin: [unload unique NPC] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_009A93A0, &UniqueNPCManager_FUN_009A93A0_hook, &UniqueNPCManager_FUN_009A93A0_orig))
		ErrorLog("KenshiExtensionPlugin: [import prisoner] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_009C4170, &FactionWarMgr_FUN_009C4170_hook, &FactionWarMgr_FUN_009C4170_orig))
		ErrorLog("KenshiExtensionPlugin: [special leader pt1] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_009C4000, &CampaignData_FUN_009C4000_hook, &CampaignData_FUN_009C4000_orig))
		ErrorLog("KenshiExtensionPlugin: [special leader pt2] could not install hook!");
}
