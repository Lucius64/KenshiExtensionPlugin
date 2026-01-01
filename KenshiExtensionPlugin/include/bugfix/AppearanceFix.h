#pragma once
#include <ogre/OgrePrerequisites.h>

class AppearanceManager;
class GameData;
class Character;
class AppearanceHuman;
class PortraitManager;
class RenderToTexture;

namespace KEP
{
	namespace AppearanceFix
	{
		Ogre::Entity* AppearanceManager_FUN_0007C030_hook(AppearanceManager* self, GameData* bodydata, Ogre::Entity* entity, bool isPlayer, float facesWeirdness, bool isShaved);
		void AppearanceHuman_FUN_005375D0_hook(AppearanceHuman* self, bool visible);
		void RenderToTexture_FUN_0084A9C0_hook(RenderToTexture* self, Character* character, Ogre::SharedPtr<Ogre::Texture> texture, const PortraitManager& portraitMgr, const Ogre::Vector4& texcoord);
		void Ogre_Skeleton__refreshAnimationState_hook(Ogre::Skeleton* self, Ogre::AnimationStateSet* animSet);
		AppearanceManager* AppearanceManager__CONSTRUCTOR_hook(AppearanceManager* self);
		void init();
	}
}