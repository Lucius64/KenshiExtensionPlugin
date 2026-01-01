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

#include <extern/RobotLimbItem.h>
#include <extern/TemporaryLimbInterface.h>
#include <extern/GunClass.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <RaceLimiterExtension.h>


namespace
{
	bool (*RaceLimiter_canEquip_orig)(RaceLimiter*, GameData*, RootObject*);
	RobotLimbItem* (*RobotLimbItem__CONTRUCTOR_orig)(RobotLimbItem*, GameData*, GameData*, hand&, int);
}

bool KEP::RaceLimiterExtension::RaceLimiter_canEquip_hook(RaceLimiter* self, GameData* item, RootObject* who)
{
	// 武器キャビネットは武器スロットになっているので、チェック対象になっている。
	// 武器とクロスボウの制限を実装する際に問題となるので、無条件でtrueを返す。
	if (who->data->type == BUILDING)
		return true;

	auto target = who;
	if (who->data == externalGlobals->_TemporaryLimbInterface) // インベントリの義肢スロットはwhoがTemporaryLimbInterfaceになっているので、所有者を参照する。
		target = reinterpret_cast<TemporaryLimbInterface*>(who)->owner;

	bool isAnimal = target->getDataType() == CHARACTER && reinterpret_cast<Character*>(target)->isAnimal() != nullptr;

	return self->canEquip(item, target->getRace(), isAnimal);
}

RobotLimbItem* KEP::RaceLimiterExtension::RobotLimbItem__CONTRUCTOR_hook(RobotLimbItem* self, GameData* baseData, GameData* materialData, hand _handle, int _level)
{
	RobotLimbItem__CONTRUCTOR_orig(self, baseData, materialData, _handle, _level);
	RaceLimiter::getSingleton()->addLimit(baseData);
	return self;
}

void KEP::RaceLimiterExtension::init()
{
	if (settings._raceEquipmentLimitsExtension)
	{
		bool (RaceLimiter::*ptrToRaceLimiterFunc)(GameData*, RootObject*) = &RaceLimiter::_NV_canEquip;
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(ptrToRaceLimiterFunc), &RaceLimiter_canEquip_hook, &RaceLimiter_canEquip_orig))
			ErrorLog("KenshiExtensionPlugin: [race limiter extension pt1] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_000CE290, &RobotLimbItem__CONTRUCTOR_hook, &RobotLimbItem__CONTRUCTOR_orig))
			ErrorLog("KenshiExtensionPlugin: [race limiter extension pt2] could not install hook!");
	}
}
