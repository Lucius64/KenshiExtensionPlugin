/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#pragma once
#include <string>
#include <kenshi/util/hand.h>

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