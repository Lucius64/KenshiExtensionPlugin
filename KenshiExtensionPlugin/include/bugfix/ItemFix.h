#pragma once

class GameData;
class hand;
class ContainerItem;
class GunClassPersonal;
class ActivePlatoon;
class Platoon;
class Building;
class MedicalSystem;
class Armour;
class BlueprintItem;
class InventorySection;
class SeveredLimbItem;

namespace KEP
{
	namespace ItemFix
	{
		void MedicalSystem_addArmour_hook(MedicalSystem* self, Armour* item);
		GunClassPersonal* GunClassPersonal__CONSTRUCTOR_hook(GunClassPersonal* self, GameData* baseData, float _level);
		void ContainerItem_setProperOwner_hook(ContainerItem* self, const hand& handle);
		void ActivePlatoon_refreshInventory_hook(ActivePlatoon* self, bool firstTime);
		void Building_setResidentSquad_hook(Building* self, Platoon* who);
		BlueprintItem* BlueprintItem__CONSTRUCTOR_hook(BlueprintItem* self, GameData* baseData, GameData* companyData, hand _handle);
		bool InventorySection_isLimitedSlotCompatible_hook(InventorySection* self, Item* item);
		void SeveredLimbItem_destroyPhysical_hook(SeveredLimbItem* self);
		void init();
	}
}