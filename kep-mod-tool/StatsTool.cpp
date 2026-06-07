#include "pch.h"
#include <algorithm>
#include <StatsTool.h>

#include <mygui/MyGUI.h>

#include <Debug.h>
#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/Platoon.h>
#include <kenshi/Character.h>
#include <kenshi/CharStats.h>
#include <kenshi/RaceData.h>
#include <kenshi/gui/ForgottenGUI.h>
#include <kenshi/gui/DatapanelGUI.h>
#include <kenshi/gui/DataPanelLine.h>
#include <extern/StateBroadcastData.h>

#include <kep/translation.h>
#include <kep/functions.h>
#include <kep/utility.h>
#include <utility.h>

namespace
{
	std::string lineStats;
	std::string lineStatsData;
	std::string lineSlave;
	std::string linePersonality;

	void initLineKey()
	{
		lineStats = KEP::TranslationUtility::gettext("Stats");
		lineStatsData = KEP::TranslationUtility::gettext("Stats data");
		lineSlave = KEP::TranslationUtility::gettext("Slave state");
		linePersonality = KEP::TranslationUtility::gettext("Personality");
	}

	std::string getStatName(StatsEnumerated what)
	{
		switch (what)
		{
		case STAT_STRENGTH:
			return KEP::TranslationUtility::gettext_main("Strength");
		case STAT_MELEE_ATTACK:
			return KEP::TranslationUtility::gettext_main("Melee attack");
		case STAT_LABOURING:
			return KEP::TranslationUtility::gettext_main("Labouring");
		case STAT_SCIENCE:
			return KEP::TranslationUtility::gettext_main("Science");
		case STAT_ENGINEERING:
			return KEP::TranslationUtility::gettext_main("Engineer");
		case STAT_ROBOTICS:
			return KEP::TranslationUtility::gettext_main("Robotics");
		case STAT_SMITHING_WEAPON:
			return KEP::TranslationUtility::gettext_main("Weapon smith");
		case STAT_SMITHING_ARMOUR:
			return KEP::TranslationUtility::gettext_main("Armour smith");
		case STAT_MEDIC:
			return KEP::TranslationUtility::gettext_main("Field medic");
		case STAT_THIEVING:
			return KEP::TranslationUtility::gettext_main("Thievery");
		case STAT_TURRETS:
			return KEP::TranslationUtility::gettext_main("Turrets");
		case STAT_FARMING:
			return KEP::TranslationUtility::gettext_main("Farming");
		case STAT_COOKING:
			return KEP::TranslationUtility::gettext_main("Cooking");
		case STAT_STEALTH:
			return KEP::TranslationUtility::gettext_main("Stealth");
		case STAT_ATHLETICS:
			return KEP::TranslationUtility::gettext_main("Athletics");
		case STAT_DEXTERITY:
			return KEP::TranslationUtility::gettext_main("Dexterity");
		case STAT_MELEE_DEFENCE:
			return KEP::TranslationUtility::gettext_main("Melee defence");
		case STAT_TOUGHNESS:
			return KEP::TranslationUtility::gettext_main("Toughness");
		case STAT_ASSASSINATION:
			return KEP::TranslationUtility::gettext_main("Assassination");
		case STAT_SWIMMING:
			return KEP::TranslationUtility::gettext_main("Swimming");
		case STAT_PERCEPTION:
			return KEP::TranslationUtility::gettext_main("Perception");
		case STAT_KATANAS:
			return KEP::TranslationUtility::gettext_main("Katanas");
		case STAT_SABRES:
			return KEP::TranslationUtility::gettext_main("Sabres");
		case STAT_HACKERS:
			return KEP::TranslationUtility::gettext_main("Hackers");
		case STAT_HEAVYWEAPONS:
			return KEP::TranslationUtility::gettext_main("Heavy weapons");
		case STAT_BLUNT:
			return KEP::TranslationUtility::gettext_main("Blunt");
		case STAT_MARTIALARTS:
			return KEP::TranslationUtility::gettext_main("Martial arts");
		case STAT_DODGE:
			return KEP::TranslationUtility::gettext_main("Dodge");
		case STAT_POLEARMS:
			return KEP::TranslationUtility::gettext_main("Polearms");
		case STAT_CROSSBOWS:
			return KEP::TranslationUtility::gettext_main("Crossbows");
		case STAT_FRIENDLY_FIRE:
			return KEP::TranslationUtility::gettext_main("Precision Shooting");
		case STAT_LOCKPICKING:
			return KEP::TranslationUtility::gettext_main("Lockpicking");
		case STAT_SMITHING_BOW:
			return KEP::TranslationUtility::gettext_main("Crossbow smith");
		default:
			return "";
		}
	}
}

KEP::tools::StatsTool::StatsTool()
	: _panel(nullptr)
	, _category(0x3)
	, _selectedStatsEnum(-1)
	, _selectedStatCategory(0)
	, _amount(0.0f)
	, _xp(0.0f)
	, _selectedStatsData(0)
	, _selectedSlaveState(0)
{
	initLineKey();

	_initStatsEnumList();
	_initStatsDataList();
}

void KEP::tools::StatsTool::refresh()
{
	if (this->_panel == nullptr)
		return;

	this->_panel->clearPage(this->_category);

	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext("[Stats]"), "", this->_category, false, true);

	this->_panel->setLineDropBox(lineStats, this->_category, &this->_selectedStatsEnum, false, 0.7f);
	_updateStatsEnumList();

	this->_panel->addSpace(this->_category, 0.25f);
	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("XP"), this->_category, true, 0.0f, 10000.0f, &this->_xp);
	this->_panel->addSpace(this->_category, 0.25f);

	auto button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Give XP"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<StatsTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &StatsTool::_increaseStats);

	this->_panel->addSpace(this->_category, 0.5f);

	this->_panel->setLineDropBox(lineStatsData, this->_category, &this->_selectedStatsData, false, 0.7f);
	_updateStatsDataList();

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Overwrite stats"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<StatsTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &StatsTool::_overwriteStats);

	this->_panel->addSpace(this->_category, 1.0f);

	this->_panel->setLineDropBox(lineSlave, this->_category, &this->_selectedSlaveState, false, 0.7f);
	_updateSlaveSateList();

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Set slave state"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<StatsTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &StatsTool::_setSlaveState);

	this->_panel->addSpace(this->_category, 1.0f);

	this->_panel->setLineDropBox(linePersonality, this->_category, &this->_selectedPersonality, false, 0.7f);
	_updatePersonalityList();

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Change personality"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<StatsTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &StatsTool::_changePersonality);

	this->_panel->addSpace(this->_category, 1.0f);
}

void KEP::tools::StatsTool::_increaseStats(DataPanelLine* line)
{
	if (this->_selectedStatsEnum < 0)
		return;

	auto character = gui->selectedObject.getCharacter();
	if (character != nullptr)
	{
		float xp = this->_xp;
		while (20.0f < xp)
		{
			increaseStat(character->stats->getStatRef(static_cast<StatsEnumerated>(this->_selectedStatsEnum)), 20.0f, 101.0f);
			xp -= 20.0f;
		}
		increaseStat(character->stats->getStatRef(static_cast<StatsEnumerated>(this->_selectedStatsEnum)), xp, 101.0f);
	}
}

void KEP::tools::StatsTool::_overwriteStats(DataPanelLine* line)
{
	if (this->_selectedStatsData < 0)
		return;

	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character."), true);
		return;
	}

	character->stats->updateStats(this->_statsDataList[this->_selectedStatsData]);
	auto race = character->getRace();
	
	for (auto iter = race->statMods.begin(); iter != race->statMods.end(); ++iter)
	{
		auto& statsValue = character->stats->getStatRef(iter->first);
		statsValue *= iter->second;
	}
	
	auto newMaxBlood = character->medical.getMaxBlood();
	if (newMaxBlood < character->medical.blood)
		character->medical.blood = newMaxBlood;
	
	auto item = character->stats->weapon.getItem();
	if (item != nullptr)
		character->stats->setWeapon(item->isWeapon());

	character->stats->_recalculateStats();
}

void KEP::tools::StatsTool::_setSlaveState(DataPanelLine* line)
{
	if (this->_selectedSlaveState < 0)
		return;

	auto character = gui->selectedObject.getCharacter();
	if (character != nullptr)
	{
		auto slave = static_cast<SlaveStateEnum>(this->_selectedSlaveState);
		if (slave == NOT_SLAVE)
		{
			character->setChainedMode(false, KEP::functions->getNULL_HAND());
			KEP::functions->StateBroadcastData_setSlaveState(character->getStateBroadcast(), slave);
		}
		else if (slave == IS_SLAVE)
		{
			character->setChainedMode(true, KEP::functions->getNULL_HAND());
		}
		else
		{
			KEP::functions->StateBroadcastData_setSlaveState(character->getStateBroadcast(), slave);
		}
	}
}

void KEP::tools::StatsTool::_changePersonality(DataPanelLine* line)
{
	if (this->_selectedPersonality < 0)
		return;

	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character."), true);
		return;
	}

	auto stateBroadcast = character->getStateBroadcast();
	if (stateBroadcast != nullptr)
		stateBroadcast->personality = static_cast<PersonalityTags>(this->_selectedPersonality);

}

void KEP::tools::StatsTool::_initStatsEnumList()
{
	this->_statsEnumList.reserve(33);
	this->_statsEnumList.push_back(STAT_STRENGTH);
	this->_statsEnumList.push_back(STAT_TOUGHNESS);
	this->_statsEnumList.push_back(STAT_DEXTERITY);
	this->_statsEnumList.push_back(STAT_PERCEPTION);
	this->_statsEnumList.push_back(STAT_KATANAS);
	this->_statsEnumList.push_back(STAT_SABRES);
	this->_statsEnumList.push_back(STAT_HACKERS);
	this->_statsEnumList.push_back(STAT_HEAVYWEAPONS);
	this->_statsEnumList.push_back(STAT_BLUNT);
	this->_statsEnumList.push_back(STAT_POLEARMS);
	this->_statsEnumList.push_back(STAT_MELEE_ATTACK);
	this->_statsEnumList.push_back(STAT_MELEE_DEFENCE);
	this->_statsEnumList.push_back(STAT_DODGE);
	this->_statsEnumList.push_back(STAT_MARTIALARTS);
	this->_statsEnumList.push_back(STAT_TURRETS);
	this->_statsEnumList.push_back(STAT_CROSSBOWS);
	this->_statsEnumList.push_back(STAT_FRIENDLY_FIRE);
	this->_statsEnumList.push_back(STAT_STEALTH);
	this->_statsEnumList.push_back(STAT_LOCKPICKING);
	this->_statsEnumList.push_back(STAT_THIEVING);
	this->_statsEnumList.push_back(STAT_ASSASSINATION);
	this->_statsEnumList.push_back(STAT_ATHLETICS);
	this->_statsEnumList.push_back(STAT_SWIMMING);
	this->_statsEnumList.push_back(STAT_MEDIC);
	this->_statsEnumList.push_back(STAT_ENGINEERING);
	this->_statsEnumList.push_back(STAT_ROBOTICS);
	this->_statsEnumList.push_back(STAT_SCIENCE);
	this->_statsEnumList.push_back(STAT_SMITHING_WEAPON);
	this->_statsEnumList.push_back(STAT_SMITHING_ARMOUR);
	this->_statsEnumList.push_back(STAT_SMITHING_BOW);
	this->_statsEnumList.push_back(STAT_LABOURING);
	this->_statsEnumList.push_back(STAT_FARMING);
	this->_statsEnumList.push_back(STAT_COOKING);
}

void KEP::tools::StatsTool::_initStatsDataList()
{
	lektor<GameData*> datas;
	ou->gamedata.getDataOfType(datas, STATS);
	std::sort(datas.begin(), datas.end(), LessGameData());

	this->_statsDataList.resize(datas.size());
	for (uint32_t i = 0; i < datas.size(); ++i)
	{
		this->_statsDataList[i] = datas[i];
	}
}

void KEP::tools::StatsTool::_updateStatsEnumList()
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineStats, this->_category));
	dropBox->clearValues();
	dropBox->addAValue("-", -2);
	for (uint32_t i = 0; i < this->_statsEnumList.size(); ++i)
	{
		dropBox->addAValue(getStatName(this->_statsEnumList[i]), this->_statsEnumList[i]);
	}
	dropBox->setSelectedValue(-2);
}

void KEP::tools::StatsTool::_updateStatsDataList()
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineStatsData, this->_category));
	if (dropBox == nullptr)
		return;

	dropBox->clearValues();
	dropBox->addAValue("-", -2);
	for (uint32_t i = 0; i < this->_statsDataList.size(); ++i)
	{
		dropBox->addAValue(this->_statsDataList[i]->name, i);
	}
	dropBox->setSelectedValue(-2);
}

void KEP::tools::StatsTool::_updateSlaveSateList()
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineSlave, this->_category));
	if (dropBox == nullptr)
		return;

	dropBox->clearValues();
	dropBox->addAValue(KEP::TranslationUtility::gettext_main("None"), NOT_SLAVE);
	dropBox->addAValue(KEP::TranslationUtility::gettext_main("Slave"), IS_SLAVE);
	dropBox->addAValue(KEP::TranslationUtility::gettext_main("Escaped slave"), ESCAPING_SLAVE);
	dropBox->addAValue(KEP::TranslationUtility::gettext_main("Ex-slave"), EX_SLAVE);
	dropBox->setSelectedValue(0);
}

void KEP::tools::StatsTool::_updatePersonalityList()
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(linePersonality, this->_category));
	if (dropBox == nullptr)
		return;

	dropBox->clearValues();
	dropBox->addAValue(KEP::TranslationUtility::gettext("honorable"), PT_HONORABLE);
	dropBox->addAValue(KEP::TranslationUtility::gettext("traitorous"), PT_TRAITOROUS);
	dropBox->addAValue(KEP::TranslationUtility::gettext("smart"), PT_SMART);
	dropBox->addAValue(KEP::TranslationUtility::gettext("dumb"), PT_DUMB);
	dropBox->addAValue(KEP::TranslationUtility::gettext("brave"), PT_BRAVE);
	dropBox->addAValue(KEP::TranslationUtility::gettext("fearful"), PT_FEARFUL);
	dropBox->addAValue(KEP::TranslationUtility::gettext("maniac"), PT_MANIACAL);
	dropBox->addAValue(KEP::TranslationUtility::gettext("warm kind"), PT_WARM_KIND);
	dropBox->addAValue(KEP::TranslationUtility::gettext("cold cruel"), PT_COLD_CRUEL);
	dropBox->addAValue(KEP::TranslationUtility::gettext("normal"), PT_NORMAL);
	dropBox->addAValue(KEP::TranslationUtility::gettext("unknown 3"), PT___);
	dropBox->addAValue(KEP::TranslationUtility::gettext("unknown 4"), PT__);
	dropBox->addAValue(KEP::TranslationUtility::gettext("unknown 7"), PT____);
	dropBox->addAValue(KEP::TranslationUtility::gettext("unknown 8"), PT_____);
	dropBox->setSelectedValue(1);
}
