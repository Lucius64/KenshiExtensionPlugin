#pragma once
#include <string>

class GameData;
class Town;
class WorldEventStateQuery;
class UtilityT;
class DistantTown;
class Research;
class NewGameWindow;

namespace MyGUI
{
	class ItemBox;

	namespace types
	{
		template<typename T>
		struct TCoord;
	}
}

namespace KEP
{
	namespace WorldStateFix
	{
		WorldEventStateQuery* WorldEventStateQuery_getFromData_hook(GameData* d);
		void init();
	}

	namespace NavMeshFix
	{
		bool UtilityT_moveFile_hook(UtilityT* self, const std::string& from, const std::string& to);
		void init();
	}

	namespace TownOverrideFix
	{
		void Town_FUN_00371480_hook(Town* self, GameData* savedata);
		void Town_FUN_009FD8D0_hook(Town* self, GameData* replacement);
		void init();
	}

	namespace DistantTownFix
	{
		DistantTown* DistantTown__CONSTRUCTOR_hook(DistantTown* self, Town* town);
		void DistantTown_FUN_000D8620_hook(DistantTown* self, bool visible);
		void init();
	}

	namespace ResearchFix
	{
		void Research_FUN_00830b90_hook(Research* self, GameData* gameData);
		void init();
	}

	namespace GUIFix
	{
		void FUN_004B0B60_hook(void* self, MyGUI::ItemBox* parent, MyGUI::types::TCoord<int>& _coord, bool b);
		void FUN_002C9840_hook(void* self, MyGUI::ItemBox* parent, MyGUI::types::TCoord<int>& _coord, bool b);
		void FUN_00912CA0_hook(NewGameWindow* self);
		void init();
	}
}