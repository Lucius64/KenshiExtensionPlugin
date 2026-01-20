#pragma once
#include <extern/StorageBuilding.h>

// KenshiLibでは未定義
class ProductionBuilding : public StorageBuilding
{
public:
	int _0x468;
	float resourceMult;
	InventorySection* outputInventory;
	lektor<StorageBuilding::ConsumptionItem> inputItems;

	virtual ~ProductionBuilding();
};

// KenshiLibでは未定義
class FurnaceBuilding : public ProductionBuilding
{
public:
	bool _0x480;

	virtual ~FurnaceBuilding();
};

// KenshiLibでは未定義
class CraftingItem
{
public:
	Item* item;
	float _0x8;
	float progress;
	hand _0x10;
	int _0x30;
	std::string _0x38;
	int _0x60;
	int _0x64;
};

// KenshiLibでは未定義
class CraftingBuilding : public ProductionBuilding
{
public:
	int weaponLevelMax;
	std::deque<CraftingItem> craftingItemDeque;
	itemType craftingItemType;
	bool repeat;
	hand crafter;
	bool _0x4f0;
	bool _0x4f1;
	GameData* _0x4f8;
	size_t _0x500;
	lektor<Item*> _0x508;
	ogre_unordered_map<GameData*, float>::type _0x520;

	virtual ~CraftingBuilding();
};

// KenshiLibでは未定義
class GameDataGroup
{
public:
	GameData* data1;
	GameData* data2;

	GameDataGroup(GameData* _data1 = nullptr, GameData* _data2 = nullptr)
		: data1(_data1)
		, data2(_data2)
	{

	}
};

// KenshiLibでは未定義
struct WeaponTypeStruct
{
	float level;
	bool craftListFixed;
	GameData* data;

	WeaponTypeStruct(float _level = 0.0f, bool _craftListFixed = false, GameData* _data = nullptr)
		: level(_level)
		, craftListFixed(_craftListFixed)
		, data(_data)
	{

	}
};