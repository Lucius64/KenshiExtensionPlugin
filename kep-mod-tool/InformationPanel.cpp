/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include "pch.h"
#include <InformationPanel.h>
#include <SquadInfo.h>

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
#include <kenshi/combat/CombatClass.h>
#include <kenshi/combat/CombatTechniqueData.h>
#include <kenshi/Animation/AnimationClass.h>
#include <kenshi/AI/AI.h>
#include <kenshi/StateBroadcastData.h>
#include <kenshi/Town.h>
#include <kenshi/CharStats.h>
#include <kenshi/gui/DialogueWindow.h>
#include <kenshi/Dialogue.h>

#include <extern/InventoryManager.h>
#include <extern/UniqueNPCManager.h>
#include <extern/TradingInfo.h>

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
	const int cat_dialogue = 11;

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
			return "Startup";
		case DECISION:
			return "Decision";
		case CIRCLE_MENACINGLY:
			return "circle";
		case WAIT_MENACINGLY:
			return "Wait";
		case HESITATE:
			return "Hesitate";
		case STUMBLE:
			return "stumble";
		case COMBAT_FINISHED:
			return "finished";
		case TARGET_PATHFINDING_STARTUP:
			return "pathfinding_startup to target";
		case TARGET_PATHFINDING:
			return "pathfinding to target";
		default:
			return "none";
		}
	}

	std::string getWeaponCategoryName(WeaponCategory what)
	{
		switch (what)
		{
		case SKILL_KATANAS:
			return "Katana";
		case SKILL_SABRES:
			return "Sabre";
		case SKILL_BLUNT:
			return "Blunt weapon";
		case SKILL_HEAVY:
			return "Heavy weapons";
		case SKILL_HACKERS:
			return "Hacker";
		case SKILL_UNARMED:
			return "Martial arts";
		case SKILL_BOW:
			return "Crossbow";
		case SKILL_TURRET:
			return "Turret";
		case ATTACK_POLEARMS:
			return "Polearm";
		case ATTACK_ELEPHANT:
			return "Elephant creatures";
		case ATTACK_DOG:
			return "Dog creatures";
		case ATTACK_BULL:
			return "Bull creatures";
		case ATTACK_ROBOTSPIDER:
			return "Robot spider creatures";
		case ATTACK_SPIDER:
			return "Spider creatures";
		case ATTACK_CAGEBEAST:
			return "Cagebeast creatures";
		case ATTACK_DUCK:
			return "Duck creatures";
		case ATTACK_GORILLA:
			return "Gorilla creatures";
		case ATTACK_GAR:
			return "Gar creatures";
		case ATTACK_FROG:
			return "Frog creatures";
		case ATTACK_GOAT:
			return "Goat creatures";
		case ATTACK_GIRAFFE:
			return "Giraffe creatures";
		case ATTACK_NULL:
			return "Type21 creatures";
		case NUM_SKILL_TYPES:
			return "Type22 creatures";
		default:
			return "none";
		}
	}

	std::string getConversationTypeName(EventTriggerEnum what)
	{
		switch (what)
		{
		case EV_PLAYER_TALK_TO_ME:
			return "EV_PLAYER_TALK_TO_ME";
		case EV_ANNOUNCEMENT:
			return "EV_ANNOUNCEMENT";
		case EV_I_SEE_NEUTRAL_SQUAD:
			return "EV_I_SEE_NEUTRAL_SQUAD";
		case EV_I_SEE_RAGDOLL:
			return "EV_I_SEE_RAGDOLL";
		case EV_______:
			return "EV_______";
		case EV_SOUND_THE_ALARM:
			return "EV_SOUND_THE_ALARM";
		case EV_I_________:
			return "EV_I_________";
		case EV_THIEF_CAUGHT_STEALING_FROM_ME:
			return "EV_THIEF_CAUGHT_STEALING_FROM_ME";
		case EV_SHOO_FROM_MY_BUILDING:
			return "EV_SHOO_FROM_MY_BUILDING";
		case EV_MARKED_FOR_DEATH:
			return "EV_MARKED_FOR_DEATH";
		case EV_SCREAMING_TORTURE:
			return "EV_SCREAMING_TORTURE";
		case EV_BAR_TALK:
			return "EV_BAR_TALK";
		case EV_UNLOCK_MY_CAGE_OR_SHACKLES:
			return "EV_UNLOCK_MY_CAGE_OR_SHACKLES";
		case EV_UNLOCK_MY_CAGE_ATTEMPT:
			return "EV_UNLOCK_MY_CAGE_ATTEMPT";
		case EV_I_DEFEATED_SQUAD:
			return "EV_I_DEFEATED_SQUAD";
		case EV_LAUNCH_ATTACK:
			return "EV_LAUNCH_ATTACK";
		case EV_INTRUDER_FOUND:
			return "EV_INTRUDER_FOUND";
		case EV_HEALING_OTHER_START:
			return "EV_HEALING_OTHER_START";
		case EV_BEING_HEALED_START:
			return "EV_BEING_HEALED_START";
		case EV_HEALING_OTHER_FINISHED:
			return "EV_HEALING_OTHER_FINISHED";
		case EV_BEING_HEALED_FINISHED:
			return "EV_BEING_HEALED_FINISHED";
		case EV_FIRSTAID_KIT_EMPTY:
			return "EV_FIRSTAID_KIT_EMPTY";
		case EV_GET_UP_PEACE:
			return "EV_GET_UP_PEACE";
		case EV_GET_UP_FIGHT:
			return "EV_GET_UP_FIGHT";
		case EV_GET_UP_UNNECCESSARY_FIGHT:
			return "EV_GET_UP_UNNECCESSARY_FIGHT";
		case EV_HARRASSMENT_SHOUTS:
			return "EV_HARRASSMENT_SHOUTS";
		case EV_I_SEE_ANIMAL_SQUAD:
			return "EV_I_SEE_ANIMAL_SQUAD";
		case EV_SPEECH_INTERRUPTED_ATTACKED_BY_TARGET:
			return "EV_SPEECH_INTERRUPTED_ATTACKED_BY_TARGET";
		case EV_SPEECH_INTERRUPTED_ATTACKED_BY_STRANGERS:
			return "EV_SPEECH_INTERRUPTED_ATTACKED_BY_STRANGERS";
		case EV_CONTRACT_JOB_ENDED:
			return "EV_CONTRACT_JOB_ENDED";
		case EV_BETRAYAL:
			return "EV_BETRAYAL";
		case EV_LOOTING_WEAPON_ONLY:
			return "EV_LOOTING_WEAPON_ONLY";
		case EV_LOOTING_EVERYTHING:
			return "EV_LOOTING_EVERYTHING";
		case EV_I_SEE_UNIFORM_IMPOSTER:
			return "EV_I_SEE_UNIFORM_IMPOSTER";
		case EV_INTRODUCING_NEW_SLAVE:
			return "EV_INTRODUCING_NEW_SLAVE";
		case EV_ESCAPING_SLAVE_SPOTTED:
			return "EV_ESCAPING_SLAVE_SPOTTED";
		case EV_RECAPTURED_A_SLAVE:
			return "EV_RECAPTURED_A_SLAVE";
		case EV_SHOUT_AT_SLAVE_WORKER:
			return "EV_SHOUT_AT_SLAVE_WORKER";
		case EV_SLAVE_DELIVERY:
			return "EV_SLAVE_DELIVERY";
		case EV_ESCAPED_EX_SLAVE_SPOTTED:
			return "EV_ESCAPED_EX_SLAVE_SPOTTED";
		case EV_WITNESS_GENERIC_ASSAULT:
			return "EV_WITNESS_GENERIC_ASSAULT";
		case EV_WITNESS_LOOTING_ALLY:
			return "EV_WITNESS_LOOTING_ALLY";
		case EV_WITNESS_THIEF_OR_LOCKPICK:
			return "EV_WITNESS_THIEF_OR_LOCKPICK";
		case EV_BOUNTY_SPOTTED:
			return "EV_BOUNTY_SPOTTED";
		case EV_ESCAPED_PRISONER_SPOTTED:
			return "EV_ESCAPED_PRISONER_SPOTTED";
		case EV_PRISONER_FREE_TO_GO:
			return "EV_PRISONER_FREE_TO_GO";
		case EV_ALMOST_WOKE_UP:
			return "EV_ALMOST_WOKE_UP";
		case EV_ENTER_BIOME:
			return "EV_ENTER_BIOME";
		case EV_ENTER_TOWN:
			return "EV_ENTER_TOWN";
		case EV_SQUAD_BROKEN:
			return "EV_SQUAD_BROKEN";
		case EV_BOUGHT_ME_FROM_SLAVERY:
			return "EV_BOUGHT_ME_FROM_SLAVERY";
		case EV_EATING_SOMETHING_SOUNDS:
			return "EV_EATING_SOMETHING_SOUNDS";
		case EV_WORSHIPING_SOMETHING:
			return "EV_WORSHIPING_SOMETHING";
		case EV_SLAVE_ESCAPE_OPPORTUNITY_SAVIOR:
			return "EV_SLAVE_ESCAPE_OPPORTUNITY_SAVIOR";
		case EV_SLAVE_ESCAPE_OPPORTUNITY_ALONE:
			return "EV_SLAVE_ESCAPE_OPPORTUNITY_ALONE";
		case EV_ASSASSINATION_FAILED:
			return "EV_ASSASSINATION_FAILED";
		case EV_EATING_MY_CROPS:
			return "EV_EATING_MY_CROPS";
		case EV_KIDNAPPING_MY_ALLY:
			return "EV_KIDNAPPING_MY_ALLY";
		case EV_USING_MY_TRAINING_EQUIPMENT:
			return "EV_USING_MY_TRAINING_EQUIPMENT";
		case EV_GIVE_UP_CHASE:
			return "EV_GIVE_UP_CHASE";
		case EV_ACID_FEET:
			return "EV_ACID_FEET";
		case EV_ACID_RAIN:
			return "EV_ACID_RAIN";
		case EV_ACID_WATER:
			return "EV_ACID_WATER";
		case EV_WINDY:
			return "EV_WINDY";
		case EV_POISON_GAS:
			return "EV_POISON_GAS";
		case EV_I_SEE_ENEMY_PLAYER:
			return "EV_I_SEE_ENEMY_PLAYER";
		case EV_I_SEE_ALLY_PLAYER:
			return "EV_I_SEE_ALLY_PLAYER";
		case EV_I_SEE_ILLEGAL_PLAYER_BUILDING:
			return "EV_I_SEE_ILLEGAL_PLAYER_BUILDING";
		case EV_BURNING:
			return "EV_BURNING";
		case EV_LOST_LEG:
			return "EV_LOST_LEG";
		case EV_LOST_ARM:
			return "EV_LOST_ARM";
		case EV_I_SEE_PLAYER_NICE_BUILDING:
			return "EV_I_SEE_PLAYER_NICE_BUILDING";
		case EV_TAKEN_OVER_PLAYER_TOWN:
			return "EV_TAKEN_OVER_PLAYER_TOWN";
		case EV_CROWD_TRIGGERED:
			return "EV_CROWD_TRIGGERED";
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
	this->_panel->addTab(cat_dialogue, KEP::TranslationUtility::gettext_main("Dialogue"), "");
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
				this->_panel->clearPage(cat_combat);
				this->_panel->clearPage(cat_vips);
				this->_panel->clearPage(cat_dialogue);

				_lastSelected = gui->selectedObject;
			}
			_displayBuildingInformation();
			_displayCharacterInformation();
			_displayCombatInformation();
			_displayUniqueNpcInformation();
			_displayDialogueInformation();
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

	auto parentBuilding = building->isIndoors().getBuilding();
	if (parentBuilding != nullptr)
		this->_panel->setLine(KEP::GUIColor::getMain() + "Parent:", KEP::GUIColor::getMain() + parentBuilding->displayName, cat_building, false, true);
	else
		this->_panel->setLine(KEP::GUIColor::getMain() + "Parent:", KEP::GUIColor::getMain() + "nothing", cat_building, false, true);

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

	auto activePlatoon = obj->getPlatoon();
	if (activePlatoon != nullptr)
	{
		displayBlackboardInformation(activePlatoon->me->blackboard, this->_panel, cat_character);
		KEP::functions->CharacterMemory_getGUIData(activePlatoon->_myMemory, this->_panel, cat_character);
	}

	this->_panel->setLine(KEP::GUIColor::getMain() + "-----------CHARACTER-------------", "", cat_character, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "Handle:", KEP::GUIColor::getMain() + obj->handle.toString(), cat_character, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "Name:", KEP::GUIColor::getMain() + obj->displayName, cat_character, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "Template:", KEP::GUIColor::getMain() + obj->getGameData()->stringID, cat_character, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "Faction:", KEP::GUIColor::getMain() + obj->getFaction()->name, cat_character, false, true);

	auto race = obj->getRace();
	this->_panel->setLine(KEP::GUIColor::getMain() + "Race:", KEP::GUIColor::getMain() + race->data->name, cat_character, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "NPC type:", KEP::GUIColor::getMain() + getCharacterTypeEnumName(obj->stateBroadcast->NPCType), cat_character, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "Personality:", KEP::GUIColor::getMain() + getPersonalityTagsName(obj->stateBroadcast->myPersonality), cat_character, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "Portrait serial:", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(obj->portraitSerial), cat_character, false, true);

	if (obj->ai->tradingInfo != nullptr)
	{
		this->_panel->setLine(KEP::GUIColor::getMain() + "TradingInfo:", "", cat_character, false, true);

		if (obj->ai->tradingInfo->town != nullptr)
			this->_panel->setLine(KEP::GUIColor::getMain() + "Town:", obj->ai->tradingInfo->town->displayName, cat_character, false, true);

		auto& tradeItems = obj->ai->tradingInfo->tradeItems;
		for (size_t i = 0; i < tradeItems.size(); ++i)
		{
			this->_panel->setLine(KEP::GUIColor::getMain() + "item function " + Ogre::StringConverter::toString(i) + ":", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(tradeItems[i].itemFunction), cat_character, false, true);
			this->_panel->setLine(KEP::GUIColor::getMain() + "quantity " + Ogre::StringConverter::toString(i) + ":", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(tradeItems[i].quantity), cat_character, false, true);
			this->_panel->setLine(KEP::GUIColor::getMain() + "count " + Ogre::StringConverter::toString(i) + ":", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(tradeItems[i].count), cat_character, false, true);
		}
	}

	obj->getSensoryData()->getGUIData(this->_panel, cat_character);

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

	this->_panel->setLine(KEP::GUIColor::getMain() + "weapon type:", KEP::GUIColor::getMain() + getWeaponCategoryName(combat->stats->currentWeaponType), cat_combat, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "Num Slots:", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(combat->attackSlots.getNumAttackSlots()), cat_combat, false, true);
	
	if (combat->combatModeActive)
		this->_panel->setLine(KEP::GUIColor::getMain() + "State:", KEP::GUIColor::getMain() + getSwordStateEnumName(combat->combatState), cat_combat, false, true);
	else
		this->_panel->setLine(KEP::GUIColor::getMain() + "State:", KEP::GUIColor::getMain() + "peace", cat_combat, false, true);

	if (combat->combatModeActive)
		this->_panel->setLine(KEP::GUIColor::getMain() + "Next:", KEP::GUIColor::getMain() + getSwordStateEnumName(combat->nextMove), cat_combat, false, true);
	else
		this->_panel->setLine(KEP::GUIColor::getMain() + "Next:", KEP::GUIColor::getMain() + "peace", cat_combat, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "Dead Time:", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(combat->inDeadTime), cat_combat, false, true);

	auto targetCharacter = combat->currentTargetHandle.getCharacter();
	std::string targetName = targetCharacter != nullptr ? targetCharacter->displayName : "";
	this->_panel->setLine(KEP::GUIColor::getMain() + "Target:", KEP::GUIColor::getMain() + targetName, cat_combat, false, true);

	auto blockingTarget = combat->blockingTargetH.getCharacter();
	std::string blockingTargetName = blockingTarget != nullptr ? blockingTarget->displayName : "";
	this->_panel->setLine(KEP::GUIColor::getMain() + "Blocking Target:", KEP::GUIColor::getMain() + blockingTargetName, cat_combat, false, true);

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

	if (combat->currentTechnique != nullptr)
	{
		this->_panel->setLine(KEP::GUIColor::getMain() + "current technique:", KEP::GUIColor::getMain() + combat->currentTechnique->animation, cat_combat, false, true);
		float progress = combat->animation->getAnimationProgress(combat->currentTechnique);
		this->_panel->setLineProgress("progress:", cat_combat, progress, "", false);
	}
	else
	{
		this->_panel->setLine(KEP::GUIColor::getMain() + "current technique:", KEP::GUIColor::getMain() + "none", cat_combat, false, true);
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

		std::string textColor;
		auto character = iter->second.handle.getCharacter();
		if (iter->second.state == DEAD)
			textColor = KEP::GUIColor::getBad();
		else if (character != nullptr)
		{
			if (iter->second.state == IMPRISONED)
				textColor = KEP::GUIColor::getGood();
			else
				textColor = KEP::GUIColor::getGoodBright();
		}
		else
		{
			if (iter->second.state == IMPRISONED)
				textColor = KEP::GUIColor::getGreyedBright();
			else
				textColor = KEP::GUIColor::getMain();
		}

		this->_panel->setLine(textColor + iter->first->name, textColor + getWorldStateEnumName(iter->second.state) + spawnMsg + involved, cat_vips, false, true);
	}

	this->_panel->addSpace(cat_vips, 0.5f);
}

void KEP::tools::InformationPanel::_displayDialogueInformation()
{
	this->_panel->clearPage(cat_dialogue);
	if (this->_panel->getCurrentCategory() != cat_dialogue)
		return;

	Dialogue* dialogue = nullptr;

	if (gui->inDialogue())
		dialogue = gui->dialogue->dialogue;
	else
	{
		auto obj = this->_lastSelected.getCharacter();
		if (obj != nullptr)
			dialogue = obj->dialogue;
	}

	if (dialogue == nullptr)
	{
		this->_panel->setLine(KEP::GUIColor::getMain() + "The selected object is invalid.", "", cat_dialogue, false, true);
		return;
	}

	this->_panel->setLine(KEP::GUIColor::getMain() + "Target: " + (dialogue->me != nullptr ? dialogue->me->getName() : "none"), "", cat_dialogue, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "dialog:", KEP::GUIColor::getMain() + getConversationTypeName(dialogue->currentConversationType), cat_dialogue, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "_needsDynamicAssessments:", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(dialogue->_needsDynamicAssessments), cat_dialogue, false, true);
	this->_panel->setLine(KEP::GUIColor::getMain() + "_hasEnded:", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(dialogue->_hasEnded), cat_dialogue, false, true);

	this->_panel->setLine(KEP::GUIColor::getMain() + "current Conversation", KEP::GUIColor::getMain() + (dialogue->currentConversation != nullptr ? dialogue->currentConversation->getName() : "-"), cat_dialogue, false, true);

	auto me = dialogue->conversationMaster.getCharacter();
	this->_panel->setLine(KEP::GUIColor::getMain() + "Conversation master", KEP::GUIColor::getMain() + (me != nullptr ? me->getName() : "none"), cat_dialogue, false, true);

	auto target = dialogue->conversationTarget.getCharacter();
	this->_panel->setLine(KEP::GUIColor::getMain() + "Conversation target (" + dialogue->conversationTarget.toString() + ")", KEP::GUIColor::getMain() + (target != nullptr ? target->getName() : "none"), cat_dialogue, false, true);

	auto interjector1 = dialogue->interjector1.getCharacter();
	this->_panel->setLine(KEP::GUIColor::getMain() + "Conversation interjector1", KEP::GUIColor::getMain() + (interjector1 != nullptr ? interjector1->getName() : "none"), cat_dialogue, false, true);

	auto interjector2 = dialogue->interjector2.getCharacter();
	this->_panel->setLine(KEP::GUIColor::getMain() + "Conversation interjector2", KEP::GUIColor::getMain() + (interjector2 != nullptr ? interjector2->getName() : "none"), cat_dialogue, false, true);

	auto interjector3 = dialogue->interjector3.getCharacter();
	this->_panel->setLine(KEP::GUIColor::getMain() + "Conversation interjector3", KEP::GUIColor::getMain() + (interjector3 != nullptr ? interjector3->getName() : "none"), cat_dialogue, false, true);

	auto waitingForReplyFrom = dialogue->waitingForReplyFrom.getCharacter();
	this->_panel->setLine(KEP::GUIColor::getMain() + "Conversation waitingForReplyFrom", KEP::GUIColor::getMain() + (waitingForReplyFrom != nullptr ? waitingForReplyFrom->getName() : "none"), cat_dialogue, false, true);

}
