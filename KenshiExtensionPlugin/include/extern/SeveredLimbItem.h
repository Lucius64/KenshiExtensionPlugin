#pragma once

#include <ogre/OgreVector4.h>
#include <ogre/OgreColourValue.h>

#include <kenshi/Item.h>

// KenshiLibでは未定義
class SeveredLimbItem : public Item
{
public:
	Ogre::Vector3 scale;
	Ogre::Vector4 skinTone;
	Ogre::ColourValue bloodColour;
	Ogre::Vector3 _0x214;
	TimeOfDay timeout;
};