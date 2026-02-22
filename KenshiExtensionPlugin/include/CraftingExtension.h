#pragma once

class GameData;
class CraftingBuilding;
class GameDataGroup;
template<typename T>
class lektor;

namespace KEP
{
	namespace CraftingExtension
	{
		GameData* FUN_002BB270_hook(CraftingBuilding* self, int level);
		void FUN_002BBCE0_hook(CraftingBuilding* self, lektor<GameDataGroup>& list);
		void FUN_002CA350_hook(const Ogre::vector<GameDataReference>::type* craftingItems, itemType type, lektor<GameDataGroup>& list);
		void init();
	}
}