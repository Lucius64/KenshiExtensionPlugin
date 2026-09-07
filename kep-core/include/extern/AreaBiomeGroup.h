/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#pragma once
#include <kenshi/util/lektor.h>
#include <kenshi/util/hand.h>
#include <kenshi/util/iVector2.h>
#include <kenshi/util/OgreUnordered.h>

class GameData;
class SpawnInfoList;
class Platoon;
class AreaSector;
enum CropType;

class AreaBiomeGroup
{
public:
	int numNests;
	bool nestsAtFixedMarkersOnly;
	int index;
	GameData* data;
	lektor<AreaSector*> sectors;
	Ogre::map<hand, Ogre::Vector3>::type _0x30;
	SpawnInfoList* spanws;
	Ogre::map<CropType, float>::type cropYields;
	float homelessSpawnAmount;
	float acidicGround;
	float acidicWater;
	int _0x94;
	WeatherRegion* weatherRegion;
};

class AreaSector
{
public:
	iVector2 coordinates;
	Ogre::Vector4 _0x8;
	Ogre::Vector3 _0x18;
	AreaBiomeGroup* area;
	bool hasTown;
	bool hasOutpost;
	bool _0x32;
	Ogre::set<Platoon*>::type _0x38;
	Ogre::set<Platoon*>::type _0x60;
	Ogre::set<Platoon*>::type _0x88;
	bool _0xb0;
};

class RealWorldEditableImage;

class AreasList
{
public:
	RealWorldEditableImage* areamapImage;
	boost::unordered::unordered_map<AreaBiomeGroup*, std::set<AreaSector*>, boost::hash<AreaBiomeGroup*>, std::equal_to<AreaBiomeGroup*>, Ogre::STLAllocator<std::pair<AreaBiomeGroup* const, std::set<AreaSector*>>, Ogre::CategorisedAllocPolicy<Ogre::MEMCATEGORY_GENERAL> > > sectorList;
	boost::unordered::unordered_map<GameData*, AreaBiomeGroup, boost::hash<GameData*>, std::equal_to<GameData*>, Ogre::STLAllocator<std::pair<GameData* const, AreaBiomeGroup>, Ogre::CategorisedAllocPolicy<Ogre::MEMCATEGORY_GENERAL> > > areaList;
	AreaSector worldMap[64][64];
};
