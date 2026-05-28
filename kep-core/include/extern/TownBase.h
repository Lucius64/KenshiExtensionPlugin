#pragma once
#include <cstdint>
#include <kenshi/RootObject.h>

namespace Ogre
{
	class Entity;
}

class Town;

class DistantTown
{
public:
	Town* owner;
	uint32_t _0x8;
	bool visible;
	Ogre::Entity* _0x10;
};

class TownList : public RootObjectContainer
{
public:
	lektor<RootObject*> playerTowns;
	lektor<RootObject*> nests;
	lektor<Town*> _0x98;
	bool _0xb0;
	lektor<hand> _0xb8[64][64];
	boost::unordered::unordered_set<TownBase*, boost::hash<TownBase*>, std::equal_to<TownBase*>, Ogre::STLAllocator<TownBase*, Ogre::CategorisedAllocPolicy<Ogre::MEMCATEGORY_GENERAL> > > _0x180b8;
	TownBase* _0x180f8;
};
