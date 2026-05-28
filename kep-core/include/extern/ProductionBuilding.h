#pragma once
#include <string>
#include <kenshi/util/hand.h>

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