#include <ogre/OgrePrerequisites.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GlobalConstants.h>
#include <kenshi/GameWorld.h>
#include <kenshi/RootObjectFactory.h>
#include <kenshi/Faction.h>
#include <kenshi/Platoon.h>
#include <kenshi/Character.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/Dialogue.h>
#include <kenshi/gui/SquadManagementScreen.h>

#include <extern/AI.h>
#include <extern/Blackboard.h>
#include <extern/StateBroadcastData.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <CharacterExtension.h>


namespace
{
	std::string dismissedCharacterFactionSID = "";

	void (*SquadManagementScreen_dismissCharacter_orig)(SquadManagementScreen*, int);
	void SquadManagementScreen_dismissCharacter_hook(SquadManagementScreen* self, int result)
	{
		if (!KEP::settings._dismissedCharacterExtension)
		{
			SquadManagementScreen_dismissCharacter_orig(self, result);
			return;
		}

		if (result != 2 || self->dismissChar == nullptr)
		{
			self->dismissChar = nullptr;
			return;
		}

		if (dismissedCharacterFactionSID.empty())
		{
			auto iter = con->settings->objectReferences.find("ex player faction");
			if (iter != con->settings->objectReferences.end())
			{
				for (auto reverseIter = iter->second.rbegin(); reverseIter != iter->second.rend(); ++reverseIter)
				{
					auto faction = ou->factionMgr->getFactionByStringID(reverseIter->sid);
					if (faction != nullptr && faction->isPlayer == nullptr)
					{
						dismissedCharacterFactionSID.append(reverseIter->sid);
						break;
					}
				}
			}

			if (dismissedCharacterFactionSID.empty())
				dismissedCharacterFactionSID.append("1233-gamedata.base");
		}

		auto faction = ou->factionMgr->getFactionByStringID(dismissedCharacterFactionSID);
		if (faction == nullptr || faction->isPlayer != nullptr)
			faction = ou->factionMgr->getEmptyFaction();

		if (ou->player->isObjectSelected(self->dismissChar))
			(ou->player->unselectPlayerCharacter)(self->dismissChar);

		self->dismissChar->setFaction(faction, nullptr);

		DebugLog("[ex-player faction] " + faction->name);

		auto squadTemplate = ou->gamedata.getData("5006-squad.mod");
		auto pos = self->dismissChar->getPosition();

		auto platoon = faction->createNewEmptyActivePlatoon(squadTemplate, true, pos);
		platoon->activePlatoon->addActiveObject(self->dismissChar);

		auto blackboard = KEP::externalFunctions->FUN_005065E0(self->dismissChar->ai);
		if (blackboard != nullptr)
		{
			blackboard->_0x130 = true;
			blackboard->leaderTasks.clear();
			blackboard->squad1Tasks.clear();
			blackboard->squad2Tasks.clear();
			blackboard->slaveTasks.clear();
			KEP::externalFunctions->FUN_002715E0(blackboard, squadTemplate);
		}
		KEP::externalFunctions->FUN_005067F0(self->dismissChar->ai->taskSystem);
		KEP::externalFunctions->FUN_0050C720(&self->dismissChar->ai->taskSystem->_0x300);
		KEP::externalFunctions->FUN_004AB930(self->dismissChar->ai->taskSystem);
		KEP::externalFunctions->FUN_0048EA30(self);

		self->dismissChar = nullptr;
		return;
	}

	bool (*Character_giveBirth_orig)(Character*, GameDataCopyStandalone*, const Ogre::Vector3&, const Ogre::Quaternion&, GameSaveState*, ActivePlatoon*, Faction*);
	bool Character_giveBirth_hook(Character* self, GameDataCopyStandalone* appearance, const Ogre::Vector3& position, const Ogre::Quaternion& rotation, GameSaveState* state, ActivePlatoon* tempplatoonptr, Faction* _faction)
	{
		int money = 0;
		bool initialization = false;
		if (KEP::settings._characterExtension && state == nullptr && self->getFaction()->isPlayer == nullptr)
		{
			if (tempplatoonptr != nullptr && tempplatoonptr->me != nullptr)
			{
				initialization = true;
				money = tempplatoonptr->me->ownerships.money;
			}
		}

		bool success = Character_giveBirth_orig(self, appearance, position, rotation, state, tempplatoonptr, _faction);

		if (initialization && success)
		{
			auto ownerShips = self->getOwnerships();
			if (ownerShips != nullptr && ownerShips->money < money)
				ownerShips->money = money;
		}

		if (KEP::settings._dialogueExtension && success && self->isAnimal() != nullptr && !self->platoon->me->squadTemplate->listExistsAndNotEmpty("dialog animal"))
		{
			lektor<std::string> dialogList;
			if (_faction->isPlayer != nullptr)
				self->data->getAllFromList("dialogue package player", dialogList);
			else
				self->data->getAllFromList("dialogue package", dialogList);

			for (auto iter = dialogList.begin(); iter != dialogList.end(); ++iter)
			{
				self->dialogue->addDialoguePackage(ou->gamedata.getData(*iter, DIALOGUE_PACKAGE));
			}
		}

		return success;
	}

	bool (*PlayerInterface_recruit_orig)(PlayerInterface*, const lektor<Character*>&, bool);
	bool PlayerInterface_recruit_hook(PlayerInterface* self, const lektor<Character*>& characters, bool editor)
	{
		bool success = PlayerInterface_recruit_orig(self, characters, editor);
		if ((KEP::settings._dialogueExtension) && success)
		{
			for (auto iter = characters.begin(); iter != characters.end(); ++iter)
			{
				if ((*iter)->isAnimal() != nullptr)
				{
					lektor<std::string> dialogList;
					(*iter)->platoon->me->squadTemplate->getAllFromList("dialog animal", dialogList);
					if (dialogList.size() == 0)
						(*iter)->data->getAllFromList("dialogue package player", dialogList);

					if (dialogList.size() != 0)
					{
						(*iter)->dialogue->clearDialogues();
						for (auto dialogIter = dialogList.begin(); dialogIter != dialogList.end(); ++dialogIter)
						{
							(*iter)->dialogue->addDialoguePackage(ou->gamedata.getData(*dialogIter, DIALOGUE_PACKAGE));
						}
					}
				}
			}
		}
		return success;
	}

	Item* (*Character_generateWeapon_orig)(Character*, GameData*, GameData*);
	Item* Character_generateWeapon_hook(Character* self, GameData* weapon, GameData* manufacturer)
	{
		auto models = self->data->getReferenceListIfExists("weapon models");

		if (manufacturer != nullptr && models != nullptr)
		{
			FitnessSelector<GameData*> selector;
			auto list = manufacturer->getReferenceListIfExists("weapon models");
			if (list != nullptr)
			{
				for (auto modelIter = models->begin(); modelIter != models->end(); ++modelIter)
				{
					for (auto iter = list->begin(); iter != list->end(); ++iter)
					{
						if (iter->sid == modelIter->sid)
							selector.addItem(modelIter->getPtr(&ou->gamedata), static_cast<float>(modelIter->values.value[0]));
					}
				}
			}

			auto materialData = selector.chooseAnItem();
			if (materialData != nullptr)
				return ou->theFactory->createItem(manufacturer, hand(), weapon, materialData, 0, nullptr);
		}

		return Character_generateWeapon_orig(self, weapon, manufacturer);
	}

	void (*Character_reCalculateNaturalWeapon_orig)(Character*);
	void Character_reCalculateNaturalWeapon_hook(Character* self)
	{
		auto weaponMesh = self->data->getFromListAsData("weapon", 0, &ou->gamedata, WEAPON);
		auto gamedata = self->data->getFromListAsData("weapon level", 0, &ou->gamedata, WEAPON_MANUFACTURER);
		auto models = self->data->getReferenceListIfExists("weapon models");

		if (self->naturalWeapon != nullptr)
			ou->destroy((RootObject*)self->naturalWeapon, false, "undefined");

		if (gamedata != nullptr && models != nullptr)
		{
			FitnessSelector<GameData*> selector;
			auto list = gamedata->getReferenceListIfExists("weapon models");
			if (list != nullptr)
			{
				for (auto modelIter = models->begin(); modelIter != models->end(); ++modelIter)
				{
					for (auto iter = list->begin(); iter != list->end(); ++iter)
					{
						if (iter->sid == modelIter->sid)
							selector.addItem(modelIter->getPtr(&ou->gamedata), static_cast<float>(modelIter->values.value[0]));
					}
				}
			}

			auto materialData = selector.chooseAnItem();
			if (materialData != nullptr)
			{
				self->naturalWeapon = (Sword*)ou->theFactory->createItem(gamedata, hand(), weaponMesh, materialData, 0, nullptr);
				return;
			}
		}

		self->naturalWeapon = (Sword*)ou->theFactory->createItem(gamedata, hand(), weaponMesh, nullptr, 0, nullptr);
	}
}

void KEP::CharacterExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&SquadManagementScreen::dismissCharacter), &SquadManagementScreen_dismissCharacter_hook, &SquadManagementScreen_dismissCharacter_orig))
		ErrorLog("[SquadManagementScreen::dismissCharacter] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Character::_NV_giveBirth), &Character_giveBirth_hook, &Character_giveBirth_orig))
		ErrorLog("[Character::giveBirth] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress((bool (PlayerInterface::*)(const lektor<Character*>&, bool))&PlayerInterface::recruit), &PlayerInterface_recruit_hook, &PlayerInterface_recruit_orig))
		ErrorLog("[PlayerInterface::recruit] could not install hook!");

	if (settings._characterExtension)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Character::generateWeapon), &Character_generateWeapon_hook, &Character_generateWeapon_orig))
			ErrorLog("[Character::generateWeapon] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Character::_NV_reCalculateNaturalWeapon), &Character_reCalculateNaturalWeapon_hook, &Character_reCalculateNaturalWeapon_orig))
			ErrorLog("[Character::reCalculateNaturalWeapon] could not install hook!");
	}

}
