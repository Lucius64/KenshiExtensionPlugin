/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <InventoryEx.h>

#include <ogre/OgrePrerequisites.h>

#include <core/Functions.h>
#include <Debug.h>
#include <kenshi/Inventory.h>

#include <Settings.h>

namespace
{
	void (*Inventory_getEquippedArmour_orig)(Inventory*, lektor<Item*>&);
	void Inventory_getEquippedArmour_hook(Inventory* self, lektor<Item*>& all)
	{
		Inventory_getEquippedArmour_orig(self, all);
		if (0 < KEP::settings._getEquippedArmour)
		{
			auto sectionIt = self->sections.find("belt");
			if (sectionIt != self->sections.end())
			{
				auto& items = sectionIt->second->items;
				for (auto iter = items.begin(); iter != items.end(); ++iter)
				{
					if (iter->item->isArmour() != nullptr)
						all.push_back(iter->item);
				}
			}
		}
		if (1 < KEP::settings._getEquippedArmour)
		{
			auto sectionIt = self->sections.find("eyes_eyes");
			if (sectionIt != self->sections.end())
			{
				auto& items = sectionIt->second->items;
				for (auto iter = items.begin(); iter != items.end(); ++iter)
				{
					if (iter->item->isArmour() != nullptr)
						all.push_back(iter->item);
				}
			}
			sectionIt = self->sections.find("eyes_belts");
			if (sectionIt != self->sections.end())
			{
				auto& items = sectionIt->second->items;
				for (auto iter = items.begin(); iter != items.end(); ++iter)
				{
					if (iter->item->isArmour() != nullptr)
						all.push_back(iter->item);
				}
			}
			sectionIt = self->sections.find("eyes_hats");
			if (sectionIt != self->sections.end())
			{
				auto& items = sectionIt->second->items;
				for (auto iter = items.begin(); iter != items.end(); ++iter)
				{
					if (iter->item->isArmour() != nullptr)
						all.push_back(iter->item);
				}
			}
			sectionIt = self->sections.find("neck");
			if (sectionIt != self->sections.end())
			{
				auto& items = sectionIt->second->items;
				for (auto iter = items.begin(); iter != items.end(); ++iter)
				{
					if (iter->item->isArmour() != nullptr)
						all.push_back(iter->item);
				}
			}
			sectionIt = self->sections.find("gloves");
			if (sectionIt != self->sections.end())
			{
				auto& items = sectionIt->second->items;
				for (auto iter = items.begin(); iter != items.end(); ++iter)
				{
					if (iter->item->isArmour() != nullptr)
						all.push_back(iter->item);
				}
			}
		}
	}
}

void KEP::InventoryEx::init()
{
	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Inventory::getEquippedArmour), &Inventory_getEquippedArmour_hook, &Inventory_getEquippedArmour_orig))
		ErrorLog("[Inventory::getEquippedArmour] could not install hook!");
}
