#include <ogre/OgrePrerequisites.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GlobalConstants.h>
#include <kenshi/GameWorld.h>
#include <kenshi/Faction.h>
#include <kenshi/Platoon.h>
#include <kenshi/Character.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/Dialogue.h>

#include <extern/AI.h>
#include <extern/Blackboard.h>
#include <extern/SquadManagementScreen.h>
#include <extern/StateBroadcastData.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <CharacterExtension.h>


namespace
{
	void (*SquadManagementScreen_FUN_0048EFA0_orig)(SquadManagementScreen*, int);
	bool (*Character_giveBirth_orig)(Character*, GameDataCopyStandalone*, const Ogre::Vector3&, const Ogre::Quaternion&, GameSaveState*, ActivePlatoon*, Faction*);
	bool (*PlayerInterface_recruit_orig)(PlayerInterface*, const lektor<Character*>&, bool);

	std::string dismissedCharacterFactionSID = "";
}

void KEP::CharacterExtension::SquadManagementScreen_FUN_0048EFA0_hook(SquadManagementScreen* self, int flag)
{
	if (!settings._dismissedCharacterExtension)
	{
		SquadManagementScreen_FUN_0048EFA0_orig(self, flag);
		return;
	}

	if (flag != 2 || self->character == nullptr)
	{
		self->character = nullptr;
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

	if (ou->player->isObjectSelected(self->character))
		(ou->player->unselectPlayerCharacter)(self->character);

	self->character->setFaction(faction, nullptr);

	DebugLog("[ex-player faction] " + faction->name);

	auto squadTemplate = ou->gamedata.getData("5006-squad.mod");
	auto pos = self->character->getPosition();

	auto platoon = faction->createNewEmptyActivePlatoon(squadTemplate, true, pos);
	platoon->activePlatoon->addActiveObject(self->character);

	auto blackboard = externalFunctions->FUN_005065E0(self->character->ai);
	if (blackboard != nullptr)
	{
		blackboard->_0x130 = true;
		blackboard->leaderTasks.clear();
		blackboard->squad1Tasks.clear();
		blackboard->squad2Tasks.clear();
		blackboard->slaveTasks.clear();
		externalFunctions->FUN_002715E0(blackboard, squadTemplate);
	}
	externalFunctions->FUN_005067F0(self->character->ai->taskSystem);
	externalFunctions->FUN_0050C720(&self->character->ai->taskSystem->_0x300);
	externalFunctions->FUN_004AB930(self->character->ai->taskSystem);
	externalFunctions->FUN_0048EA30(self);

	self->character = nullptr;
	return;
}

bool KEP::CharacterExtension::Character_giveBirth_hook(Character* self, GameDataCopyStandalone* appearance, const Ogre::Vector3& position, const Ogre::Quaternion& rotation, GameSaveState* state, ActivePlatoon* tempplatoonptr, Faction* _faction)
{
	bool success = Character_giveBirth_orig(self, appearance, position, rotation, state, tempplatoonptr, _faction);
	if (settings._dialogueExtension && self->isAnimal() != nullptr && self->platoon->me->squadTemplate->listExistsAndNotEmpty("dialog animal"))
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

bool KEP::CharacterExtension::PlayerInterface_recruit_hook(PlayerInterface* self, const lektor<Character*>& characters, bool editor)
{
	bool success = PlayerInterface_recruit_orig(self, characters, editor);
	if ((settings._dialogueExtension) && success)
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

void KEP::CharacterExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_0048EFA0, &SquadManagementScreen_FUN_0048EFA0_hook, &SquadManagementScreen_FUN_0048EFA0_orig))
		ErrorLog("KenshiExtensionPlugin: [dissmiss character extension] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Character::_NV_giveBirth), &Character_giveBirth_hook, &Character_giveBirth_orig))
		ErrorLog("KenshiExtensionPlugin: [keep npc class pt1] could not install hook!");

	bool (PlayerInterface::*ptrToPlayerInterface)(const lektor<Character*>&, bool) = &PlayerInterface::recruit;
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(ptrToPlayerInterface), &PlayerInterface_recruit_hook, &PlayerInterface_recruit_orig))
		ErrorLog("KenshiExtensionPlugin: [keep npc class pt2] could not install hook!");
}
