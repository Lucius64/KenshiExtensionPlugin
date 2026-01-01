#pragma once
#include <ogre/OgrePrerequisites.h>
#include <kenshi/util/lektor.h>
#include <kenshi/util/OgreUnordered.h>

enum WeatherAffecting;
class AnimationData;
class GameData;

// KenshiLibでは未定義
class AnimsListsManager
{
public:
	struct AnimList
	{
		lektor<AnimationData*> _0x0;
		lektor<AnimationData*> _0x18;
		lektor<AnimationData*> idleAnims;
		lektor<AnimationData*> _0x48;
		lektor<AnimationData*> _0x60;
		ogre_unordered_map<std::string, AnimationData*>::type _0x78;
		ogre_unordered_map<std::string, AnimationData*>::type enableAnims;
		AnimationData* _0xf8;
		ogre_unordered_map<GameData*, lektor<AnimationData*>>::type _0x100;
		ogre_unordered_map<WeatherAffecting, lektor<AnimationData*>>::type _0x140;
	};

	AnimList animList;
	Ogre::map<GameData*, AnimList>::type animListMap;
};
