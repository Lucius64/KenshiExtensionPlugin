/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <boost/filesystem.hpp>

#include <ogre/OgreResourceGroupManager.h>
#include <ogre/OgreStringConverter.h>

#include <mygui/MyGUI_ItemBox.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>
#include <kenshi/util/UtilityT.h>
#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/WorldEventStateQuery.h>
#include <kenshi/Town.h>
#include <kenshi/gui/ForgottenGUI.h>
#include <kenshi/gui/NewGameWindow.h>
#include <kenshi/Building/TortureBuilding.h>
#include <kenshi/Character.h>
#include <kenshi/Faction.h>
#include <kenshi/Platoon.h>
#include <kenshi/AI/AI.h>
#include <kenshi/StateBroadcastData.h>
#include <kenshi/Item.h>
#include <kenshi/gui/ProspectingWindow.h>
#include <kenshi/SaveManager.h>
#include <kenshi/AI/Blackboard.h>

#include <extern/TownBase.h>
#include <extern/BasePopulationManager.h>

#include <kep/functions.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <bugfix/MiscFix.h>

namespace fs = boost::filesystem;

namespace
{
	struct GameDataNewGameLess
	{
		bool operator()(const GameData* a, const GameData* b) const
		{
			if (a->createdIndex != b->createdIndex)
				return a->createdIndex < b->createdIndex;
			return a->stringID < b->stringID;
		}
	};
}

namespace
{
	WorldEventStateQuery* (*WorldEventStateQuery_getFromData_orig)(GameData*);
	WorldEventStateQuery* WorldEventStateQuery_getFromData_hook(GameData* d)
	{
		auto query = WorldEventStateQuery_getFromData_orig(d);
		if (KEP::settings._enablePlayerInvolvement)
			query->playerInvolvement = d->bdata["player involvement"];

		return query;
	}

	bool (*UtilityT_moveFile_orig)(UtilityT*, const std::string&, const std::string&);
	bool UtilityT_moveFile_hook(UtilityT* self, const std::string& from, const std::string& to)
	{
		if (!KEP::settings._enableTheNavigationMeshCache)
			return UtilityT_moveFile_orig(self, from, to);

		fs::path srcPath = *KEP::externalGlobals->_KenshiFolder;
		srcPath /= from;
		fs::path destPath = to;
		return MoveFileW(srcPath.wstring().c_str(), destPath.wstring().c_str()) != 0;
	}

	void (*Town_loadState_orig)(Town*, GameData*);
	void Town_loadState_hook(Town* self, GameData* state)
	{
		Town_loadState_orig(self, state);
		if (self->replacementTown != nullptr && KEP::settings._fixTownOverride)
		{
			self->minFoliageRange = self->replacementTown->fdata["no-foliage range"];
			self->townType = static_cast<TownType>(self->replacementTown->idata["type"]);
			self->townRangeMultiplier = self->replacementTown->fdata["town radius mult"];

			if (self->townRangeMultiplier < 0.1f)
				self->townRangeMultiplier = 1.0f;
			else if (self->townRangeMultiplier >= 5.0f)
				self->townRangeMultiplier = 5.0f;

			self->openToPublic = self->replacementTown->bdata["is public"];

			if (self->replacementTown->bdata["town name override"])
				self->setName(self->replacementTown->name);
		}
	}

	void (*Town_notifyRepopulation_orig)(Town*, GameData*);
	void Town_notifyRepopulation_hook(Town* self, GameData* newTowndata)
	{
		Town_notifyRepopulation_orig(self, newTowndata);

		if (KEP::settings._fixTownOverride)
		{
			self->townRangeMultiplier = newTowndata->fdata["town radius mult"];

			if (self->townRangeMultiplier < 0.1f)
				self->townRangeMultiplier = 1.0f;
			else if (self->townRangeMultiplier >= 5.0f)
				self->townRangeMultiplier = 5.0f;

			if (self->population->ded)
				self->population->ded = false;

			if (newTowndata->bdata["town name override"])
				self->setName(newTowndata->name);
		}
	}

	DistantTown* (*DistantTown__CONSTRUCTOR_orig)(DistantTown*, Town*);
	DistantTown* DistantTown__CONSTRUCTOR_hook(DistantTown* self, Town* town)
	{
		if (!KEP::settings._fixVisibleDistantTown)
			return DistantTown__CONSTRUCTOR_orig(self, town);

		self->owner = town;
		self->_0x8 = 0;
		self->visible = false;
		self->_0x10 = nullptr;
		return self;
	}

	void (*DistantTown_FUN_000D8620_orig)(DistantTown*, bool);
	void DistantTown_FUN_000D8620_hook(DistantTown* self, bool visible)
	{
		if (KEP::settings._enableCrashPrevention && visible && visible != self->visible && self->_0x8 == 0)
		{
			auto resourceMgr = Ogre::ResourceGroupManager::getSingletonPtr();
			std::string meshfile;
			KEP::externalFunctions->FUN_000D6360(self, &meshfile);
			if (!resourceMgr->resourceExistsInAnyGroup(meshfile))
			{
				if (self->_0x10 == nullptr)
					KEP::externalFunctions->FUN_000D8310(self);

				self->visible = visible;
				return;
			}
		}

		DistantTown_FUN_000D8620_orig(self, visible);
	}

	void (*FUN_004B0B60_orig)(void*, MyGUI::ItemBox*, MyGUI::types::TCoord<int>&, bool);
	void FUN_004B0B60_hook(void* self, MyGUI::ItemBox* _sender, MyGUI::IntCoord& _coord, bool _drop)
	{
		_coord.left = 0;
		_coord.top = 0;
		_coord.width = _sender->getClientCoord().width;
		_coord.height = static_cast<int>(gui->Scale.y * 28.0f);
	}

	void (*FUN_002C9840_orig)(void*, MyGUI::ItemBox*, MyGUI::types::TCoord<int>&, bool);
	void FUN_002C9840_hook(void* self, MyGUI::ItemBox* _sender, MyGUI::IntCoord& _coord, bool _drop)
	{
		_coord.left = 0;
		_coord.top = 0;
		_coord.width = _sender->getClientCoord().width;
		_coord.height = static_cast<int>(gui->Scale.y * 28.0f);
	}

	void (*NewGameWindow_loadData_orig)(NewGameWindow*);
	void NewGameWindow_loadData_hook(NewGameWindow* self)
	{
		lektor<GameData*> newGames;
		ou->gamedata.getDataOfType(newGames, NEW_GAME_STARTOFF);
		std::sort(newGames.begin(), newGames.end(), GameDataNewGameLess());
		int defaultIndex = -1;

		for (uint32_t i = 0; i < newGames.size(); ++i)
		{
			auto newGame = newGames[i];
			if (defaultIndex == -1 && newGame->stringID == "1980-gamedata.base")
				defaultIndex = static_cast<int>(i);

			self->startsData.push_back(newGame);
		}

		if (defaultIndex > -1)
			self->currentStart = defaultIndex;
		else
			self->currentStart = 0;
	}

	void (*TortureBuilding_update_orig)(TortureBuilding*);
	void TortureBuilding_update_hook(TortureBuilding* self)
	{
		Character* character = nullptr;
		if (self->currentOperators.size() != 0)
			character = self->currentOperators.begin()->getCharacter();

		TortureBuilding_update_orig(self);

		if (self->currentOperators.size() == 0 && character != nullptr)
			character->setPrisonMode(false, nullptr);
	}

	void (*UtilityT_getResourceFilePath_orig)(const std::string&, std::string&);
	void UtilityT_getResourceFilePath_hook(const std::string& filename, std::string& filepath)
	{
		if (!KEP::settings._fixGetResourceFilePath)
		{
			UtilityT_getResourceFilePath_orig(filename, filepath);
			return;
		}

		auto resourceMgr = Ogre::ResourceGroupManager::getSingletonPtr();
		if (resourceMgr->resourceExistsInAnyGroup(filename))
		{
			auto fileInfoList = resourceMgr->findResourceFileInfo(resourceMgr->findGroupContainingResource(filename), filename);
			if (fileInfoList->size() != 0)
			{
				const auto& fileInfo = fileInfoList->back();
				filepath.assign(fileInfo.archive->getName());
				filepath.append("\\").append(fileInfo.filename);
			}
		}
	}

	bool (*AI_itemOnGroundStealingCheckIsSafe_orig)(AI*, Item*);
	bool AI_itemOnGroundStealingCheckIsSafe_hook(AI* self, Item* item)
	{
		if (!KEP::settings._fixStealingCheck)
			return AI_itemOnGroundStealingCheckIsSafe_orig(self, item);

		auto town = self->me->getStateBroadcast()->currentTownLocation;
		bool inPlayerTown = town != nullptr ? town->getFaction()->isPlayer != nullptr : false;

		auto building = item->isIndoors().getBuilding();
		bool inNpcBuilding = building != nullptr ? building->getFaction()->isPlayer == nullptr : false;

		if (inNpcBuilding || !inPlayerTown)
		{
			if (inNpcBuilding && building != nullptr)
			{
				auto residentSquad = building->residentSquad.getPlatoon();
				if (residentSquad != nullptr && residentSquad->blackboard->homeIsLockedUp)
					return false;
			}
			auto platoon = item->getProperOwner().getPlatoon();
			if (platoon != nullptr)
				return self->me->getFaction() == platoon->getFaction();
		}

		return true;
	}

	void (*ProspectiongWindow_showT_orig)(ProspectingWindow*, const Ogre::Vector3&, float, const std::string&);
	void ProspectiongWindow_showT_hook(ProspectingWindow* self, const Ogre::Vector3& pos, float skill, const std::string& name)
	{
		if (KEP::settings._enableCrashPrevention && skill < 0.0f)
			skill = 0.0f;
		ProspectiongWindow_showT_orig(self, pos, skill, name);
	}

	int (*SaveManager_importGame_orig)(SaveManager*, const std::string&, const std::string&, int);
	int SaveManager_importGame_hook(SaveManager* self, const std::string& location, const std::string& name, int flags)
	{
		int res = SaveManager_importGame_orig(self, location, name, flags);
		if (KEP::settings._importingNegativeMoney && (res == 0 || res == 2))
		{
			lektor<GameData*> list;
			ou->savedata.getDataOfType(list, CAMERA);
			if (list.size() != 0)
			{
				int money = list[0]->idata["player money"];
				if (money < 0)
					ou->player->participant->factionOwnerships->money = money;
			}
		}
		return res;
	}
}

void KEP::MiscFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&WorldEventStateQuery::getFromData), &WorldEventStateQuery_getFromData_hook, &WorldEventStateQuery_getFromData_orig))
		ErrorLog("[WorldEventStateQuery::getFromData] could not install hook!");

	if (GetModuleHandleA("_kenshi_fix1.asi") != nullptr)
	{
		DebugLog("Kenshi CTD fix patch has been detected, so the Navigation Mesh cache function has been disabled.");
	}
	else
	{
		if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&UtilityT::moveFile), &UtilityT_moveFile_hook, &UtilityT_moveFile_orig))
			ErrorLog("[UtilityT::moveFile] could not install hook!");
	}

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Town::_NV_loadState), &Town_loadState_hook, &Town_loadState_orig))
		ErrorLog("[Town::loadState] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Town::notifyRepopulation), &Town_notifyRepopulation_hook, &Town_notifyRepopulation_orig))
		ErrorLog("[Town::notifyRepopulation] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(externalFunctions->FUN_000D5FD0, &DistantTown__CONSTRUCTOR_hook, &DistantTown__CONSTRUCTOR_orig))
		ErrorLog("[DistantTown::DistantTown] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(externalFunctions->FUN_000D8620, &DistantTown_FUN_000D8620_hook, &DistantTown_FUN_000D8620_orig))
		ErrorLog("[FUN_000D8620] could not install hook!");

	if (settings._fixTechAndCraftingQueue)
	{
		if (KenshiLib::SUCCESS != KenshiLib::QueueHook(externalFunctions->FUN_004B0B60, &FUN_004B0B60_hook, &FUN_004B0B60_orig))
			ErrorLog("[FUN_004B0B60] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::QueueHook(externalFunctions->FUN_002C9840, &FUN_002C9840_hook, &FUN_002C9840_orig))
			ErrorLog("[FUN_002C9840] could not install hook!");
	}

	if (settings._sortedNewGameStarts)
	{
		if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&NewGameWindow::loadData), &NewGameWindow_loadData_hook, &NewGameWindow_loadData_orig))
			ErrorLog("[NewGameWindow::loadData] could not install hook!");
	}

	if (settings._fixTortureBuilding)
	{
		if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&TortureBuilding::_NV_update), &TortureBuilding_update_hook, &TortureBuilding_update_orig))
			ErrorLog("[TortureBuilding::_NV_update] could not install hook!");
	}

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&UtilityT::getResourceFilePath), &UtilityT_getResourceFilePath_hook, &UtilityT_getResourceFilePath_orig))
		ErrorLog("[UtilityT::getResourceFilePath] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&AI::itemOnGroundStealingCheckIsSafe), &AI_itemOnGroundStealingCheckIsSafe_hook, &AI_itemOnGroundStealingCheckIsSafe_orig))
		ErrorLog("[AI::itemOnGroundStealingCheckIsSafe] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&ProspectingWindow::showT), &ProspectiongWindow_showT_hook, &ProspectiongWindow_showT_orig))
		ErrorLog("[ProspectingWindow::showT] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&SaveManager::importGame), &SaveManager_importGame_hook, &SaveManager_importGame_orig))
		ErrorLog("[SaveManager::importGame] could not install hook!");
}
