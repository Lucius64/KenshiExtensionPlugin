#pragma once
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreVector3.h>
#include <ogre/OgreVector4.h>
#include <kenshi/util/OgreUnordered.h>
#include <kenshi/util/hand.h>


class GameData;
class Character;
class AnimationClass;
class AnimationData;
class Harpoon;
class CollapseParts;
class AttachmentData;
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

// KenshiLibでは未定義
class AppearanceBase
{
public:
	ogre_unordered_map<std::string, AttachmentData*>::type* slot;
	ogre_unordered_map<std::string, AttachedEntity*>::type attachedEntities;
	Ogre::FastArray<Harpoon*> _0x50;
	size_t _0x68[7];
	Ogre::FastArray<CollapseParts*> collapseParts;
	size_t _0xb8[4];
	Ogre::Entity* entity;
	Ogre::Entity* characterModel;
	bool _0xe8;
	float _0xec;
	Character* me;
	bool visible;
	std::string meshFile;
	size_t _0x128[2];
	AnimationClass* animation;
	bool isFemale;
	bool _0x141;
	bool _0x142;
	bool _0x143;
	GameData* data;
	GameData* raceData;
	GameData* colorData;
	GameData* hairData;
	bool shaved;
	Ogre::Vector4 _0x16c;
	float _0x17c;
	bool barefoot;
	bool _0x181;
	float _0x184;
	float _0x188;
	float height;

	virtual ~AppearanceBase();
	virtual void func0x8();
	virtual void func0x10();
	virtual void func0x18();
	virtual void func0x20();
	virtual void func0x28();
	virtual void func0x30();
	virtual void setFlayed(bool flayed);
	virtual bool isFlayed();
	virtual int getBarefoot();
	virtual void updateVisbleForAttachedEntityies(bool visible);
	virtual void func0x58();
	virtual void func0x60();
	virtual void updateCharaterTexture();
};

// KenshiLibでは未定義
class AppearanceHuman : public AppearanceBase
{
public:
	float _0x190;
	Ogre::Vector3 _0x194;
	float bodyBulk;
	float muscular;
	float hunger;
	bool flayed;
	GameData* beardData;
	bool _0x1b8;
};
