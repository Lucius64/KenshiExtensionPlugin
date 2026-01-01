#pragma once
#include <ogre/OgrePrerequisites.h>
#include <kenshi/util/OgreUnordered.h>

class AppearanceHuman;
class AppearanceManager;
class Faction;
class GameData;
class Character;
class PortraitManager;

namespace KEP
{
	class IdleStanceList
	{
	public:
		const ogre_unordered_map<GameData*, Ogre::FastArray<std::pair<GameData*, int> > >::type& getRaceIdleStanceList() const { return _raceIdleStanceList; };
		const ogre_unordered_map<Faction*, ogre_unordered_map<GameData*, int>::type>::type& getFactionIdleStanceList() const { return _factionIdleStanceList; };

		static const IdleStanceList& getSingleton();
	private:
		IdleStanceList();

		ogre_unordered_map<GameData*, Ogre::FastArray<std::pair<GameData*, int> > >::type _raceIdleStanceList;
		ogre_unordered_map<Faction*, ogre_unordered_map<GameData*, int>::type>::type _factionIdleStanceList;
	};

	namespace AppearanceExtension
	{
		void AppearanceHuman_FUN_0052B1E0_hook(AppearanceHuman* self);
		void AppearanceManager_FUN_0006F8F0_hook(AppearanceManager* self, GameData* bodydata, Character* character);
		PortraitManager* FUN_00413FE0_hook(PortraitManager* self);
		void init();
	}
}