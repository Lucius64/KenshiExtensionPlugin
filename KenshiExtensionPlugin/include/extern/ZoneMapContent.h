#pragma once
#include <boost/thread/shared_mutex.hpp>

#include <ogre/OgrePrerequisites.h>

#include <kenshi/util/OgreUnordered.h>
#include <kenshi/RootObject.h>

enum BuildingFunction;
class ZoneMap;
class FoliageSystem;

// KenshiLibでは未定義
class ZoneMapContent : public RootObjectContainer
{
public:
	int _0x68;
	Ogre::map<BuildingFunction, Building*>::type _0x70;
	void* _0x98;
	void* _0xa0;
	bool _0xa8;
	void* _0xb0;
	lektor<RootObject*> _0xb8;
	ZoneMap* _0xd0;
	bool _0xd8;
	boost::shared_mutex _0xe0;
	FoliageSystem* _0x100;
	bool _0x108;
	ogre_unordered_set<hand>::type _0x110;
	lektor<InstanceID> _0x150;
};
