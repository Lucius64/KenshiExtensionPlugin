#pragma once
#include <ogre/OgrePrerequisites.h>
#include <kenshi/util/OgreUnordered.h>

class ShopTrader;
class Inventory;
class InventorySection;
class UseableStuff;
class GameData;
enum itemType;

class InventoryManager
{
public:
	class BuildingInventoryInfo
	{
	public:
		Inventory* inventory;
		InventorySection* section;
		UseableStuff* stuff;
		itemType type;
		int lockLevel;
		float _0x20;
		ogre_unordered_set<GameData*>::type _0x28;
	};

	ShopTrader* onwer;
};
