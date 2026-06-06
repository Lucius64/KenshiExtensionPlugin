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
#include <kenshi/Town.h>
#include <kenshi/FactionWarMgr.h>

#include <extern/UniqueNPCManager.h>
#include <extern/FactionWarMgr.h>
#include <extern/Blackboard.h>
#include <extern/AreaBiomeGroup.h>

#include <kep/functions.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <bugfix/UniqueCharacterFix.h>

namespace
{
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

	RootObject* (*RootObjectFactory_createRandomCharacter_orig)(RootObjectFactory*, Faction*, Ogre::Vector3&, RootObjectContainer*, GameData*, Building*, float);
	RootObject* RootObjectFactory_createRandomCharacter_hook(RootObjectFactory* self, Faction* faction, Ogre::Vector3 position, RootObjectContainer* owner, GameData* characterTemplate, Building* home, float age)
	{
		if (!KEP::settings._fixspawningOfUniqueCharacters)
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
			auto& uniqueNPCMgr = KEP::externalFunctions->FUN_00354500();

			if (KEP::externalFunctions->FUN_00591720(uniqueNPCMgr, characterTemplate))
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

				auto platoon = (uniqueNPCMgr.*KEP::externalFunctions->FUN_009AFCA0)(characterTemplate).getPlatoon();
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

					if (KEP::externalFunctions->FUN_00591720(uniqueNPCMgr, replacementCharacterTemplate)) // unique replacement spawnもUniqueCharacterStateをチェックするようにする
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
						platoon = (uniqueNPCMgr.*KEP::externalFunctions->FUN_009AFCA0)(replacementCharacterTemplate).getPlatoon();
						if (platoon != nullptr &&
							owner->getType() == DataObjectContainer::TYPE_PLATOON &&
							reinterpret_cast<ActivePlatoon*>(owner)->me != nullptr &&
							platoon->getOwnerships()->_homeTown == reinterpret_cast<ActivePlatoon*>(owner)->me->getOwnerships()->_homeTown || KEP::externalFunctions->FUN_005E7D60(uniqueNPCMgr, replacementCharacterTemplate) != ALIVE)
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

	Platoon* (*RootObjectFactory_createRandomSquad_hook_orig)(RootObjectFactory*, Faction*, Ogre::Vector3&, TownBase*, int, Building*, GameData*, RootObjectContainer*, AreaBiomeGroup*, Platoon*, bool, const hand&, TownBase*, float, SquadType, bool);
	Platoon* RootObjectFactory_createRandomSquad_hook(
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
		if ((KEP::settings._fixspawningOfUniqueCharacters || KEP::settings._fixHousemateInventoryRefresh) && ownr != nullptr && ownr->getType() == DataObjectContainer::TYPE_PLATOON)
		{
			ActivePlatoon* activePlatoon = reinterpret_cast<ActivePlatoon*>(ownr);
			bool refreshed = activePlatoon->me->messageOnActivation == CM_REFRESH;
			if (KEP::settings._fixspawningOfUniqueCharacters && refreshed)
			{
				auto& uniqueNPCMgr = KEP::externalFunctions->FUN_00354500();
				UniqueNPCManager_removeStateForPlatoon(uniqueNPCMgr, activePlatoon->me, false); // 部隊を再生成する場合は事前にUniqueCharacterStateを削除する
			}

			if (KEP::settings._fixHousemateInventoryRefresh && home != nullptr)
			{
				hand previousResident = home->residentSquad;
				auto previousResidentPlatoon = previousResident.getPlatoon();

				auto platoon = RootObjectFactory_createRandomSquad_hook_orig(self, faction, position, homeTown, maxnum, home, squad, ownr, maparea, _activePlatoon, permanentsquad, AItarget, targetTown, sizeMultiplier, squadType, isJustARefresh);
				if (previousResident == home->residentSquad || previousResidentPlatoon == nullptr)
					return platoon;

				// residentsとインベントリを復元
				if (refreshed || previousResidentPlatoon->activePlatoon != nullptr && previousResidentPlatoon->activePlatoon->things.size() != 0 && (activePlatoon->things.size() == 0 || home->residentSquad.container < previousResident.container))
				{
					home->residentSquad.setNull();
					home->setResidentSquad(previousResidentPlatoon);

					if (previousResidentPlatoon->activePlatoon != nullptr && KEP::functions->VendorListManager_hasVendor(previousResidentPlatoon->activePlatoon))
					{
						auto residentsLeader = previousResidentPlatoon->getSquadLeader();
						if (residentsLeader != nullptr)
						{
							auto trader = KEP::functions->InventoryManager_getSingleton();
							KEP::functions->InventoryManager_refreshBuildingInventory(trader, home, previousResidentPlatoon->squadTemplate, previousResidentPlatoon, true, false);
						}
					}
				}

				return platoon;
			}
		}

		return RootObjectFactory_createRandomSquad_hook_orig(self, faction, position, homeTown, maxnum, home, squad, ownr, maparea, _activePlatoon, permanentsquad, AItarget, targetTown, sizeMultiplier, squadType, isJustARefresh);
	}

	YesNoMaybe* (*ActivePlatoon_setupCheck_orig)(ActivePlatoon*, YesNoMaybe*);
	YesNoMaybe* ActivePlatoon_setupCheck_hook(ActivePlatoon* self, YesNoMaybe* _rvo)
	{
		ActivePlatoon_setupCheck_orig(self, _rvo);
		if (KEP::settings._fixspawningOfUniqueCharacters && _rvo->key == YesNoMaybe::NO)
		{
			auto& uniqueNPCMgr = KEP::externalFunctions->FUN_00354500();
			UniqueNPCManager_removeStateForPlatoon(uniqueNPCMgr, self->me, true); // 一部のキャラクターは街のオーバーライドでUniqueCharacterStateが削除されないので確実に削除する
		}
		return _rvo;
	}

	void (*FactionWarMgr_getAllTheForces_orig)(FactionWarMgr*, RootObjectBase*, CampaignInstance*);
	void FactionWarMgr_getAllTheForces_hook(FactionWarMgr* self, RootObjectBase* target, CampaignInstance* campaign)
	{
		if (!KEP::settings._fixSpecialLeader)
		{
			FactionWarMgr_getAllTheForces_orig(self, target, campaign);
			return;
		}

		auto targetTownPos = target->getPosition();
		auto nearlyTowns = KEP::externalFunctions->FUN_009429B0(KEP::externalGlobals->_UtilityT, targetTownPos, self->myTowns, 4);
		auto& uniqueNPCMgr = KEP::externalFunctions->FUN_00354500();

		const std::string refKey = "leader";

		for (auto leaderIter = campaign->campaginData->specialLeaders.begin(); leaderIter != campaign->campaginData->specialLeaders.end(); ++leaderIter)
		{
			hand uniqueNPCHandle = (uniqueNPCMgr.*KEP::externalFunctions->FUN_009AFCA0)(*leaderIter);
			if (uniqueNPCHandle.type != NULL_ITEM)
			{
				if (KEP::externalFunctions->FUN_005E7D60(uniqueNPCMgr, *leaderIter) == ALIVE) // 生存以外またはイベント発動元と別勢力の場合は選出しない
				{
					auto platoon = uniqueNPCHandle.getPlatoon();
					if (platoon != nullptr && platoon->owner == self->me)
					{
						campaign->vfunc0x70(platoon, true);
						break;
					}
				}
			}
			else
			{
				lektor<GameData*> squads;
				ou->gamedata.findAllDataThatReferencesThis(squads, *leaderIter, SQUAD_TEMPLATE, refKey);

				if (squads.size() != 0 && KEP::externalFunctions->FUN_00591720(uniqueNPCMgr, *leaderIter))
				{
					TownBase* homeTown = nullptr;
					auto spawnPos = target->getPosition();

					if (nearlyTowns->size() == 0)
					{
						auto sector = KEP::functions->AreaManager_getAreaSector(KEP::functions->getLevelManager()->areaMgr, spawnPos);
						if (sector != nullptr)
							KEP::externalFunctions->FUN_008F4200(sector, spawnPos, true, self->me, 1.3f, 0.001f);
					}
					else
					{
						homeTown = nearlyTowns->at(0);
						spawnPos = homeTown->getPositionOutsideTownGates(700.0f);
					}

					hand handle;
					auto plat = ou->theFactory->createRandomSquad(self->me, spawnPos, homeTown, 999, nullptr, squads[0], nullptr, nullptr, nullptr, true, handle, nullptr, 1.0f, SQ_ROAMING, false);
					campaign->vfunc0x70(plat, false);
					break;
				}
			}
		}

		if (campaign->numForces <= campaign->numForcesMin)
		{
			for (auto platoonIter = self->me->activePlatoons.begin(); platoonIter != self->me->activePlatoons.end(); ++platoonIter)
			{
				if (KEP::externalFunctions->FUN_009C3510(*platoonIter, campaign, nullptr))
				{
					campaign->vfunc0x70(*platoonIter, false);
					if (campaign->numForcesMin < campaign->numForces) return;
				}
			}

			for (auto tonwIter = nearlyTowns->begin(); tonwIter != nearlyTowns->end(); ++tonwIter)
			{
				for (auto platoonsIter = self->forces.begin(); platoonsIter != self->forces.end(); ++platoonsIter)
				{
					if (platoonsIter->second == nullptr && (KEP::externalFunctions->FUN_009C3510(platoonsIter->first, campaign, *tonwIter)))
						campaign->vfunc0x70(platoonsIter->first, false);

					if (campaign->numForcesMin < campaign->numForces)
						return;
				}

				if (campaign->numForcesMin < campaign->numForces || (self->_generateForcesForTown(*tonwIter, campaign), campaign->numForcesMin < campaign->numForces))
					break;
			}
		}
	}

	bool (*CampaignData_FUN_009C4000_orig)(CampaignData*);
	bool CampaignData_FUN_009C4000_hook(CampaignData* self)
	{
		if (!KEP::settings._fixSpecialLeader)
			return CampaignData_FUN_009C4000_orig(self);

		if (self->specialLeaders.size() != 0)
		{
			auto& uniqueNPCMgr = KEP::externalFunctions->FUN_00354500();
			bool isAllUnavailable = true;

			for (auto leaderIter = self->specialLeaders.begin(); leaderIter != self->specialLeaders.end(); ++leaderIter)
			{
				hand uniqueNPCHandle = (uniqueNPCMgr.*KEP::externalFunctions->FUN_009AFCA0)(*leaderIter);
				if (uniqueNPCHandle.type != NULL_ITEM)
				{
					auto platoon = uniqueNPCHandle.getPlatoon();
					if (platoon != nullptr && platoon->owner != self->owner) // イベント発動元と別勢力として出現済みの場合は生存チェックをスキップ
						continue;
				}
				if (KEP::externalFunctions->FUN_005E7D60(uniqueNPCMgr, *leaderIter) == ALIVE)
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

	void (*ActivePlatoon__checkForUniqueCharactersOnUnload_orig)(ActivePlatoon*);
	void ActivePlatoon__checkForUniqueCharactersOnUnload_hook(ActivePlatoon* self)
	{
		if (KEP::settings._fixUnloadingOfUniqueCharacters)
		{
			auto platoon = self->me;
			auto campaign = platoon->owner->warMgr->getCurrentCampaign(platoon);
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

	void (*UniqueNPCManager_FUN_009A93A0_orig)(UniqueNPCManager*, GameData*, bool);
	void UniqueNPCManager_FUN_009A93A0_hook(UniqueNPCManager* self, GameData* savedata, bool isImport)
	{
		if (!KEP::settings._fixImportingOfUniquePrisoners)
		{
			UniqueNPCManager_FUN_009A93A0_orig(self, savedata, isImport);
			return;
		}

		KEP::externalFunctions->FUN_009A7AA0(*self);

		if (savedata == nullptr)
			return;

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
			if (usedUniques == nullptr)
				continue;

			key = "usedUniquesState" + index;
			auto usedUniquesState = savedata->idata[key];

			if (isImport && usedUniquesState == 1)
				continue;

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
				if (uniqueState.handle == *KEP::externalGlobals->_hand_01E395F8)
				{
					uniqueState.characterTemplate = nullptr;
				}
				else if (isDefaultPrisoner)
				{
					auto iter = platoonMap.find(std::make_pair(uniqueState.handle.container, uniqueState.handle.containerSerial));
					if (iter != platoonMap.end())
					{
						auto gamedata = iter->second;
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
								DebugLog(usedUniques->name + "(" + usedUniques->stringID + ") has not changed since the initial state, so the state was not imported.");
							}
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
		ErrorLog("[RootObjectFactory::createRandomCharacter] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&RootObjectFactory::createRandomSquad), &RootObjectFactory_createRandomSquad_hook, &RootObjectFactory_createRandomSquad_hook_orig))
		ErrorLog("[RootObjectFactory::createRandomSquad] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&ActivePlatoon::setupCheck), &ActivePlatoon_setupCheck_hook, &ActivePlatoon_setupCheck_orig))
		ErrorLog("[ActivePlatoon::setupCheck] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&ActivePlatoon::_checkForUniqueCharactersOnUnload), &ActivePlatoon__checkForUniqueCharactersOnUnload_hook, &ActivePlatoon__checkForUniqueCharactersOnUnload_orig))
		ErrorLog("[ActivePlatoon::_checkForUniqueCharactersOnUnload] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_009A93A0, &UniqueNPCManager_FUN_009A93A0_hook, &UniqueNPCManager_FUN_009A93A0_orig))
		ErrorLog("[FUN_009A93A0] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&FactionWarMgr::getAllTheForces), &FactionWarMgr_getAllTheForces_hook, &FactionWarMgr_getAllTheForces_orig))
		ErrorLog("[FactionWarMgr::getAllTheForces] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_009C4000, &CampaignData_FUN_009C4000_hook, &CampaignData_FUN_009C4000_orig))
		ErrorLog("[FUN_009C4000] could not install hook!");
}
