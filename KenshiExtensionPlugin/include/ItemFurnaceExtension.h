#pragma once

class FurnaceBuilding;
class StorageBuilding;
class Item;
class Inventory;
class GameData;
template<typename T>
class lektor;
class AI;
class hand;
class Task_FillMachine;

namespace KEP
{
	namespace ItemFurnaceExtension
	{
		void FurnaceBuilding_setupFromData_hook(FurnaceBuilding* self);
		float FurnaceBuilding_FUN_002A5F00_hook(FurnaceBuilding* self, Item* input);
		void FurnaceBuilding_FUN_0029C810_hook(FurnaceBuilding* self, lektor<GameData*>& out);
		bool AI_haveSomeResourcesFor_hook(AI* self, hand& handle);
		int Task_FillMachine_FUN_00340EB0_hook(Task_FillMachine* self, StorageBuilding* storage, Inventory* inventory);

		void init();
	}
}