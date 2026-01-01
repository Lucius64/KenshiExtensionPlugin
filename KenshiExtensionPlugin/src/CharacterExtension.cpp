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

void KEP::CharacterExtension::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_0048EFA0, &SquadManagementScreen_FUN_0048EFA0_hook, &SquadManagementScreen_FUN_0048EFA0_orig))
		ErrorLog("KenshiExtensionPlugin: [dissmiss character extension] could not install hook!");
}
