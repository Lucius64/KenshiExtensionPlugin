#pragma once
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreVector3.h>
#include <kenshi/util/lektor.h>
#include <kenshi/util/OgreUnordered.h>
#include <kenshi/util/StringPair.h>
#include <kenshi/Enums.h>
#include <kenshi/GameData.h>

#include <extern/AnimsListsManager.h>


namespace RagdollPart
{
	enum Enum;
}

class GameData;
class Character;
class Entity;
class Node;
class OldSkeletonInstance;
class CharMovement;
class AppearanceBase;
class RagdollAnimation;
class SingleAnimation;
class AnimationStateData;
class CombatTechniqueData;


// KenshiLibでは未定義
class StringValuePair
{
	std::string str;
	uint32_t val;
};

// KenshiLibでは未定義
enum CharacterAnimCategory
{
	ANIM_NORMAL,
	ANIM_IMPRISONED,
	ANIM_SLEEPING,
	ANIM_CARRIED,
	ANIM_SWIMMING,
	ANIM_GROUNDED,
	ANIM_COMBAT,
	ANIM_ATTACKS,
	ANIM_RANGED
};

// KenshiLibでは未定義
class AnimationData
{
public:
	CharacterAnimCategory category;
	std::string name;
	std::string animName;
	GameData* data;
	int layer;
	uint32_t weatherType;
	float moveSpeed;
	float maxSpeed;
	float minSpeed;
	float playSpeed;
	uint32_t hasWeaponL;
	uint32_t hasWeaponR;
	uint32_t isCombatMode;
	uint32_t stealthMode;
	bool relocates;
	bool disablesMovement;
	bool loop;
	bool isLayerAll;
	bool synchs;
	bool isAction;
	float synchOffset;
	bool normalise;
	float possibleEnding;
	bool crouched;
	bool prone;
	bool usesRightArm;
	bool usesLeftArm;
	bool carryingRight;
	bool carryingLeft;
	bool beingCarried;
	std::string slaveAnim;
	float rightLegDamageMin;
	float rightLegDamageMax;
	float leftLegDamageMin;
	float leftLegDamageMax;
	float leftLegDamageIdeal;
	float rightLegDamageIdeal;
	float idleTimeMin;
	float idleTimeMax;
	float idleChance;
	Ogre::vector<GameDataReference>::type* events;
	uint32_t skillTypes;
	ogre_unordered_set<GameData*>::type stumbles;
	uint32_t stumbleFrom;
	bool bigStumble;
	float chance;
};

// KenshiLibでは未定義
class AnimationEvents
{
public:
	struct Pair
	{
		size_t _0x0;
		size_t _0x8;
	};

	lektor<Pair> pairs;
	float _0x18;
	int _0x1c;
	Character* character;
};

// KenshiLibでは未定義
class AnimationClassBase
{
public:

	// KenshiLibでは未定義
	class SingleAnimation
	{
	public:
		std::string name;
		Ogre::AnimationState* animState;
		AnimationData* animData;
		AnimationClassBase* animClass;
		float playSpeed;
		float weight;
		float startFrame;
		float synchOffset;
		float length;
		float currentFrame;
		float acceptableEndTime;
		bool isAction;
		bool loop;
		bool synchs;
		bool normalize;
		bool reverse;
		bool _0x61;
		bool _0x62;
		bool usesRightArm;
		bool usesLeftArm;
		bool _0x65;
		bool _0x66;
		bool _0x67;
		bool relocates;
		AnimationEvents animevents;
	};

	// KenshiLibでは未定義
	class AnimationLayer
	{
	public:
		lektor<SingleAnimation*> _0x0;
		lektor<SingleAnimation*> _0x18;
		Ogre::Entity* entity;
		AnimationClassBase* animClass;
		float blendRate;
		AnimationClassBase* _0x48;
	};

	bool _0x8;
	ogre_unordered_map<std::string, Ogre::Vector3>::type bonePositions;
	Ogre::map<std::string, Ogre::Quaternion>::type boneRotations;
	Ogre::Quaternion _0x78;
	bool _0x88;
	bool _0x89;
	Ogre::Vector3 _0x8c;
	Ogre::Vector3 _0x98;
	bool visible;
	bool _0xa5;
	Entity* entity;
	Node* node;
	OldSkeletonInstance* skeleton;
	CharMovement* charMove;
	float _0xc8;
	float _0xcc;
	lektor<AnimationLayer*> animLayers;

	virtual ~AnimationClassBase();
	virtual size_t vfunc0x8() = 0;
	virtual Character* getCharacter();
	virtual AnimationData* getAnimationData(const std::string&) = 0;
private:

};

class AnimationClassHuman;

//placeholder name
class AnimationStateData
{
public:
	CharacterAnimCategory animCategory;
	int32_t _0x4;
	bool _0x8;
	int32_t _0xc;
	int32_t _0x10;
	std::deque<int> _0x18;
	bool _0x48;
	int32_t _0x4c;
	int32_t _0x50;
	std::deque<int> _0x58;
	float _0x88;
	float _0x8c;
	float moveSpeed;
	float _0x94;
	float _0x98;
	bool _0x9c;
	bool hasWeaponL;
	bool hasWeaponR;
	bool _0x9f;
	bool isAffectedWeather;
	int32_t _0xa4;
	WeatherAffecting weatherAffectType;
	float _0xac;
	float weatherStrength;
	float playSpeedMult;
	bool _0xb8;
	std::string carryingAttachmentBoneName;
	int32_t _0xe8;
	hand carryingHandle;
	size_t _0x110;
	SingleAnimation* singleAnim;
	AnimationData* animationData;
	uint32_t _0x128;
	bool crouched;
	bool proned;
	bool carryingLeft;
	bool carryingRight;
	bool beingCarried;
	AnimationData* stumbleAnim;
	bool _0x140;
	AnimationData* idleAnim;
	float idleTime;
	uint32_t combatMode;
	uint32_t stealthMode;
	bool _0x15c;
	bool _0x15d;
	float RlegDamage;
	float LlegDamage;
	Ogre::Vector3 _0x168;
	WeaponCategory weaponCategory;
	Ogre::set<StringValuePair>::type _0x178;
	std::string overlayAnimName;
};

// KenshiLibでは未定義
class AnimationClass : public AnimationClassBase
{
public:
	AppearanceBase* appearance;
	AnimationStateData animationInfo;
	size_t _0x2b8;
	AnimsListsManager::AnimList* animList;
	float _0x2c8;
	float heightOffset;
	int _0x2d0;
	int _0x2d4;
	Character* me;
	AnimationData* idleStance;
	void* ragdollBone;
	RagdollAnimation* ragdollAnimation;
	RagdollPart::Enum ragdollPart;

	virtual ~AnimationClass();
	virtual AnimationClassHuman* isHumanAnimation() = 0;
	virtual void setAppearance(CharMovement*, GameData*, GameData*, Character*, GameData*) = 0;
	virtual void vfunc0x30(float);
	virtual void vfunc0x38();
	virtual void vfunc0x40(float);
	virtual void* vfunc0x48();
	virtual void setRagdoll(bool, Ogre::Vector3&, uint32_t, std::string&, Character*);
	virtual void vfunc0x58();
	virtual AnimationData* getAnimationData(const std::string&) override;
	virtual AnimationData* getAnimationData(GameData*);
	virtual AnimationData* getAnimationDataByActualName(const std::string&);
	virtual void setLegDamage(float left, float right);
	virtual void vfunc0x78(Ogre::set<StringPair>::type&);
	virtual void setOverlayAnimName(std::string&);
	virtual void vfunc0x88();
	virtual void setPronState(bool crouched, bool proned);
	virtual void vfunc0x98(int);
	virtual void vfunc0xa0();
	virtual bool isRequiredTurningAnimation();
	virtual bool vfunx0xb0();
	virtual bool playHeadTurningAnimation();
	virtual Ogre::Vector3 vfunx0xc0();
	virtual AnimationData* getIdleAnimationData();
	virtual AnimationData* getSwimIdleAnimationData();
	virtual AnimationData* getPronIdleAnimationData();
	virtual void playStrafeAnimation();
	virtual void vfunx0xe8(AnimationData*, float);
	virtual void playOverlayAnimation();
	virtual bool playTurningAnimation();
	virtual void playBreathingAnimation();
};

// KenshiLibでは未定義
class AnimationClassHuman : public AnimationClass
{
public:
	bool usingRightArm;
	bool usingLeftArm;
	std::string _0x308;
	Ogre::Vector3 _0x330;
	Ogre::Vector3 _0x33c;
	int _0x348;
	lektor<float> _0x350;

	virtual ~AnimationClassHuman();
	virtual AnimationClassHuman* isHumanAnimation() override;
	virtual void setAppearance(CharMovement*, GameData*, GameData*, Character*, GameData*) override;
};