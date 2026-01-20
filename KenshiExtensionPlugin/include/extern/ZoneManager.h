#pragma once
#include <boost/thread/shared_mutex.hpp>

#include <ogre/OgrePrerequisites.h>

#include <kenshi/util/OgreUnordered.h>
#include <kenshi/util/iVector2.h>
#include <kenshi/util/lektor.h>

class ZoneMapContent;
class ZoneMapOverlay;
class ZoneSmell;
class Town;
class MapFeatureList;

// KenshiLibでは未定義
class ZoneMap
{
public:
	ZoneMapContent* container;
	void* _0x8;
	ZoneSmell* _0x10;
	iVector2 zonePos;
	int _0x20;
	bool _0x24;
	size_t _0x28[0x28];

};


// KenshiLibでは未定義
class ZoneManager : public Ogre::GeneralAllocatedObject
{
public:
	struct BiomeGroundEffects
	{

	};

	bool _0x8;
	size_t _0x10[0x7];
	size_t _0x48[0x7];
	size_t _0x80[0x7];
	bool _0xb8;
	float _0xbc;
	float _0xc0;
	ZoneMap _zones[4096];
	ogre_unordered_set<ZoneMap*>::type _0x1680c8;
	ogre_unordered_set<ZoneMap*>::type _0x168108;
	ogre_unordered_set<Town*>::type _0x168148;
	lektor<MapFeatureList*> _0x168188;
	Ogre::set<Town*>::type _0x1681a0;
	iVector2 _0x1681c8;
	ZoneMap* _0x1681d0;
	int _0x1681d8;
	ogre_unordered_map<iVector2, ZoneMapOverlay*>::type _0x1681e0;
	void* _0x168220;
	ogre_unordered_map<int, uint8_t*>::type _0x168228;
	ogre_unordered_map<int, BiomeGroundEffects>::type _0x168268;
	size_t _0x1682a8[0x3];

	virtual bool vfunc0x0(Ogre::Vector3&);
	virtual bool vfunc0x8(Ogre::Vector3&);
	virtual Ogre::Vector4 vfunc0x10(iVector2&);
	virtual Ogre::Vector4 vfunc0x18(Ogre::Vector3&);
	virtual void vfunc0x20(void*);
};
