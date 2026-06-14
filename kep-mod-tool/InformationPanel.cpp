#include "pch.h"
#include <InformationPanel.h>

#include <boost/scoped_ptr.hpp>

#include <ogre/OgreStringConverter.h>
#include <mygui/MyGUI.h>

#include <Debug.h>
#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/gui/ForgottenGUI.h>
#include <kenshi/gui/DatapanelGUI.h>
#include <kenshi/gui/DataPanelLine.h>
#include <kenshi/Faction.h>
#include <kenshi/Platoon.h>
#include <kenshi/Character.h>
#include <kenshi/RaceData.h>
#include <kenshi/SensoryData.h>
#include <kenshi/Inventory.h>
#include <kenshi/Building/DoorStuff.h>
#include <kenshi/CombatClass.h>

#include <extern/InventoryManager.h>
#include <extern/UniqueNPCManager.h>
#include <extern/StateBroadcastData.h>

#include <kep/translation.h>
#include <kep/functions.h>
#include <kep/utility.h>

namespace
{
	bool _initialized = false;
	float infoPanelCoolTime = 0.75f;

	const int cat_building = 1;
	const int cat_character = 2;
	const int cat_combat = 9;
	const int cat_vips = 10;

	std::string getWorldStateEnumName(WorldStateEnum what)
	{
		switch (what)
		{
		case DEAD:
			return "Dead";
		case ALIVE:
			return "Living";
		case IMPRISONED:
			return "Imprisoned";
		default:
			return "unknown";
		}
	}

	std::string getCharacterTypeEnumName(CharacterTypeEnum what)
	{
		switch (what)
		{
		case OT_NONE:
			return "no type";
		case OT_LAW_ENFORCEMENT:
			return "law enforcer";
		case OT_MILITARY:
			return "military";
		case OT_TRADER:
			return "trader";
		case OT_CIVILIAN:
			return "civilian";
		case OT_DIPLOMAT:
			return "diplomat";
		case OT_SLAVE:
			return "slave";
		case OT_SLAVER:
			return "slaver";
		case OT_BANDIT:
			return "bandit";
		case OT_ADVENTURER:
			return "adventurer";
		case OT_END:
			return "end";
		default:
			return "unknown";
		}
	}

	std::string getPersonalityTagsName(PersonalityTags what)
	{
		switch (what)
		{
		case PT_HONORABLE:
			return "honorable";
		case PT_TRAITOROUS:
			return "traitorous";
		case PT_SMART:
			return "smart";
		case PT_DUMB:
			return "dumb";
		case PT_BRAVE:
			return "brave";
		case PT_FEARFUL:
			return "fearful";
		case PT_WARM_KIND:
			return "warm kind";
		case PT_COLD_CRUEL:
			return "cold cruel";
		case PT_NORMAL:
			return "normal";
		case PT_MANIACAL:
			return "maniac";
		default:
			return "unknown";
		}
	}

	std::string getSwordStateEnumName(swordStateEnum what)
	{
		switch (what)
		{
		case CHOP_WEAPON:
			return "chop";
		case BLOCK:
			return "block";
		case REACTION_BLOCK:
			return "reaction block";
		case STARTUP_STATE:
		case DECISION:
			return "Decision";
		case CIRCLE_MENACINGLY:
			return "circle";
		case WAIT_MENACINGLY:
			return "Wait";
		case STUMBLE:
			return "stumble";
		case TARGET_PATHFINDING:
			return "pathfinding to target";
		default:
			return "none";
		}
	}

}

KEP::tools::InformationPanel* KEP::tools::InformationPanel::getSingletonPtr()
{
	static boost::scoped_ptr<InformationPanel> singleton(new InformationPanel());
	return singleton.get();
}

bool KEP::tools::InformationPanel::initialized()
{
	return _initialized;
}

KEP::tools::InformationPanel::InformationPanel()
	: _panel(nullptr)
{
	create();
	_initialized = true;
}

void KEP::tools::InformationPanel::create()
{
	if (this->_panel != nullptr)
	{
		this->_panel->show(false);
		this->_lastSelected.setNull();
		gui->destroy(this->_panel);
	}

	this->_panel = gui->createDatapanel(0.05f, 0.05f, 0.28f, 0.58f, true, "Window", true);
	this->_panel->setCaption(KEP::TranslationUtility::gettext("Information Panel"));
	this->_panel->setPanelName("Information");
	this->_panel->setLineSpacing(32.0f);

	this->_panel->showTabs(true);
	this->_panel->changeCategory(cat_building);
	this->_panel->addTab(cat_building, KEP::TranslationUtility::gettext_main("Buildings"), "");
	this->_panel->addTab(cat_character, KEP::TranslationUtility::gettext_main("Characters"), "");
	this->_panel->addTab(cat_combat, KEP::TranslationUtility::gettext("Combat"), "");
	this->_panel->addTab(cat_vips, KEP::TranslationUtility::gettext("VIPs"), "");
	this->_panel->show(false);
}

void KEP::tools::InformationPanel::refresh()
{
	infoPanelCoolTime -= KEP::functions->getTime();
	if (infoPanelCoolTime <= 0.0f)
	{
		infoPanelCoolTime = 0.75f;
		if (this->_panel->isVisible())
		{
			if (this->_lastSelected != gui->selectedObject)
			{
				this->_panel->clearPage(cat_building);
				this->_panel->clearPage(cat_character);
				this->_panel->clearPage(cat_vips);

				_lastSelected = gui->selectedObject;
			}
			_displayBuildingInformation();
			_displayCharacterInformation();
			_displayCombatInformation();
			_displayUniqueNpcInformation();
		}
	}
}

void KEP::tools::InformationPanel::show()
{
	this->_panel->show(true);
}

void KEP::tools::InformationPanel::hide()
{
	this->_panel->show(false);
	this->_lastSelected.setNull();
}

bool KEP::tools::InformationPanel::isVisible()
{
	return this->_panel->isVisible();
}

void KEP::tools::InformationPanel::_displayBuildingInformation()
{
	this->_panel->clearPage(cat_building);
	if (this->_panel->getCurrentCategory() != cat_building)
		return;

	auto building = this->_lastSelected.getBuilding();
	if (building == nullptr)
	{
		this->_panel->setLine(KEP::GUIColor::getMain() + "The selected object is invalid.", "", cat_building, false, true);
		return;
	}

	if (building->imADoor)
		building = building->doorStuff()->doorParentBuilding();

	if (building == nullptr)
	{
		this->_panel->setLine(KEP::GUIColor::getMain() + "The selected object is invalid.", "", cat_building, false, true);
		return;
	}

	this->_panel->setLine(KEP::GUIColor::getMain() + "Handle:", KEP::GUIColor::getMain() + building->handle.toString(), cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "Name", KEP::GUIColor::getMain() + building->displayName, cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "Faction", KEP::GUIColor::getMain() + building->getFaction()->name, cat_building, false, true);

	std::string residentTemplate = building->residentSquadTemplate != nullptr ? building->residentSquadTemplate->stringID : "nothing";
	this->_panel->setLine(KEP::GUIColor::getMain() + "Resident SquadTemplate", KEP::GUIColor::getMain() + residentTemplate, cat_building, false, true);

	auto residentPlatoon = building->residentSquad.getPlatoon();
	if (residentPlatoon != nullptr)
	{
		this->_panel->setLine(KEP::GUIColor::getMain() + "Resident", KEP::GUIColor::getMain() + residentPlatoon->displayName, cat_building, false, true);

		auto residentLeader = residentPlatoon->squadleader.getCharacter();
		if (residentLeader != nullptr)
		{
			this->_panel->setLine(KEP::GUIColor::getMain() + "Leader", KEP::GUIColor::getMain() + residentLeader->displayName, cat_building, false, true);
		}

		auto ownerFaction = residentPlatoon->getFaction();

		lektor<Platoon*> squads;
		ownerFaction->getAllSquadsThatOwn(squads, building);
		if (squads.size() != 0)
		{
			this->_panel->setLine(KEP::GUIColor::getMain() + "Owners", "", cat_building, false, true);
			for (auto iter = squads.begin(); iter != squads.end(); ++iter)
			{
				auto leader = (*iter)->squadleader.getCharacter();
				std::string leaderName = leader != nullptr ? leader->displayName : "nothing";

				this->_panel->setLine(KEP::GUIColor::getMain() + (*iter)->displayName, KEP::GUIColor::getMain() + "leader: " + leaderName, cat_building, false, true);
			}
		}
	}
	this->_panel->addSpace(cat_building, 0.5f);

	if (!building->hasInterior())
		return;

	Ogre::vector<InventoryManager::BuildingInventoryInfo>::type buildingInventoryList;
	auto invMgr = KEP::functions->InventoryManager_getSingleton();
	KEP::functions->InventoryManager_getBuildingInventoryList(invMgr, building, &buildingInventoryList);

	int numItems = 0;
	int numMapItems = 0;
	int numNestItems = 0;
	int numWeapons = 0;
	int numArmours = 0;
	int numBackpacks = 0;
	int numCrossbows = 0;
	int numRobotLimbs = 0;
	int numBlueprints = 0;
	int numUniques = 0;

	for (auto iter = buildingInventoryList.begin(); iter != buildingInventoryList.end(); ++iter)
	{
		for (auto itemIter = iter->inventory->_allItems.begin(); itemIter != iter->inventory->_allItems.end(); ++itemIter)
		{
			if ((*itemIter)->itemFunction == ITEM_MONEY)
				continue;

			switch ((*itemIter)->objectType)
			{
			case ITEM:
				numItems += (*itemIter)->quantity;
				break;
			case MAP_ITEM:
				numMapItems += (*itemIter)->quantity;
				break;
			case NEST_ITEM:
				numNestItems += (*itemIter)->quantity;
				break;
			case WEAPON:
				numWeapons += (*itemIter)->quantity;
				break;
			case ARMOUR:
				numArmours += (*itemIter)->quantity;
				break;
			case CONTAINER:
				numBackpacks += (*itemIter)->quantity;
				break;
			case CROSSBOW:
				numCrossbows += (*itemIter)->quantity;
				break;
			case LIMB_REPLACEMENT:
				numRobotLimbs += (*itemIter)->quantity;
				break;
			case RESEARCH:
				numBlueprints += (*itemIter)->quantity;
				break;
			}
			if ((*itemIter)->isUnique)
				numUniques += (*itemIter)->quantity;
		}
	}

	int numAllItems = numItems + numMapItems + numNestItems + numWeapons + numArmours + numBackpacks + numCrossbows + numRobotLimbs + numBlueprints;

	this->_panel->setLine(KEP::GUIColor::getMain() + "Item count in the inventory:", "", cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "all", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(numAllItems), cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "item", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(numItems), cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "weapon", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(numWeapons), cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "armour", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(numArmours), cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "backpack", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(numBackpacks), cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "crossbow", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(numCrossbows), cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "robot limb", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(numRobotLimbs), cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "blueprint", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(numBlueprints), cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "map", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(numMapItems), cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "nest", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(numNestItems), cat_building, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "unique", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(numUniques), cat_building, false, true);

	this->_panel->addSpace(cat_building, 0.5f);
}

void KEP::tools::InformationPanel::_displayCharacterInformation()
{
	this->_panel->clearPage(cat_character);
	if (this->_panel->getCurrentCategory() != cat_character)
		return;

	auto obj = this->_lastSelected.getCharacter();
	if (obj == nullptr)
	{
		this->_panel->setLine(KEP::GUIColor::getMain() + "The selected object is invalid.", "", cat_character, false, true);
		return;
	}

	this->_panel->setLine(KEP::GUIColor::getMain() + "-----------CHARACTER-------------", "", cat_character, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "Handle:", KEP::GUIColor::getMain() + obj->handle.toString(), cat_character, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "Name:", KEP::GUIColor::getMain() + obj->displayName, cat_character, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "Template:", KEP::GUIColor::getMain() + obj->getGameData()->stringID, cat_character, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "Faction:", KEP::GUIColor::getMain() + obj->getFaction()->name, cat_character, false, true);

	auto race = obj->getRace();
	this->_panel->setLine(KEP::GUIColor::getMain() + "Race:", KEP::GUIColor::getMain() + race->data->name, cat_character, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "NPC type:", KEP::GUIColor::getMain() + getCharacterTypeEnumName(obj->stateBroadcast->npcClass), cat_character, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "Personality:", KEP::GUIColor::getMain() + getPersonalityTagsName(obj->stateBroadcast->personality), cat_character, false, true);

	this->_panel->addSpace(cat_character, 0.5f);
}

void KEP::tools::InformationPanel::_displayCombatInformation()
{
	this->_panel->clearPage(cat_combat);
	if (this->_panel->getCurrentCategory() != cat_combat)
		return;

	auto obj = this->_lastSelected.getCharacter();
	if (obj == nullptr)
	{
		this->_panel->setLine(KEP::GUIColor::getMain() + "The selected object is invalid.", "", cat_combat, false, true);
		return;
	}

	auto combat = obj->getCombatClass();

	this->_panel->setLine(KEP::GUIColor::getMain() + "combat mode:", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(combat->combatModeActive), cat_combat, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "Num Slots:", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(combat->attackSlots.getNumAttackSlots()), cat_combat, false, true);

	if (combat->combatModeActive)
		this->_panel->setLine(KEP::GUIColor::getMain() + "State:", KEP::GUIColor::getMain() + getSwordStateEnumName(combat->combatState), cat_combat, false, true);
	else
		this->_panel->setLine(KEP::GUIColor::getMain() + "State:", KEP::GUIColor::getMain() + "peace", cat_combat, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "Dead Time:", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(combat->inDeadTime), cat_combat, false, true);

	auto targetCharacter = combat->currentTargetHandle.getCharacter();
	std::string targetName = targetCharacter != nullptr ? targetCharacter->displayName : "";
	this->_panel->setLine(KEP::GUIColor::getMain() + "Target:", KEP::GUIColor::getMain() + targetName, cat_combat, false, true);

	for (size_t i = 0; i < 5; i++)
	{
		auto slot = combat->attackSlots.attackSlotH[i].who.getCharacter();
		std::string slotName = slot != nullptr ? slot->displayName : "";
		this->_panel->setLine(KEP::GUIColor::getMain() + "slot" + Ogre::StringConverter::toString(i+1) + ":", KEP::GUIColor::getMain() + slotName, cat_combat, false, true);
	}

	size_t index = 1;
	for (uint32_t i = 0; i < combat->attackersH.size(); i++)
	{
		if (4 < i)
		{
			this->_panel->setLine(KEP::GUIColor::getMain() + "...", "", cat_combat, false, true);
			break;
		}
		auto attaker = combat->attackersH[i].getCharacter();
		std::string attakerName = attaker != nullptr ? attaker->displayName : "";
		this->_panel->setLine(KEP::GUIColor::getMain() + "attaker" + Ogre::StringConverter::toString(i+1) + ":", KEP::GUIColor::getMain() + attakerName, cat_combat, false, true);
	}

	this->_panel->addSpace(cat_combat, 0.5f);
}

void KEP::tools::InformationPanel::_displayUniqueNpcInformation()
{
	this->_panel->clearPage(cat_vips);
	if (this->_panel->getCurrentCategory() != cat_vips)
		return;

	auto uniqueNPCMgr = KEP::functions->UniqueNPCManager_getSingleton();
	uniqueNPCMgr->uniqueCharacterStates.begin();
	for (auto iter = uniqueNPCMgr->uniqueCharacterStates.begin(); iter != uniqueNPCMgr->uniqueCharacterStates.end(); ++iter)
	{
		std::string spawnMsg = iter->second.characterTemplate == nullptr ? " (not spawned)" : "";
		std::string involved = iter->second.playerInvolvement ? " -player involved" : "";
		this->_panel->setLine(KEP::GUIColor::getMain() + iter->first->name, KEP::GUIColor::getMain() + getWorldStateEnumName(iter->second.state) + spawnMsg + involved, cat_vips, false, true);
	}

	this->_panel->addSpace(cat_vips, 0.5f);
}
