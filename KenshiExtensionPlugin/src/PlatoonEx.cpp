/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <PlatoonEx.h>

#include <ogre/OgrePrerequisites.h>

#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/Faction.h>
#include <kenshi/Platoon.h>
#include <kenshi/Character.h>
#include <kenshi/Inventory.h>
#include <kenshi/AI/AI.h>
#include <extern/TradingInfo.h>
#include <extern/InventoryManager.h>

#include <kep/functions.h>
#include <UtilityFunction.h>
#include <Settings.h>

namespace
{
	class PlatoonStateEx
	{
	public:
		PlatoonStateEx();
		TimeOfDay wagePaymentTime;
	};

	PlatoonStateEx::PlatoonStateEx()
		: wagePaymentTime(0.0)
	{
	}

	ogre_unordered_map<hand, PlatoonStateEx>::type platoonStateExMap;

	void (*GameWorld_loadAllPlatoons_orig)(GameWorld*);
	void GameWorld_loadAllPlatoons_hook(GameWorld* self)
	{
		GameWorld_loadAllPlatoons_orig(self);
		platoonStateExMap.clear();
	}

	GameData* (*Platoon_serialiseEverything_orig)(Platoon*, GameDataContainer*, bool);
	GameData* Platoon_serialiseEverything_hook(Platoon* self, GameDataContainer* dataContainer, bool levelEditor)
	{
		auto platoonData = Platoon_serialiseEverything_orig(self, dataContainer, levelEditor);
		if (KEP::settings._shoppingEx && platoonData != nullptr && !self->squadTemplate->bdata["is trader"])
		{
			auto& stateEx = platoonStateExMap[self->handle];
			platoonData->fdata["wage payment time"] = static_cast<float>(stateEx.wagePaymentTime.time);
		}
		return platoonData;
	}

	void (*Platoon_loadStateData_orig)(Platoon*, GameData*);
	void Platoon_loadStateData_hook(Platoon* self, GameData* state)
	{
		Platoon_loadStateData_orig(self, state);
		if (!KEP::settings._shoppingEx || self->squadTemplate->bdata["is trader"])
			return;

		auto& stateEx = platoonStateExMap[self->handle];
		if (state != nullptr)
		{
			auto tod = ou->getTimeStamp_inGameHours();
			stateEx.wagePaymentTime.time = self->data->fdata["wage payment time"];
			if (stateEx.wagePaymentTime.time <= 0.0)
				stateEx.wagePaymentTime.time = tod.time + 24.0f;
		}
		else
		{
			auto tod = ou->getTimeStamp_inGameHours();
			stateEx.wagePaymentTime.time = tod.time + 24.0f;
		}
	}

	void _getAllItemsWithFunction(InventorySection* section, lektor<Item*>& out, ItemFunction type)
	{
		if (section == nullptr)
			return;

		for (auto iter = section->items.begin(); iter != section->items.end(); ++iter)
		{
			if (iter->item->itemFunction == type)
				out.push_back(iter->item);
		}
	}

	bool (*ActivePlatoon_update_orig)(ActivePlatoon*);
	bool ActivePlatoon_update_hook(ActivePlatoon* self)
	{
		bool success = ActivePlatoon_update_orig(self);
		if (KEP::settings._shoppingEx && success && !self->getIsTrader() && self->isPlayer == nullptr)
		{
			auto& platoonStateEx = platoonStateExMap[self->me->handle];
			auto tod = ou->getTimeStamp_inGameHours();
			if (platoonStateEx.wagePaymentTime.time < tod.time)
			{
				platoonStateEx.wagePaymentTime.time = tod.time + 24.0f;
				int money = 0;
				for (auto iter = self->things.begin(); iter != self->things.end(); ++iter)
				{
					int wages = (*iter)->getGameData()->idata["wages"];
					if (money < wages)
						money = wages;
				}

				int maxMoney = money * 20;
				if (maxMoney < 0)
					maxMoney = 2000000000;
				self->me->ownerships.money += money;

				if (maxMoney < self->me->ownerships.money || self->me->ownerships.money < 0)
					self->me->ownerships.money = maxMoney;
			}
		}

		return success;
	}

	bool (*AI_buySomething_orig)(AI*, Building*);
	bool AI_buySomething_hook(AI* self, Building* shop)
	{
		if (KEP::settings._shoppingEx && !self->me->isPlayerCharacter())
		{
			ogre_unordered_map<ItemFunction, int>::type numConsumption;
			for (auto iter = self->tradingInfo->tradeItems.begin(); iter != self->tradingInfo->tradeItems.end(); ++iter)
			{
				numConsumption[iter->itemFunction] += iter->count;
			}

			auto inv = self->me->getInventory();
			auto backpack = self->me->hasABackpackOn();
			auto backpackInv = backpack != nullptr ? backpack->getInventory() : nullptr;
			lektor<Item*> mainItems;
			lektor<Item*> backpackItems;
			for (auto iter = numConsumption.begin(); iter != numConsumption.end(); ++iter)
			{
				mainItems.clear();
				backpackItems.clear();

				if (iter->second < 1)
					continue;

				auto itemFunction = iter->first;

				if (backpackInv != nullptr)
					backpackInv->getAllItemsWithFunction(backpackItems, itemFunction);

				if (inv != nullptr)
					inv->getAllItemsWithFunction(mainItems, itemFunction);

				int numItem = mainItems.size() + backpackItems.size();
				if (numItem <= iter->second && (itemFunction == ITEM_FOOD || itemFunction == ITEM_FIRSTAID || itemFunction == ITEM_ROBOTREPAIR || itemFunction == ITEM_MEDRIGGING || itemFunction == ITEM_AMMO))
				{
					iter->second = numItem - 1;

					if (iter->second < 1)
						continue;
				}

				for (auto itemIt = backpackItems.begin(); itemIt != backpackItems.end(); ++itemIt)
				{
					auto item = *itemIt;
					if (item->isEquipped || item->deathItem)
						continue;

					if (iter->second <= item->quantity)
					{
						backpackInv->removeItemAutoDestroy(item, iter->second);
						iter->second = 0;
					}
					else
					{
						iter->second -= item->quantity;
						backpackInv->removeItemAutoDestroy(item, item->quantity);
					}

					if (iter->second < 1)
						break;
				}

				for (auto itemIt = mainItems.begin(); itemIt != mainItems.end(); ++itemIt)
				{
					auto item = *itemIt;
					if (item->isEquipped || item->deathItem)
						continue;

					if (iter->second <= item->quantity)
					{
						inv->removeItemAutoDestroy(item, iter->second);
						iter->second = 0;
					}
					else
					{
						iter->second -= item->quantity;
						inv->removeItemAutoDestroy(item, item->quantity);
					}

					if (iter->second < 1)
						break;
				}
			}
		}
		return AI_buySomething_orig(self, shop);
	}

	bool _hasBackpack(RootObject* obj)
	{
		auto inv = obj->getInventory();
		if (inv != nullptr)
		{
			auto section = inv->getSectionOfType(ATTACH_BACKPACK);
			if (section != nullptr && !section->isEmpty())
			{
				auto containerInv = section->items[0].item->getInventory();
				if (containerInv != nullptr && containerInv->sectionsInSearchOrder.size() != 0)
					return true;
			}
		}
		return false;
	}

	void (*InventoryManager_refreshSquadInventory_orig)(InventoryManager*, ActivePlatoon*);
	void InventoryManager_refreshSquadInventory_hook(InventoryManager* self, ActivePlatoon* _platoon)
	{
		InventoryManager_refreshSquadInventory_orig(self, _platoon);
		if (KEP::settings._shoppingEx && _platoon != nullptr && KEP::functions->VendorListManager_hasVendor(_platoon) && _platoon->me->getSquadLeader() != nullptr)
		{
			bool hasBackpack = false;
			for (auto iter = _platoon->things.begin(); iter != _platoon->things.end(); ++iter)
			{
				if (_hasBackpack(*iter))
				{
					hasBackpack = true;
					break;
				}
			}
			if (hasBackpack)
			{
				int money = _platoon->me->squadTemplate->idata["vendor money"];
				if (0 < money)
				{
					int deviation = static_cast<int>(money * 0.4f);
					_platoon->me->ownerships.money = static_cast<int>((money + UtilityT::randomInt(-deviation, deviation)) * _platoon->me->getFaction()->getProsperityMultiplier());
				}
			}
		}
	}
}

void KEP::PlatoonEx::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&GameWorld::loadAllPlatoons), &GameWorld_loadAllPlatoons_hook, &GameWorld_loadAllPlatoons_orig))
		ErrorLog("[GameWorld::loadAllPlatoons] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Platoon::_NV_serialiseEverything), &Platoon_serialiseEverything_hook, &Platoon_serialiseEverything_orig))
		ErrorLog("[Platoon::serialiseEverything] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Platoon::_NV_loadStateData), &Platoon_loadStateData_hook, &Platoon_loadStateData_orig))
		ErrorLog("[Platoon::loadStateData] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&ActivePlatoon::_NV_update), &ActivePlatoon_update_hook, &ActivePlatoon_update_orig))
		ErrorLog("[ActivePlatoon::update] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&AI::buySomething), &AI_buySomething_hook, &AI_buySomething_orig))
		ErrorLog("[AI::buySomething] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->InventoryManager_refreshSquadInventory, &InventoryManager_refreshSquadInventory_hook, &InventoryManager_refreshSquadInventory_orig))
		ErrorLog("[InventoryManager::refreshSquadInventory] could not install hook!");
}
