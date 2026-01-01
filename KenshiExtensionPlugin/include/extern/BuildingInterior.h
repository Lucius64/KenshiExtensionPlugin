#pragma once
#include <ogre/OgrePrerequisites.h>
#include <kenshi/util/OgreUnordered.h>
#include <kenshi/util/hand.h>
#include <kenshi/RootObject.h>

class Building;
class BuildingItemGroup;
class PhysicsCollection;

class Layout : public RootObjectContainer
{
public:
	std::string _0x68;
	Building* building;
	BuildingInterior* buildingInterior;
	bool _0xa0;
	uintptr_t exterior;

	virtual ~Layout();
};

class BuildingInterior
{
public:
	Building* building;
	Ogre::Entity* interiorMask;
	PhysicsCollection* physicsCollection;
	size_t interiorParts;
	size_t facotryCallback;
	float _0x28;
	bool enabledInterior;
	bool _0x2d;
	Layout layout;
	bool allowAnimals;
	ogre_unordered_set<hand>::type handles;
	ogre_unordered_set<BuildingItemGroup*>::type buildingItemGroups;
};