/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#pragma once
#include <ogre/OgrePrerequisites.h>
#include <kenshi/util/OgreUnordered.h>
#include <kenshi/FitnessSelector.h>

class GameData;
enum itemType;

class VendorList
{
public:
	GameData* data;
	FitnessSelector<GameData*> itemSelector;
	Ogre::vector<std::pair<GameData*, float>>::type _0x68;
	FitnessSelector<GameData*> weaponLevelSelector;
	FitnessSelector<uint32_t> armorLevelSelector;
	Ogre::set<itemType> _0x148;
	ogre_unordered_set<GameData*>::type _0x170;
private:

};