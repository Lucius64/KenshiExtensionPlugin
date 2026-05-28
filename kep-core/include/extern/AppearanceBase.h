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
	std::string _0x88;
	GameData* materialGameData;
	GameData* colorGameData;
	std::string attachPointName;
	GameData* physicsAttachmentData;
	ScythePhysicsT* scytheObject;
	bool isOverlap;

	virtual ~AttachedEntity();
};
