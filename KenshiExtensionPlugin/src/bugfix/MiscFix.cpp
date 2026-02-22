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

#include <extern/ForgottenGUI.h>
#include <extern/TownBase.h>
#include <extern/BasePopulationManager.h>
#include <extern/Research.h>
#include <extern/NewGameWindow.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <bugfix/MiscFix.h>


namespace fs = boost::filesystem;

namespace
{
	WorldEventStateQuery* (*WorldEventStateQuery_getFromData_orig)(GameData*);
	bool (*UtilityT_moveFile_orig)(UtilityT*, const std::string&, const std::string&);
	void (*Town_FUN_00371480_orig)(Town*, GameData*);
	void (*Town_FUN_009FD8D0_orig)(Town*, GameData*);
	DistantTown* (*DistantTown__CONSTRUCTOR_orig)(DistantTown*, Town*);
	void (*DistantTown_FUN_000D8620_orig)(DistantTown*, bool);
	void (*Research_FUN_00830b90_orig)(Research*, GameData*);
	void (*FUN_004B0B60_orig)(void*, MyGUI::ItemBox*, MyGUI::types::TCoord<int>&, bool);
	void (*FUN_002C9840_orig)(void*, MyGUI::ItemBox*, MyGUI::types::TCoord<int>&, bool);
	void (*FUN_00912CA0_orig)(NewGameWindow*);

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

WorldEventStateQuery* KEP::WorldStateFix::WorldEventStateQuery_getFromData_hook(GameData* d)
{
	auto query = WorldEventStateQuery_getFromData_orig(d);
	if (settings._enablePlayerInvolvement)
		query->playerInvolvement = d->bdata["player involvement"];

	return query;
}

void KEP::WorldStateFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&WorldEventStateQuery::getFromData), &WorldEventStateQuery_getFromData_hook, &WorldEventStateQuery_getFromData_orig))
		ErrorLog("KenshiExtensionPlugin: [player involvement] could not install hook!");
}

bool KEP::NavMeshFix::UtilityT_moveFile_hook(UtilityT* self, const std::string& from, const std::string& to)
{
	if (!settings._enableTheNavigationMeshCache)
		return UtilityT_moveFile_orig(self, from, to);

	fs::path srcPath = *externalGlobals->_KenshiFolder;
	srcPath /= from;
	fs::path destPath = to;
	return MoveFileW(srcPath.wstring().c_str(), destPath.wstring().c_str()) != 0;
}

void KEP::NavMeshFix::init()
{
	auto hModule = GetModuleHandleA("_kenshi_fix1.asi");
	if (hModule != nullptr)
	{
		DebugLog("KenshiExtensionPlugin: Kenshi CTD fix patch has been detected, so the Navigation Mesh cache function has been disabled.");
		return;
	}

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&UtilityT::moveFile), &UtilityT_moveFile_hook, &UtilityT_moveFile_orig))
		ErrorLog("KenshiExtensionPlugin: [NavMesh cache] could not install hook!");
}

void KEP::TownOverrideFix::Town_FUN_00371480_hook(Town* self, GameData* savedata)
{
	Town_FUN_00371480_orig(self, savedata);
	if (self->replacementTown != nullptr && settings._fixTownOverride)
	{
		self->noFoliageRange = self->replacementTown->fdata["no-foliage range"];
		self->townType = static_cast<TownType>(self->replacementTown->idata["type"]);
		self->townRadiusMult = self->replacementTown->fdata["town radius mult"];

		if (self->townRadiusMult < 0.1f)
			self->townRadiusMult = 1.0f;
		else if (self->townRadiusMult >= 5.0f)
			self->townRadiusMult = 5.0f;

		self->isPublic = self->replacementTown->bdata["is public"];
		
		if (self->replacementTown->bdata["town name override"])
			self->setName(self->replacementTown->name);
	}
}

void KEP::TownOverrideFix::Town_FUN_009FD8D0_hook(Town* self, GameData* replacement)
{
	Town_FUN_009FD8D0_orig(self, replacement);

	if (settings._fixTownOverride)
	{
		self->townRadiusMult = replacement->fdata["town radius mult"];

		if (self->townRadiusMult < 0.1f)
			self->townRadiusMult = 1.0f;
		else if (self->townRadiusMult >= 5.0f)
			self->townRadiusMult = 5.0f;

		if (self->populationMgr->ded)
			self->populationMgr->ded = false;

		if (replacement->bdata["town name override"])
			self->setName(replacement->name);
	}
}

void KEP::TownOverrideFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_00371480, &Town_FUN_00371480_hook, &Town_FUN_00371480_orig))
		ErrorLog("KenshiExtensionPlugin: [town override p1] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_009FD8D0, &Town_FUN_009FD8D0_hook, &Town_FUN_009FD8D0_orig))
		ErrorLog("KenshiExtensionPlugin: [town override p2] could not install hook!");
}

DistantTown* KEP::DistantTownFix::DistantTown__CONSTRUCTOR_hook(DistantTown* self, Town* town)
{
	if (!settings._fixVisibleDistantTown)
		return DistantTown__CONSTRUCTOR_orig(self, town);

	self->owner = town;
	self->_0x8 = 0;
	self->visible = false;
	self->_0x10 = nullptr;
	return self;
}

void KEP::DistantTownFix::DistantTown_FUN_000D8620_hook(DistantTown* self, bool visible)
{
	if (visible && visible != self->visible && self->_0x8 == 0)
	{
		auto resourceMgr = Ogre::ResourceGroupManager::getSingletonPtr();
		std::string meshfile;
		externalFunctions->FUN_000D6360(self, &meshfile);
		if (!resourceMgr->resourceExistsInAnyGroup(meshfile))
		{
			if (self->_0x10 == nullptr)
				externalFunctions->FUN_000D8310(self);

			self->visible = visible;
			return;
		}
	}

	DistantTown_FUN_000D8620_orig(self, visible);
}

void KEP::DistantTownFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_000D5FD0, &DistantTown__CONSTRUCTOR_hook, &DistantTown__CONSTRUCTOR_orig))
		ErrorLog("KenshiExtensionPlugin: [distant town] could not install hook!");

	if (settings._enableCrashPrevention)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_000D8620, &DistantTown_FUN_000D8620_hook, &DistantTown_FUN_000D8620_orig))
			ErrorLog("KenshiExtensionPlugin: [missing distant town mesh CTD fix] could not install hook!");
	}
}

void KEP::ResearchFix::Research_FUN_00830b90_hook(Research* self, GameData* gameData)
{
	if (!settings._fixBuildingImprovements)
	{
		Research_FUN_00830b90_orig(self, gameData);
		return;
	}

	lektor<GameData*> improveBuildings;
	gameData->getAllFromListAsDatas("improve buildings", improveBuildings, &ou->gamedata, BUILDING);

	for (auto it = improveBuildings.begin(); it != improveBuildings.end(); ++it)
	{
		auto& buidingUpgrade = self->buildingUpgradeResearchs[*it];
		buidingUpgrade.productionMult *= gameData->fdata["production mult"];
		buidingUpgrade.powerOutput += gameData->idata["power increase"];
		buidingUpgrade.powerCapacity += gameData->idata["power capacity increase"];
	}
}

void KEP::ResearchFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_00830b90, &Research_FUN_00830b90_hook, &Research_FUN_00830b90_orig))
		ErrorLog("KenshiExtensionPlugin: [research building upgrade] could not install hook!");
}

void KEP::GUIFix::FUN_004B0B60_hook(void* self, MyGUI::ItemBox* parent, MyGUI::types::TCoord<int>& _coord, bool b)
{
	_coord.left = 0;
	_coord.top = 0;
	_coord.width = parent->getClientCoord().width;
	_coord.height = static_cast<int>(externalGlobals->_KenshiGUI->scaleFactor.y * 28.0f);
}

void KEP::GUIFix::FUN_002C9840_hook(void* self, MyGUI::ItemBox* parent, MyGUI::types::TCoord<int>& _coord, bool b)
{
	_coord.left = 0;
	_coord.top = 0;
	_coord.width = parent->getClientCoord().width;
	_coord.height = static_cast<int>(externalGlobals->_KenshiGUI->scaleFactor.y * 28.0f);
}

void KEP::GUIFix::FUN_00912CA0_hook(NewGameWindow* self)
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

		self->newGameStartList.push_back(newGame);
	}

	if (defaultIndex > -1)
		self->selected = defaultIndex;
	else
		self->selected = 0;
}

void KEP::GUIFix::init()
{
	if (settings._fixTechAndCraftingQueue)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_004B0B60, &FUN_004B0B60_hook, &FUN_004B0B60_orig))
			ErrorLog("KenshiExtensionPlugin: [queue size pt1] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_002C9840, &FUN_002C9840_hook, &FUN_002C9840_orig))
			ErrorLog("KenshiExtensionPlugin: [queue size pt2] could not install hook!");
	}

	if (settings._sortedNewGameStarts)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_00912CA0, &FUN_00912CA0_hook, &FUN_00912CA0_orig))
			ErrorLog("KenshiExtensionPlugin: [new game list] could not install hook!");
	}
}
