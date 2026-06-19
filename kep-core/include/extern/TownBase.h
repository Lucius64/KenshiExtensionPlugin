#pragma once
#include <kenshi/Town.h>

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

class Nest : public TownBase
{
public:
	bool _0x338;
};
