/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#pragma once
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreVector3.h>
#include <ogre/OgreVector4.h>
#include <kenshi/util/OgreUnordered.h>
#include <kenshi/util/hand.h>


class GameData;
class ScythePhysicsT;

// KenshiLibでは未定義
class AttachedObject
{
public:
	uint32_t attachType;
	std::string boneName;
	Ogre::Vector3 pos;
	Ogre::Quaternion rot;
	hand handle;
	GameData* data;

	virtual ~AttachedObject();
	virtual void detach();
};

// KenshiLibでは未定義
class AttachedEntity : public AttachedObject
{
public:
	Ogre::Entity* entity;
	std::string material;
	GameData* materialGameData;
	GameData* colorGameData;
	std::string mesh;
	GameData* physicsAttachment;
	ScythePhysicsT* scytheObject;
	bool isOverlap;

	virtual ~AttachedEntity();
};
