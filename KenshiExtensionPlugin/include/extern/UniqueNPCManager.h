#pragma once
#include <kenshi/util/hand.h>
#include <kenshi/util/OgreUnordered.h>
#include <kenshi/WorldEventStateQuery.h>

// KenshiLibでは未定義
class UniqueNPCManager
{
public:
	// KenshiLibでは未定義
	class UniqueCharacterState
	{
	public:
		GameData* characterTemplate;
		hand handle;
		WorldStateEnum state;
		bool playerInvolvement;

		UniqueCharacterState()
			: characterTemplate(nullptr)
			, handle()
			, state(ALIVE)
			, playerInvolvement(false)
		{

		}
	};
	ogre_unordered_map<GameData*, UniqueCharacterState>::type uniqueCharacterStates;
};
