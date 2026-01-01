#pragma once
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreTexture.h>
#include <ogre/OgreVector4.h>
#include <kenshi/util/iVector2.h>
#include <kenshi/util/hand.h>

// KenshiLibでは未定義
class PortraitImage
{
public:
	uint16_t portraitIndex;
	bool created;
	std::string textureName;
	Ogre::Vector4 individualImagePoints;
	Ogre::Vector4 portraitImagePoints;
};

// KenshiLibでは未定義
class PortraitData
{
public:
	std::string name;
	uint8_t _0x28;
	uint32_t overlayFlag;
	bool _0x30;
	hand characterHandle;
	bool _0x58;
};

//placeholder name
class PortraitManager : private Ogre::GeneralAllocatedObject
{
public:
	iVector2 imageSize;
	std::string textureName;
	iVector2 portraitSize;
	uint16_t numWidth;
	Ogre::SharedPtr<Ogre::Texture> texture;
	Ogre::vector<std::pair<bool, PortraitImage*>>::type _0x50;
	Ogre::map<hand, std::pair<hand, std::pair<PortraitData*, PortraitImage*>>>::type _0x70;
	bool* usedSlots;
	bool _0xa0;
};

//placeholder name
class RenderToTexture
{
public:
	Ogre::SceneManager* sceneManager;
	Ogre::Camera* camera;
};
