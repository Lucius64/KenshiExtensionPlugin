#include <ogre/OgrePrerequisites.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Logger.h>
#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/RootObjectFactory.h>
#include <kenshi/util/hand.h>
#include <kenshi/Gear.h>
#include <kenshi/Item.h>
#include <kenshi/CharacterHuman.h>
#include <kenshi/CharStats.h>
#include <kenshi/MedicalSystem.h>
#include <kenshi/RaceData.h>
#include <kenshi/Inventory.h>
#include <kenshi/Platoon.h>
#include <kenshi/GunClass.h>

#include <extern/TemporaryLimbInterface.h>
#include <kep/functions.h>

#include <UtilityFunction.h>
#include <ExternalFunctions.h>
#include <Settings.h>
#include <RaceLimiterExtension.h>

namespace
{
	bool (*RaceLimiter_canEquip_orig)(RaceLimiter*, GameData*, RootObject*);
	bool RaceLimiter_canEquip_hook(RaceLimiter* self, GameData* item, RootObject* who)
	{
		// 武器キャビネットは武器スロットになっているので、チェック対象になっている。
		// 武器とクロスボウの制限を実装する際に問題となるので、無条件でtrueを返す。
		if (who->data->type == BUILDING)
			return true;

		auto target = who;
		if (who->data == KEP::externalGlobals->_TemporaryLimbInterface) // インベントリの義肢スロットはwhoがTemporaryLimbInterfaceになっているので、所有者を参照する。
			target = reinterpret_cast<TemporaryLimbInterface*>(who)->owner;

		bool isAnimal = target->getDataType() == CHARACTER && reinterpret_cast<Character*>(target)->isAnimal() != nullptr;

		return self->canEquip(item, target->getRace(), isAnimal);
	}

	RobotLimbItem* (*RobotLimbItem__CONTRUCTOR_orig)(RobotLimbItem*, GameData*, GameData*, hand&, int);
	RobotLimbItem* RobotLimbItem__CONTRUCTOR_hook(RobotLimbItem* self, GameData* baseData, GameData* materialData, hand _handle, int _level)
	{
		RobotLimbItem__CONTRUCTOR_orig(self, baseData, materialData, _handle, _level);
		RaceLimiter::getSingleton()->addLimit(baseData);
		return self;
	}

	void (*RaceLimiter_addLimit_orig)(RaceLimiter*, GameData* dat);
	void RaceLimiter_addLimit_hook(RaceLimiter* self, GameData* dat)
	{
		RaceLimiter_addLimit_orig(self, dat);

		auto limiterIter = self->limits.find(dat);
		if (limiterIter == self->limits.end())
			return;

		auto& raceGroupsExclude = dat->objectReferences["race groups exclude"];
		auto eIter = raceGroupsExclude.end();
		for (auto iter = raceGroupsExclude.begin(); iter != eIter; ++iter)
		{
			auto referenceData = iter->getPtr(&ou->gamedata);
			if (referenceData == nullptr)
				continue;

			auto& races = referenceData->objectReferences["races"];
			auto eRacesIter = races.end();
			for (auto racesIter = races.begin(); racesIter != eRacesIter; ++racesIter)
			{
				auto gameData = ou->gamedata.getData(racesIter->sid, RACE);
				if (gameData == nullptr)
					continue;

				auto race = RaceData::getRaceData(gameData);
				if (race == nullptr)
					continue;

				if (limiterIter->second.racesInclude.count(race) == 0)
					limiterIter->second.racesExclude.insert(race);
			}
		}

		auto& raceGroupsInclude = dat->objectReferences["race groups include"];
		eIter = raceGroupsInclude.end();
		for (auto iter = raceGroupsInclude.begin(); iter != eIter; ++iter)
		{
			auto referenceData = iter->getPtr(&ou->gamedata);
			if (referenceData == nullptr)
				continue;

			auto& races = referenceData->objectReferences["races"];
			auto eRacesIter = races.end();
			for (auto racesIter = races.begin(); racesIter != eRacesIter; ++racesIter)
			{
				auto gameData = ou->gamedata.getData(racesIter->sid, RACE);
				if (gameData == nullptr)
					continue;

				auto race = RaceData::getRaceData(gameData);
				if (race == nullptr)
					continue;

				limiterIter->second.racesInclude.insert(race);
			}
		}
	}
}

void KEP::RaceLimiterExtension::init()
{
	if (settings._raceEquipmentLimitsExtension)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress((bool (RaceLimiter::*)(GameData*, RootObject*))&RaceLimiter::_NV_canEquip), &RaceLimiter_canEquip_hook, &RaceLimiter_canEquip_orig))
			ErrorLog("[RaceLimiter::canEquip] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&RobotLimbItem::_CONSTRUCTOR), &RobotLimbItem__CONTRUCTOR_hook, &RobotLimbItem__CONTRUCTOR_orig))
			ErrorLog("[RobotLimbItem::RobotLimbItem] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&RaceLimiter::addLimit), &RaceLimiter_addLimit_hook, &RaceLimiter_addLimit_orig))
			ErrorLog("[RaceLimiter::addLimit] could not install hook!");
	}
}
