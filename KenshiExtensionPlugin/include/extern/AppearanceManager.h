#pragma once
#include <string>
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreFastArray.h>
#include <ogre/OgreVector3.h>
#include <kenshi/util/OgreUnordered.h>

class AnimationData;
class GameData;

// KenshiLibでは未定義
class AppearanceManager : private Ogre::GeneralAllocatedObject
{
public:
	class Gender
	{
	public:
		enum Enum
		{
			MALE,
			FEMALE
		};
	};

	class DataRange
	{
	public:
		std::string name;
		short max;
		short min;
		short mid;
		short randomGroup;
		short randomVariation;

		virtual void vfunc0x0(GameData*);
	};

	class DataRangePose : public DataRange
	{
	public:
		std::string target;
		std::string targetOpposite;
	};

	class DataRangeVector : public DataRange
	{
	public:
		Ogre::FastArray<Ogre::Vector3> vectors;
	};

	class DataCategory
	{
	public:
		enum Enum
		{
			NONE,
			FACE,
			BODY,
			HAIR,
			PERSONALITY
		};
		Ogre::FastArray<DataRange> _0x0;
		Ogre::FastArray<DataRangePose> _0x18;
		Ogre::FastArray<DataRangePose> _0x30;
		Enum _0x48;
	};

	class AppearanceData
	{
		GameData* _0x0;
		Gender::Enum _0x8;
		Ogre::FastArray<GameData*> _0x10;
		ogre_unordered_map<int, Ogre::FastArray<GameData*>>::type _0x28;
		ogre_unordered_map<DataCategory::Enum, DataCategory>::type _0x68;
		ogre_unordered_map<std::string, DataRange*>::type _0xa8;
		Ogre::FastArray<float> _0xe8;
		ogre_unordered_map<int, Ogre::FastArray<float>>::type _0x100;
		ogre_unordered_map<size_t, Ogre::FastArray<float>>::type _0x140;
		ogre_unordered_map<std::string, std::string>::type _0x180;
		std::string _0x1c0;
	};

	Ogre::FastArray<GameData*> _0x0;
	ogre_unordered_map<GameData*, ogre_unordered_map<Gender::Enum, AppearanceData>::type>::type _0x18;
	Ogre::FastArray<AnimationData*> idleStances;
	GameData* _0x70;
};
