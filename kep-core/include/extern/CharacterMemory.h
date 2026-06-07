#pragma once
#include <ogre/OgrePrerequisites.h>
#include <kenshi/Enums.h>
#include <kenshi/util/hand.h>
#include <kenshi/util/TimeOfDay.h>
#include <kenshi/util/TagsClass.h>

enum CharacterPerceptionTags_ShortTerm;
enum CharacterPerceptionTags_LongTerm;

class SubjectiveTags
{
public:
	virtual void setTagshortTerm();
	virtual void setTagLongTerm();
	TimeOfDay tagtime;
	TagsClass<CharacterPerceptionTags_LongTerm> tagLong;
	TagsClass<CharacterPerceptionTags_ShortTerm> tagShort;
	hand _0x30;
};

class Platoon;

class CharacterMemory
{
public:
	Ogre::map<hand, SubjectiveTags>::type tags;
	bool _0x28;
	Platoon* _0x30;
};
