#include <intrin.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/util/hand.h>
#include <kenshi/GlobalConstants.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/Faction.h>
#include <kenshi/RootObject.h>
#include <kenshi/Item.h>
#include <kenshi/Inventory.h>
#include <kenshi/Platoon.h>
#include <kenshi/Building.h>
#include <kenshi/Gear.h>
#include <kenshi/MedicalSystem.h>

#include <extern/GunClass.h>
#include <extern/BuildingInterior.h>
#include <extern/BlueprintItem.h>
#include <extern/StorageBuilding.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <bugfix/ItemFix.h>



namespace
{
	void (*MedicalSystem_addArmour_orig)(MedicalSystem*, Armour*);
	GunClassPersonal* (*GunClassPersonal__CONSTRUCTOR_orig)(GunClassPersonal*, GameData*, float);
	void (*ContainerItem_setProperOwner_orig)(ContainerItem*, const hand&);
	void (*ActivePlatoon_refreshInventory_orig)(ActivePlatoon*, bool);
	void (*Building_setResidentSquad_orig)(Building*, Platoon*);
	BlueprintItem* (*BlueprintItem__CONSTRUCTOR_orig)(BlueprintItem*, GameData*, GameData*, hand&);
	bool (*InventorySection_isLimitedSlotCompatible_orig)(InventorySection*, Item*);
}

void KEP::ItemFix::MedicalSystem_addArmour_hook(MedicalSystem* self, Armour* item)
{
	if (!settings._ChangeArmorDamageReductionOrder)
	{
		MedicalSystem_addArmour_orig(self, item);
		return;
	}

	for (uint32_t i = 0; i < self->armourList.size(); ++i)
	{
		if (item->cutToStun < self->armourList[i]->cutToStun)
		{
			self->armourList.insert(item, i);
			return;
		}
	}
	self->armourList.push_back(item);
}

GunClassPersonal* KEP::ItemFix::GunClassPersonal__CONSTRUCTOR_hook(GunClassPersonal* self, GameData* baseData, float _level)
{
	GunClassPersonal__CONSTRUCTOR_orig(self, baseData, _level);

	if (settings._fixGlobalDamageMultiplier)
	{
		self->pierceDamageMin = static_cast<int>(lerp(_level, con->BOW_DAMAGE_0 * baseData->fdata["pierce damage min 0"], con->BOW_DAMAGE_99 * baseData->fdata["pierce damage min 1"]));
		self->pierceDamageMax = static_cast<int>(lerp(_level, con->BOW_DAMAGE_0 * baseData->fdata["pierce damage max 0"], con->BOW_DAMAGE_99 * baseData->fdata["pierce damage max 1"]));
	}

	return self;
}

void KEP::ItemFix::ContainerItem_setProperOwner_hook(ContainerItem* self, const hand& handle)
{
	if (!settings._fixItemBecomingStolenGoodsFromPlayerBug)
	{
		ContainerItem_setProperOwner_orig(self, handle);
		return;
	}

	uintptr_t retAddr = reinterpret_cast<uintptr_t>(_ReturnAddress());

	auto newOwner = handle.getRootObject();
	Faction* newOwnerFaction = newOwner != nullptr ? newOwner->getFaction() : nullptr;

	if (retAddr != externalGlobals->_dropItemReturnAddress || newOwnerFaction == nullptr || newOwnerFaction->isPlayer == nullptr)
	{
		auto inventory = self->getInventory();
		for (auto iter = inventory->_allItems.begin(); iter != inventory->_allItems.end(); ++iter)
		{
			auto& itemOwnerHandle = (*iter)->getProperOwner();
			if (itemOwnerHandle.type == NULL_ITEM)
			{
				(*iter)->setProperOwner(handle);
			}
			else if (handle.type == NULL_ITEM)
			{
				auto& containerOwnerHandle = self->getProperOwner();
				if (itemOwnerHandle == containerOwnerHandle)
				{
					(*iter)->setProperOwner(handle);
				}
				else
				{
					auto containerOwner = containerOwnerHandle.getRootObject();
					auto itemOwner = itemOwnerHandle.getRootObject();
					if (containerOwner != nullptr &&
						itemOwner != nullptr &&
						containerOwner->getFaction() == itemOwner->getFaction())
					{
						(*iter)->setProperOwner(handle);
					}
				}
			}
		}
	}

	self->properOwner.type = handle.type;
	self->properOwner.container = handle.container;
	self->properOwner.containerSerial = handle.containerSerial;
	self->properOwner.index = handle.index;
	self->properOwner.serial = handle.serial;

	return;
}


void KEP::ItemFix::ActivePlatoon_refreshInventory_hook(ActivePlatoon* self, bool firstTime)
{
	if (firstTime && settings._fixHousemateInventoryRefresh)
	{
		auto& homeBuildingHandle = self->me->getOwnerships()->_homeBuilding;
		if (homeBuildingHandle.type != NULL_ITEM)
		{
			auto homeBuilding = homeBuildingHandle.getBuilding();
			if (homeBuilding != nullptr &&
				homeBuilding->myInterior != nullptr &&
				homeBuilding->myInterior->physicsCollection == nullptr &&
				(homeBuilding->residentSquad.container != self->me->handle.container || homeBuilding->residentSquad.containerSerial != self->me->handle.containerSerial))
			{
				hand handle = homeBuilding->residentSquad;

				homeBuilding->residentSquad.type = self->me->handle.type;
				homeBuilding->residentSquad.container = self->me->handle.container;
				homeBuilding->residentSquad.containerSerial = self->me->handle.containerSerial;
				homeBuilding->residentSquad.index = self->me->handle.index;
				homeBuilding->residentSquad.serial = self->me->handle.serial;

				ActivePlatoon_refreshInventory_orig(self, firstTime);
				homeBuilding->residentSquad.type = handle.type;
				homeBuilding->residentSquad.container = handle.container;
				homeBuilding->residentSquad.containerSerial = handle.containerSerial;
				homeBuilding->residentSquad.index = handle.index;
				homeBuilding->residentSquad.serial = handle.serial;

				auto residentsPlatoon = homeBuilding->residentSquad.getPlatoon();
				if (residentsPlatoon != nullptr && residentsPlatoon->activePlatoon != nullptr && externalFunctions->FUN_00958550(residentsPlatoon->activePlatoon))
				{
					auto residentsLeader = residentsPlatoon->getSquadLeader();
					if (residentsLeader != nullptr)
					{
						auto trader = externalFunctions->FUN_004FDE00();
						externalFunctions->FUN_0095A340(trader, homeBuilding, residentsPlatoon->squadTemplate, residentsPlatoon, true, false);
					}
				}
				return;
			}
		}
	}

	ActivePlatoon_refreshInventory_orig(self, firstTime);
}


void KEP::ItemFix::Building_setResidentSquad_hook(Building* self, Platoon* who)
{
	if (!settings._fixHousemateInventoryRefresh)
	{
		Building_setResidentSquad_orig(self, who);
		return;
	}

	// residentsの上書き条件
	// 1. residentsの型がNULL_ITEM
	// 2. residentsの部隊がnull
	// 3. handleCがresidents <= 設定対象 and 設定対象のseparatedの型がNULL_ITEM
	if (who == nullptr)
		return;

	hand& currentResident = self->residentSquad;
	if (self->residentSquad.type == NULL_ITEM || self->residentSquad.getPlatoon() == nullptr || who->isSeparatedSquad.type == NULL_ITEM && self->residentSquad.container <= who->handle.container)
	{
		Building_setResidentSquad_orig(self, who);
	}
	else
	{
		auto ownerships = who->getOwnerships();
		if (ownerships->_homeBuilding != self->handle)
			ownerships->setHomeBuilding(self->handle, who->squadType);

		return;
	}
}


BlueprintItem* KEP::ItemFix::BlueprintItem__CONSTRUCTOR_hook(BlueprintItem* self, GameData* baseData, GameData* companyData, hand _handle)
{
	BlueprintItem__CONSTRUCTOR_orig(self, baseData, companyData, _handle);

	if (settings._fixBlueprintTextures)
	{
		auto materialData = ou->gamedata.getData(baseData->getFromList("material", 0), MATERIAL_SPECS_CLOTHING);
		if (materialData != nullptr)
			self->materialData = materialData;
	}

	return self;
}

bool KEP::ItemFix::InventorySection_isLimitedSlotCompatible_hook(InventorySection* self, Item* item)
{
	if (!settings._fixItemTypeLimit)
		return InventorySection_isLimitedSlotCompatible_orig(self, item);

	auto itemGamedata = item->getGameData();
	if (self->veryLimitedSlot.size() != 0)
	{
		for (auto iter = self->veryLimitedSlot.begin(); iter != self->veryLimitedSlot.end(); ++iter)
		{
			if (*iter == itemGamedata)
				return true;
		}
		return false;
	}

	if (self->limitedSlot != ATTACH_NONE && self->callbackObject != nullptr)
	{
		if (!RaceLimiter::getSingleton()->canEquip(itemGamedata, self->callbackObject))
			return false;
	}

	if (self->armourOnly)
		return itemGamedata->type == ARMOUR;

	if (self->parentInventory->owner != nullptr && self->parentInventory->owner->data->type == BUILDING)
	{
		auto storage = reinterpret_cast<Building*>(self->parentInventory->owner)->getFunctionStuff();
		if (storage != nullptr && (storage->itemtypeLimit == CROSSBOW || storage->itemtypeLimit == LIMB_REPLACEMENT || storage->itemtypeLimit == CONTAINER || storage->itemtypeLimit == NEST_ITEM || storage->itemtypeLimit == MAP_ITEM ))
			return storage->itemtypeLimit == itemGamedata->type;
	}

	if (self->limitedSlot == ATTACH_NONE)
		return true;

	return self->limitedSlot == item->slotType;
}

void KEP::ItemFix::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&MedicalSystem::addArmour), &MedicalSystem_addArmour_hook, &MedicalSystem_addArmour_orig))
		ErrorLog("KenshiExtensionPlugin: [cut efficiency] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_0043BDE0, &GunClassPersonal__CONSTRUCTOR_hook, &GunClassPersonal__CONSTRUCTOR_orig))
		ErrorLog("KenshiExtensionPlugin: [crossbow global damage multiplier] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&ContainerItem::_NV_setProperOwner), &ContainerItem_setProperOwner_hook, &ContainerItem_setProperOwner_orig))
		ErrorLog("KenshiExtensionPlugin: [drop backpack] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&ActivePlatoon::refreshInventory), &ActivePlatoon_refreshInventory_hook, &ActivePlatoon_refreshInventory_orig))
		ErrorLog("KenshiExtensionPlugin: [refresh inventory pt1] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Building::setResidentSquad), &Building_setResidentSquad_hook, &Building_setResidentSquad_orig))
		ErrorLog("KenshiExtensionPlugin: [refresh inventory pt2] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_002B7860, &BlueprintItem__CONSTRUCTOR_hook, &BlueprintItem__CONSTRUCTOR_orig))
		ErrorLog("KenshiExtensionPlugin: [blueprint material] could not install hook!");

	bool (InventorySection::*ptrToInventorySectionFunc)(Item*) = &InventorySection::isLimitedSlotCompatible;
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(ptrToInventorySectionFunc), &InventorySection_isLimitedSlotCompatible_hook, &InventorySection_isLimitedSlotCompatible_orig))
		ErrorLog("KenshiExtensionPlugin: [itemtype limit] could not install hook!");
}
