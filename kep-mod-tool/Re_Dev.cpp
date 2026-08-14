/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include "pch.h"
#include <Re_Dev.h>

#include <mygui/MyGUI.h>

#include <core/Functions.h>
#include <kenshi/Kenshi.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/InputHandler.h>
#include <kenshi/GameWorld.h>
#include <kenshi/gui/ForgottenGUI.h>
#include <kenshi/gui/OptionsWindow.h>
#include <kenshi/gui/MainBarGUI.h>
#include <kenshi/gui/MapScreen.h>
#include <kenshi/gui/DatapanelGUI.h>
#include <kenshi/gui/DataPanelLine.h>
#include <kenshi/CameraClass.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/Faction.h>
#include <kenshi/Building/Building.h>
#include <kenshi/Character.h>
#include <kenshi/CharStats.h>
#include <kenshi/CharMovement.h>
#include <kenshi/SensoryData.h>
#include <kenshi/StateBroadcastData.h>
#include <kenshi/Animation/AnimationClass.h>

#include <kep/functions.h>

#include <utility.h>
#include <DevToolsPanel.h>
#include <InformationPanel.h>
#include <GamePlayOptionsTab.h>

namespace
{
	void (*GameWorld_processKeys_orig)(GameWorld*);
	void GameWorld_processKeys_hook(GameWorld* self)
	{
		GameWorld_processKeys_orig(self);
		if (!gui->isLoadingMessageVisible() && !gui->isPaused())
		{
			for (auto iter = key->events.begin(); iter != key->events.end(); ++iter)
			{
				if ((*iter)->name == "toggle_devtools")
				{
					auto toolsPanel = KEP::tools::DevToolsPanel::getSingletonPtr();
					if (!toolsPanel->isVisible())
						toolsPanel->show();
					else
						toolsPanel->hide();
				}
				else if ((*iter)->name == "toggle_info_panel")
				{
					auto infoPanel = KEP::tools::InformationPanel::getSingletonPtr();
					if (!infoPanel->isVisible())
						infoPanel->show();
					else
						infoPanel->hide();
				}
			}
			KEP::tools::InformationPanel::getSingletonPtr()->refresh();
		}
	}

	bool (*EscMenu_openedOtherWindows_orig)(void*);
	bool EscMenu_openedOtherWindows_hook(void* self)
	{
		auto out = EscMenu_openedOtherWindows_orig(self);
		if (out)
		{
			KEP::tools::DevToolsPanel::getSingletonPtr()->hide();
			KEP::tools::InformationPanel::getSingletonPtr()->hide();
		}
		else
		{
			auto toolsPanel = KEP::tools::DevToolsPanel::getSingletonPtr();
			if (toolsPanel->isVisible())
			{
				toolsPanel->hide();
				out = true;
			}
			auto infoPanel = KEP::tools::InformationPanel::getSingletonPtr();
			if (infoPanel->isVisible())
			{
				infoPanel->hide();
				out = true;
			}
		}
		return out;
	}

	void (*CameraClass_restrictPosition_orig)(CameraClass*, lektor<Character*>&);
	void CameraClass_restrictPosition_hook(CameraClass* self, lektor<Character*>& objects)
	{
		if (!KEP::tools::DevToolsPanel::getSingletonPtr()->getGamePlayOptionsTab()->useLevelEditorCamera())
			CameraClass_restrictPosition_orig(self, objects);
	}

	void (*MainBarGUI_toggleInventory_orig)(MainBarGUI*, MyGUI::Widget*);
	void MainBarGUI_toggleInventory_hook(MainBarGUI* self, MyGUI::Widget* _sender)
	{
		if (KEP::tools::DevToolsPanel::getSingletonPtr()->getGamePlayOptionsTab()->enableNpcInventory())
		{
			auto character = gui->selectedObject.getCharacter();
			if (character != nullptr && !character->isPlayerCharacter())
			{
				if (gui->hasInventoryWindowOpen(gui->selectedObject))
				{
					gui->closeInventory(gui->selectedObject);
					return;
				}
				auto layout = character->createInventoryLayout();
				if (layout != nullptr)
				{
					gui->createInventoryWindow(gui->selectedObject, layout);
					gui->inventoryWindowNPC = gui->selectedObject;
				}
				return;
			}
		}
		MainBarGUI_toggleInventory_orig(self, _sender);
	}

	void (*Building_select_orig)(Building*);
	void Building_select_hook(Building* self)
	{
		Building_select_orig(self);
		if (KEP::tools::DevToolsPanel::getSingletonPtr()->getGamePlayOptionsTab()->enableNpcInventory())
			if (self->getBuildState()->isComplete && self->getFaction()->isPlayer == nullptr)
				gui->showInventoryBuilding(self->handle);
	}

	void (*MainBarGUI_autoChangeSelectedObject_orig)(MainBarGUI*, const hand&);
	void MainBarGUI_autoChangeSelectedObject_hook(MainBarGUI* self, const hand& obj)
	{
		MainBarGUI_autoChangeSelectedObject_orig(self, obj);
		if (KEP::tools::DevToolsPanel::getSingletonPtr()->getGamePlayOptionsTab()->enableNpcInventory())
		{
			auto building = obj.getBuilding();
			if (building != nullptr)
				if (building->getBuildState()->isComplete && building->getFaction()->isPlayer == nullptr)
					gui->showInventoryBuilding(obj);
		}
	}

	void (*MapScreen_mapMousePressed_orig)(MapScreen*, MyGUI::Widget*, int, int, MyGUI::MouseButton);
	void MapScreen_mapMousePressed_hook(MapScreen* self, MyGUI::Widget* _sender, int _left, int _top, MyGUI::MouseButton _id)
	{
		if (KEP::tools::DevToolsPanel::getSingletonPtr()->getGamePlayOptionsTab()->enableFastTravelMap() && _id.getValue() == MyGUI::MouseButton::Left && key->shift)
			return;
		MapScreen_mapMousePressed_orig(self, _sender, _left, _top, _id);
	}

	void (*MapScreen_mapMouseReleased_orig)(MapScreen*, MyGUI::Widget*, int, int, MyGUI::MouseButton);
	void MapScreen_mapMouseReleased_hook(MapScreen* self, MyGUI::Widget* _sender, int _left, int _top, MyGUI::MouseButton _id)
	{
		if (KEP::tools::DevToolsPanel::getSingletonPtr()->getGamePlayOptionsTab()->enableFastTravelMap() && _id.getValue() == MyGUI::MouseButton::Left && key->shift)
		{
			Ogre::Vector3 pos;
			if (self->getMouseWorldPosition(pos))
			{
				pos.y = UtilityT::getTerrainWithWaterHeight(pos.x, pos.z);
				ou->player->camera->teleport(pos);
			}

			return;
		}
		MapScreen_mapMouseReleased_orig(self, _sender, _left, _top, _id);
	}

	void (*ForgottenGUI_changeFontSize_orig)();
	void ForgottenGUI_changeFontSize_hook()
	{
		ForgottenGUI_changeFontSize_orig();
		if (KEP::tools::DevToolsPanel::initialized())
			KEP::tools::DevToolsPanel::getSingletonPtr()->create();
		if (KEP::tools::InformationPanel::initialized())
			KEP::tools::InformationPanel::getSingletonPtr()->create();

	}

	void (*OptionsWindow_create_orig)(OptionsWindow*);
	void OptionsWindow_create_hook(OptionsWindow* self)
	{
		OptionsWindow_create_orig(self);

		DatapanelGUI* controlsTab = nullptr;

		auto tabCount = self->tabs->getItemCount();
		for (size_t i = 0; i < tabCount; i++)
		{
			auto panel = self->tabs->getItemDataAt<DatapanelGUI*>(i, false);
			if (*panel != nullptr && (*panel)->currentCategory == 0x19)
			{
				controlsTab = *panel;
				break;
			}
		}

		if (controlsTab != nullptr)
		{
			controlsTab->addCustomLine(new DataPanelLine_KeyConfig("toggle_devtools", KEP::TranslationUtility::gettext("KEP: Developer tools"), 0x19));
			controlsTab->addCustomLine(new DataPanelLine_KeyConfig("toggle_info_panel", KEP::TranslationUtility::gettext("KEP: Information Panel"), 0x19));
			controlsTab->addSpace(0x19, 1.0f);
		}
	}

	void (*AppearanceManager_getEditorData_orig)(AppearanceManager*, ogre_unordered_map<RaceGroupData*, Ogre::FastArray<GameData*>>::type&, ogre_unordered_map<GameData*, ogre_unordered_map<AppearanceManager::Gender::Enum, AppearanceManager::AppearanceData>::type>::type&, bool, const Ogre::vector<GameDataReference>::type*);
	void AppearanceManager_getEditorData_hook(AppearanceManager* self, ogre_unordered_map<RaceGroupData*, Ogre::FastArray<GameData*>>::type& _racesGroups, ogre_unordered_map<GameData*, ogre_unordered_map<AppearanceManager::Gender::Enum, AppearanceManager::AppearanceData>::type>::type& _raceAppearanceData, bool playableOnly, const Ogre::vector<GameDataReference>::type* filter)
	{
		AppearanceManager_getEditorData_orig(self, _racesGroups, _raceAppearanceData, false, filter);
	}
}

void KEP::tools::initHook()
{
	void* AppearanceManager_getEditorData = nullptr;

	auto baseAddr = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));

	auto versionInfo = KenshiLib::GetKenshiVersion();
	auto platform = versionInfo.GetPlatform();
	auto version = versionInfo.GetVersion();
	if (platform == KenshiLib::BinaryVersion::UNKNOWN || (version != "1.0.65"))
	{
		ErrorLog("error!");
		return;
	}

	if (platform == KenshiLib::BinaryVersion::STEAM)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&AppearanceManager_getEditorData = baseAddr + 0x79b70;
		}
	}
	else if (platform == KenshiLib::BinaryVersion::GOG)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&AppearanceManager_getEditorData = baseAddr + 0x79b70;
		}
	}


	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&GameWorld::processKeys), &GameWorld_processKeys_hook, &GameWorld_processKeys_orig))
		ErrorLog("[GameWorld::processKeys] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KEP::functions->EscMenu_openedOtherWindows, &EscMenu_openedOtherWindows_hook, &EscMenu_openedOtherWindows_orig))
		ErrorLog("[EscMenu::openedOtherWindows] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&CameraClass::restrictPosition), (void(*)(CameraClass*, lektor<Character*>&))(&CameraClass_restrictPosition_hook), &CameraClass_restrictPosition_orig))
		ErrorLog("[CameraClass::restrictPosition] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&MainBarGUI::toggleInventory), &MainBarGUI_toggleInventory_hook, &MainBarGUI_toggleInventory_orig))
		ErrorLog("[MainBarGUI::toggleInventory] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&MapScreen::mapMousePressed), &MapScreen_mapMousePressed_hook, &MapScreen_mapMousePressed_orig))
		ErrorLog("[MapScreen::mapMousePressed] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&MapScreen::mapMouseReleased), &MapScreen_mapMouseReleased_hook, &MapScreen_mapMouseReleased_orig))
		ErrorLog("[MapScreen::mapMouseReleased] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&Building::_NV_select), &Building_select_hook, &Building_select_orig))
		ErrorLog("[Building::select] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&MainBarGUI::_NV_autoChangeSelectedObject), &MainBarGUI_autoChangeSelectedObject_hook, &MainBarGUI_autoChangeSelectedObject_orig))
		ErrorLog("[MainBarGUI::autoChangeSelectedObject] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&ForgottenGUI::changeFontSize), &ForgottenGUI_changeFontSize_hook, &ForgottenGUI_changeFontSize_orig))
		ErrorLog("[ForgottenGUI::changeFontSize] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(KenshiLib::GetRealAddress(&OptionsWindow::create), OptionsWindow_create_hook, &OptionsWindow_create_orig))
		ErrorLog("[OptionsWindow::create] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::QueueHook(AppearanceManager_getEditorData, AppearanceManager_getEditorData_hook, &AppearanceManager_getEditorData_orig))
		ErrorLog("[AppearanceManager::getEditorData] Could not add hook!");

	KenshiLib::ApplyQueuedHooks();
}
