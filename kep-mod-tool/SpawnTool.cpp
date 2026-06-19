#include "pch.h"
#include <algorithm>
#include <DevToolsPanel.h>
#include <SpawnTool.h>

#include <mygui/MyGUI.h>

#include <Debug.h>
#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/SharedKing.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/Faction.h>
#include <kenshi/FactionWarMgr.h>
#include <kenshi/FactionRelations.h>
#include <kenshi/Building/DoorStuff.h>
#include <kenshi/Building/UseableStuff.h>
#include <kenshi/Platoon.h>
#include <kenshi/Character.h>
#include <kenshi/Gear.h>
#include <kenshi/Town.h>
#include <kenshi/RootObjectFactory.h>
#include <kenshi/gui/ForgottenGUI.h>
#include <kenshi/gui/DatapanelGUI.h>
#include <kenshi/gui/DataPanelLine.h>
#include <extern/UniqueNPCManager.h>
#include <extern/AreaBiomeGroup.h>
#include <extern/TownBase.h>
#include <extern/BuildingInterior.h>

#include <kep/translation.h>
#include <kep/functions.h>
#include <kep/utility.h>
#include <utility.h>

namespace
{
	std::string lineLabelPlayerFaction;
	std::string lineBoxFaction;
	std::string lineBoxSquads;
	std::string lineBoxCharacter;
	std::string lineBoxItem;
	std::string lineBoxWeapon;
	std::string lineBoxModel;
	std::string lineBoxGearLevel;
	std::string lineLabelRandom;
	std::string lineTextItem;
	std::string lineTextWeapon;
	std::string lineBoxItemCategory;
	std::string lineBoxtemSubCategory;
	std::string lineBoxCampaign;
	std::string blueprintSuffix;
	std::string lineBoxFromFaction;
	std::string lineBoxToFaction;
	std::string lineBoxUniqueNpc;
	std::string lineBoxFactionUniform;
	std::string lineBoxItemColor;

	void initLineKey()
	{
		lineLabelPlayerFaction = KEP::TranslationUtility::gettext("Player Faction");
		lineBoxFaction = KEP::TranslationUtility::gettext_main("Faction");
		lineBoxSquads = KEP::TranslationUtility::gettext_main("Squads");
		lineBoxCharacter = KEP::TranslationUtility::gettext_main("Characters");
		lineBoxItem = KEP::TranslationUtility::gettext_main("Items");
		lineBoxWeapon = KEP::TranslationUtility::gettext("Weapons");
		lineBoxModel = KEP::TranslationUtility::gettext("Models");
		lineBoxGearLevel = KEP::TranslationUtility::gettext("Gear Level");
		lineLabelRandom = KEP::TranslationUtility::gettext("Random");
		lineTextItem = "Search for items";
		lineTextWeapon = "Search for weapons";
		lineBoxItemCategory = KEP::TranslationUtility::gettext("Item category");
		lineBoxtemSubCategory = KEP::TranslationUtility::gettext("Item subcategory");
		lineBoxCampaign = KEP::TranslationUtility::gettext("Campaigns");
		blueprintSuffix = " (" + KEP::TranslationUtility::gettext_main("Blueprints") + ")";
		lineBoxFromFaction = "from faction";
		lineBoxToFaction = "to faction";
		lineBoxUniqueNpc = "unique npc";
		lineBoxFactionUniform = KEP::TranslationUtility::gettext("Uniform");
		lineBoxItemColor = KEP::TranslationUtility::gettext_main("Colour");
	}
}

KEP::tools::SpawnTool::SpawnTool()
	: _panel(nullptr)
	, _category(0x2)
	, _gearLevel(4)
	, _selectedSpawnFaction(0)
	, _selectedSpawnSquad(0)
	, _selectedSpawnCharacter(0)
	, _selectedItem(0)
	, _selectedWeapon(-1)
	, _selectedModel(-1)
	, _searchItemCategory(NULL_ITEM)
	, _searchItemSubCategory(0)
	, _searchWeaponCategory(0)
	, _quantity(1.0f)
	, _selectedCampaign(0)
	, _animalAge(1.0f)
	, _squadSpawnMethodType(0)
	, _selectedFromFaction(0)
	, _selectedToFaction(0)
	, _relation(0.0f)
	, _selectedUniqueCharacter(0)
	, _uniqueState(3)
	, _selectedFactionUniform(-2)
	, _selectedItemColor(-2)
{
	initLineKey();

	_initFactionList();
	_initSquadList();
	_initCharacterList();
	_initItemList();
	_initWeaponList();
	_initModelList();
	_initCampaignList();
	_initUniqueCharacters();
	_initColorList();
}

void KEP::tools::SpawnTool::refresh()
{
	if (this->_panel == nullptr)
		return;

	this->_panel->clearPage(this->_category);

	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext("[Characters]"), "", this->_category, false, true);

	auto textbox = this->_panel->setLineTextEditable("Search for factions", "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->nameText->setCaption(KEP::TranslationUtility::gettext("Search"));
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_changeFactionSearchText);

	this->_panel->setLineDropBox(lineBoxFaction, this->_category, &this->_selectedSpawnFaction, false, 0.7f);
	_updateFactionList("");

	this->_panel->addSpace(this->_category, 0.5f);

	textbox = this->_panel->setLineTextEditable("Search for squads", "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->nameText->setCaption(KEP::TranslationUtility::gettext("Search"));
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_changeSquadSearchText);

	this->_panel->setLineDropBox(lineBoxSquads, this->_category, &this->_selectedSpawnSquad, false, 0.7f);
	_updateSquadList("");

	auto dropbox = this->_panel->setLineDropBox(KEP::TranslationUtility::gettext("Spawn method"), this->_category, &this->_squadSpawnMethodType, false, 0.7f);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Roaming"), 0);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Bar"), 1);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Resident"), 2);
	dropbox->setSelectedValue(0);

	auto button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Spawn"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_spawnSquad);

	this->_panel->addSpace(this->_category, 0.5f);

	textbox = this->_panel->setLineTextEditable("Search for characters", "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->nameText->setCaption(KEP::TranslationUtility::gettext("Search"));
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_changeCharacterSearchText);

	this->_panel->setLineDropBox(lineBoxCharacter, this->_category, &this->_selectedSpawnCharacter, false, 0.7f);
	_updateCharacterList("");

	this->_panel->addSpace(this->_category, 0.25f);
	auto slider = this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext("Age"), this->_category, true, 0.01f, 3.0f, &this->_animalAge);
	this->_panel->addSpace(this->_category, 0.25f);

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Add character"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_addCharacter);

	this->_panel->addSpace(this->_category, 1.0f);

	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext("[Items]"), "", this->_category, false, true);

	dropbox = this->_panel->setLineDropBox(lineBoxItemCategory, this->_category, &this->_searchItemCategory, false, 0.7f);
	dropbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_selectItemCategoryList);
	dropbox->addAValue(KEP::TranslationUtility::gettext("All"), NULL_ITEM);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Items"), ITEM);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Map"), MAP_ITEM);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Nest item"), NEST_ITEM);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Weapons"), WEAPON_MANUFACTURER);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Armor"), ARMOUR);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Backpack"), CONTAINER);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Crossbows"), CROSSBOW);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Robotics"), LIMB_REPLACEMENT);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Blueprints"), RESEARCH);
	dropbox->setSelectedValue(NULL_ITEM);

	dropbox = this->_panel->setLineDropBox(lineBoxtemSubCategory, this->_category, &this->_searchItemSubCategory, false, 0.7f);
	dropbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_selectItemSubcategoryList);
	this->_updateItemSubcategorylList();

	textbox = this->_panel->setLineTextEditable(lineTextItem, "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->nameText->setCaption(KEP::TranslationUtility::gettext("Search"));
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_changeItemSearchText);

	dropbox = this->_panel->setLineDropBox(lineBoxItem, this->_category, &this->_selectedItem, false, 0.7f);
	dropbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_selectItemList);

	dropbox = this->_panel->setLineDropBox(lineBoxGearLevel, this->_category, &this->_gearLevel, false, 0.7f);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Prototype"), 0);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Shoddy"), 1);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Standard"), 2);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("High"), 3);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Specialist"), 4);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Masterwork"), 5);
	dropbox->setSelectedValue(4);

	dropbox = this->_panel->setLineDropBox(KEP::TranslationUtility::gettext("Weapon category"), this->_category, &this->_searchWeaponCategory, false, 0.7f);
	dropbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_selectWeaponCategoryList);
	dropbox->addAValue(KEP::TranslationUtility::gettext("All"), 0);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Katanas"), 1);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Sabres"), 2);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Blunt"), 3);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Heavy weapons"), 4);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Hackers"), 5);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Polearms"), 9);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Martial arts"), 6);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Crossbows"), 7);
	dropbox->addAValue(KEP::TranslationUtility::gettext_main("Turrets"), 8);
	dropbox->setSelectedValue(0);

	textbox = this->_panel->setLineTextEditable(lineTextWeapon, "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->nameText->setCaption(KEP::TranslationUtility::gettext("Search"));
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_changeWeaponSearchText);

	this->_panel->setLineDropBox(lineBoxWeapon, this->_category, &this->_selectedWeapon, false, 0.7f);
	this->_panel->setLineDropBox(lineBoxModel, this->_category, &this->_selectedModel, false, 0.7f);

	_updateItemList("");
	_updateModelList();
	_updateWeaponList("");

	this->_panel->addSpace(this->_category, 0.25f);

	textbox = this->_panel->setLineTextEditable("Search for color", "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->nameText->setCaption(KEP::TranslationUtility::gettext("Search"));
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_changeColorSearchText);

	this->_panel->setLineDropBox(lineBoxItemColor, this->_category, &this->_selectedItemColor, false, 0.7f);
	_updateColorList("");

	this->_panel->addSpace(this->_category, 0.25f);

	textbox = this->_panel->setLineTextEditable("Search for unifroms", "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->nameText->setCaption(KEP::TranslationUtility::gettext("Search"));
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_changeUniformSearchText);

	this->_panel->setLineDropBox(lineBoxFactionUniform, this->_category, &this->_selectedFactionUniform, false, 0.7f);
	_updateUniformList("");

	this->_panel->addSpace(this->_category, 0.25f);
	slider = this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext("Quantity"), this->_category, true, 1.0f, 50.0f, &this->_quantity);
	slider->setPrecision(0);
	this->_panel->addSpace(this->_category, 0.25f);

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Give item"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_giveItem);

	this->_panel->addSpace(this->_category, 0.5f);

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Refresh Inventory"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_refreshInventory);

	this->_panel->addSpace(this->_category, 1.0f);

	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext("[Campaigns]"), "", this->_category, false, true);

	textbox = this->_panel->setLineTextEditable("Search for campaigns", "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->nameText->setCaption(KEP::TranslationUtility::gettext("Search"));
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_changeCampaignSearchText);

	this->_panel->setLineDropBox(lineBoxCampaign, this->_category, &this->_selectedCampaign, false, 0.7f);
	_updateCampaginList("");

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Trigger"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_triggerCampaign);

	this->_panel->addSpace(this->_category, 0.5f);

	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext("[Faction Relations]"), "", this->_category, false, true);

	textbox = this->_panel->setLineTextEditable("search for fromFactions", "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->nameText->setCaption(KEP::TranslationUtility::gettext("Search"));
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_changeFromFactionSearchText);

	dropbox = this->_panel->setLineDropBox(lineBoxFromFaction, this->_category, &this->_selectedFromFaction, false, 0.7f);
	dropbox->w1->setCaption("from");
	_updateFromFactionList("");

	this->_panel->addSpace(this->_category, 0.5f);

	textbox = this->_panel->setLineTextEditable("search for toFactions", "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->nameText->setCaption(KEP::TranslationUtility::gettext("Search"));
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_changeToFactionSearchText);

	dropbox = this->_panel->setLineDropBox(lineBoxToFaction, this->_category, &this->_selectedToFaction, false, 0.7f);
	dropbox->w1->setCaption("to");
	_updateToFactionList("");

	this->_panel->addSpace(this->_category, 0.25f);
	slider = this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext("Relation"), this->_category, true, -100.0f, 100.0f, &this->_relation);
	slider->setPrecision(0);
	this->_panel->addSpace(this->_category, 0.25f);

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Overwrite relation"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_overwriteRelation);

	this->_panel->addSpace(this->_category, 0.5f);

	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext("[Unique Character States]"), "", this->_category, false, true);

	textbox = this->_panel->setLineTextEditable("search for uniqueNpcs", "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->nameText->setCaption(KEP::TranslationUtility::gettext("Search"));
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_changeUniqueNpcSearchText);

	dropbox = this->_panel->setLineDropBox(lineBoxUniqueNpc, this->_category, &this->_selectedUniqueCharacter, false, 0.7f);
	dropbox->w1->setCaption(KEP::TranslationUtility::gettext("Unique characters"));
	_updateUniqueNpcList("");

	dropbox = this->_panel->setLineDropBox(KEP::TranslationUtility::gettext("State"), this->_category, &this->_uniqueState, false, 0.7f);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Not spawned"), 0);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Living"), 1);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Imprisoned"), 2);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Imprisoned (player involvement)"), 3);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Dead"), 4);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Dead (player involvement)"), 5);
	dropbox->setSelectedValue(0);

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Change unique character state"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<SpawnTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &SpawnTool::_changeUniqueNpcState);

	this->_panel->addSpace(this->_category, 1.0f);
}

void KEP::tools::SpawnTool::_changeFactionSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateFactionList(keyword);
}

void KEP::tools::SpawnTool::_changeSquadSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateSquadList(keyword);
}

void KEP::tools::SpawnTool::_changeCharacterSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateCharacterList(keyword);
}

void KEP::tools::SpawnTool::_changeItemSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateItemList(keyword);
	_updateModelList();
}

void KEP::tools::SpawnTool::_changeWeaponSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateWeaponList(keyword);
}

void KEP::tools::SpawnTool::_selectItemList(DataPanelLine* line)
{
	_updateModelList();
}

void KEP::tools::SpawnTool::_selectItemCategoryList(DataPanelLine* line)
{
	_updateItemSubcategorylList();

	std::string keyword = reinterpret_cast<DataPanelLine_TextEditable*>(this->_panel->getLine(lineTextItem, this->_category))->editBox->getOnlyText();

	_updateItemList(keyword);
	_updateModelList();
}

void KEP::tools::SpawnTool::_selectItemSubcategoryList(DataPanelLine* line)
{
	std::string keyword = reinterpret_cast<DataPanelLine_TextEditable*>(this->_panel->getLine(lineTextItem, this->_category))->editBox->getOnlyText();

	_updateItemList(keyword);
	_updateModelList();
}

void KEP::tools::SpawnTool::_selectWeaponCategoryList(DataPanelLine* line)
{
	std::string keyword = reinterpret_cast<DataPanelLine_TextEditable*>(this->_panel->getLine(lineTextWeapon, this->_category))->editBox->getOnlyText();

	_updateWeaponList(keyword);
}

void KEP::tools::SpawnTool::_changeCampaignSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateCampaginList(keyword);
}

void KEP::tools::SpawnTool::_changeFromFactionSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateFromFactionList(keyword);
}

void KEP::tools::SpawnTool::_changeToFactionSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateToFactionList(keyword);
}

void KEP::tools::SpawnTool::_changeUniqueNpcSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateUniqueNpcList(keyword);
}

void KEP::tools::SpawnTool::_changeUniformSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateUniformList(keyword);
}

void KEP::tools::SpawnTool::_changeColorSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateColorList(keyword);
}

namespace
{
	void _spawnPlayerSquad(GameData* squad)
	{
		auto pos = ou->player->getCameraCenter();
		pos.y = UtilityT::getTerrainHeightFast(pos.x, pos.z, nullptr);

		auto platoon = ou->theFactory->createRandomSquad(ou->player->participant, pos, nullptr, 50, nullptr, squad, nullptr, nullptr, nullptr, false, hand(), nullptr, 1.0f, SQ_UNKNOWN, false);
		if (platoon->getCharacterCount() == 0)
		{
			ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Warning: The spawned squad is empty"), true);
		}
	}

	void _spawnRoamingSquad(Faction* faction, GameData* squad)
	{
		auto pos = ou->player->getCameraCenter();
		pos.y = UtilityT::getTerrainHeightFast(pos.x, pos.z, nullptr);

		AreaSector* sector = KEP::functions->AreasList_getAreaSector(shou->areasList, pos);

		TownBase* town = shou->townList->getNearestTown(pos, faction, nullptr, nullptr, TOWN_NULL);
		TownBase* nest = shou->townList->getNearestNest(pos, faction, nullptr);
		TownBase* home = town;
		if (home != nullptr)
		{
			if (nest != nullptr)
			{
				auto townPos = town->getPosition();
				float townDistance = Ogre::Math::Sqrt((townPos.x - pos.x) * (townPos.x - pos.x) + (townPos.z - pos.z) * (townPos.z - pos.z));

				auto nestPos = town->getPosition();
				float nestDistance = Ogre::Math::Sqrt((nestPos.x - pos.x) * (nestPos.x - pos.x) + (nestPos.z - pos.z) * (nestPos.z - pos.z));

				if (nestDistance + 9216.0f < townDistance)
					home = nest;
			}

			if (home != nullptr)
			{
				auto homePos = home->getPosition();
				float homeDistance = Ogre::Math::Sqrt((homePos.x - pos.x) * (homePos.x - pos.x) + (homePos.z - pos.z) * (homePos.z - pos.z));
				if (50000.0f < homeDistance)
					home = nullptr;
			}
		}

		auto platoon = ou->theFactory->createRandomSquad(faction, pos, home, 99, nullptr, squad, nullptr, sector->area, nullptr, false, hand(), nullptr, 1.0f, SQ_ROAMING, false);
		if (platoon->getCharacterCount() == 0)
		{
			ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Warning: The spawned squad is empty"), true);
		}
	}

	void _spawnBarSquad(Faction* faction, GameData* squad)
	{
		auto pos = ou->player->getCameraCenter();
		pos.y = UtilityT::getTerrainHeightFast(pos.x, pos.z, nullptr);

		TownBase* home = shou->townList->getNearestTown(pos, nullptr, nullptr, nullptr, TOWN_NULL);
		
		auto homePos = home->getPosition();
		float homeDistance = Ogre::Math::Sqrt((homePos.x - pos.x) * (homePos.x - pos.x) + (homePos.z - pos.z) * (homePos.z - pos.z));
		float radius = home->getRadius();
		if (radius < homeDistance)
		{
			ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please move the camera within the town range."), true);
			return;
		}

		auto platoon = ou->theFactory->createRandomSquad(faction, pos, home, 99, nullptr, squad, nullptr, nullptr, nullptr, true, hand(), nullptr, 1.0f, SQ_RESIDENT, false);
		if (platoon->getCharacterCount() == 0)
		{
			ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Warning: The spawned squad is empty"), true);
		}
	}

	void _spawnResidentSquad(Faction* faction, GameData* squad)
	{
		auto building = gui->selectedObject.getBuilding();
		if (building == nullptr)
		{
			ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a building."), true);
			return;
		}

		if (building->imADoor)
			building = building->doorStuff()->doorParentBuilding();

		if (building == nullptr || building->myInterior == nullptr && building->isGate() == nullptr || building->isAWall() != nullptr && building->isGate() == nullptr)
		{
			ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Squads cannot spawn in this building."), true);
			return;
		}

		auto home = building->getRealTown();
		if (home == nullptr)
		{
			ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a building in the town."), true);
			return;
		}

		lektor<GameData*> datas;
		std::set<GameData*> likes;
		squad->getAllFromListAsDatas("building", datas, &ou->gamedata, BUILDING);
		for (auto iter = datas.begin(); iter != datas.end(); ++iter)
		{
			likes.insert(*iter);
		}

		bool matched = false;
		if (building->isGate() == nullptr)
		{
			const auto& layout = squad->sdata["layout interior"];
			if (!layout.empty())
			{
				matched = KEP::functions->BuildingInteriorDatasManager_hasLayout(KEP::functions->getInteriorMgr(), building->data, layout, true);
			}
		}
		else
		{
			matched = likes.find(building->data) != likes.end();
		}

		if (!matched)
		{
			ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: The interior layout did not match."), true);
			return;
		}

		auto owner = building->getFaction();
		auto& handle = building->getHandle();
		owner->factionOwnerships->removeOwnedObject(handle);

		lektor<Platoon*> squads;
		owner->getAllSquadsThatOwn(squads, building);
		lektor<RootObject*> destroyChars;

		for (auto iter = squads.begin(); iter != squads.end(); ++iter)
		{
			auto ownerships = (*iter)->getOwnerships();
			ownerships->_homeBuilding = KEP::functions->getNULL_HAND();
			ownerships->removeOwnedObject(handle);

			auto activePlatoon = (*iter)->activePlatoon;
			if (activePlatoon != nullptr)
			{
				for (auto charaIter = activePlatoon->things.begin(); charaIter != activePlatoon->things.end(); ++charaIter)
				{
					destroyChars.push_back(*charaIter);
				}
			}
			(*iter)->setPersistentSquad(false);
		}

		for (auto iter = destroyChars.begin(); iter != destroyChars.end(); ++iter)
		{
			ou->destroy(*iter, false, "_spawnResidentSquad");
		}

		building->residentSquad.setNull();

		if (building->myInterior != nullptr)
		{
			for (auto iter = building->myInterior->furnitures.begin(); iter != building->myInterior->furnitures.end(); ++iter)
			{
				auto furniture = iter->getBuilding();
				if (furniture != nullptr)
				{
					auto stuff = furniture->getUseableStuff();
					if (stuff != nullptr)
					{
						if (stuff->currentOperators.size() != 0)
						{
							auto it = stuff->currentOperators.begin();
							auto character = it->getCharacter();
							if (character != nullptr)
							{
								character->setBedMode(false, nullptr);
								character->setPrisonMode(false, nullptr);
							}
						}
					}
				}
			}
		}

		if (building->isDestroyed())
		{
			building->setDestroyed(false);
			auto buildState = building->getBuildState();
			building->addConstructionProgress(10000000.f);
			buildState->constructionProgress = buildState->totalMats;
		}

		auto residentFaction = ou->factionMgr->getFactionByStringID(squad->getFromList("faction", 0));
		if (residentFaction == nullptr)
			residentFaction = home->getFaction();

		bool needDelete = home->factionsResidentHere.find(residentFaction) == home->factionsResidentHere.end();

		KEP::functions->BuildingInterior_destroyAllInternalBuildings(building->myInterior, true);
		KEP::functions->NodeList_destroyNodesByBuilding(ou->nodeList, building->handle, false);

		if (!home->_setMainResident(building, squad, false))
		{
			ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: The interior layout did not match."), true);
			return;
		}

		building->setFaction(faction, nullptr);

		if (needDelete)
			home->factionsResidentHere.erase(residentFaction);
		
		ou->theFactory->populateBuilding(building);
	}
}

void KEP::tools::SpawnTool::_spawnSquad(DataPanelLine* line)
{
	if (this->_selectedSpawnFaction < 0)
		return;

	if (this->_selectedSpawnSquad < 0)
		return;

	auto faction = ou->factionMgr->getFactionByStringID(this->_spawnFactionList[this->_selectedSpawnFaction]->stringID);
	auto squad = this->_spawnSquadList[this->_selectedSpawnSquad];

	if (faction->isPlayer != nullptr)
		_spawnPlayerSquad(squad);
	else if (this->_squadSpawnMethodType == 0)
		_spawnRoamingSquad(faction, squad);
	else if (this->_squadSpawnMethodType == 1)
		_spawnBarSquad(faction, squad);
	else if (this->_squadSpawnMethodType == 2)
		_spawnResidentSquad(faction, squad);
}

void KEP::tools::SpawnTool::_addCharacter(DataPanelLine* line)
{
	if (this->_selectedSpawnCharacter < 0)
		return;

	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character."), true);
		return;
	}

	auto platoon = character->getPlatoon();
	if (platoon == nullptr)
		return;

	auto dat = this->_spawnCharacterList[this->_selectedSpawnCharacter];
	if (dat->type != CHARACTER && dat->type != ANIMAL_CHARACTER)
		return;

	float age = 1.0f;
	if (dat->type == ANIMAL_CHARACTER)
		age = this->_animalAge;
	if (age <= 0.0f)
		age = 1.0f;

	auto home = platoon->me->getOwnerships()->_homeBuilding.getBuilding();

	auto obj = reinterpret_cast<Character*>(ou->theFactory->createRandomCharacter(platoon->me->getFaction(), platoon->me->getPosition(), platoon, dat, home, age));
	if (obj != nullptr)
		obj->setSquadMemberType(SQUAD_1);
}

void KEP::tools::SpawnTool::_giveItem(DataPanelLine* line)
{
	if (this->_selectedItem < 0)
		return;

	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character."), true);
		return;
	}

	auto data = this->_spawnItemList[this->_selectedItem];
	GameData* weapon = nullptr;
	GameData* model = nullptr;

	int level = 0;
	if (data->type == ARMOUR || data->type == CROSSBOW || data->type == LIMB_REPLACEMENT)
		level = getLevel(this->_gearLevel);

	if (data->type == WEAPON_MANUFACTURER)
	{
		if (-1 < this->_selectedWeapon)
			weapon = this->_spawnWeaponList[this->_selectedWeapon];
		if (-1 < this->_selectedModel)
			model = this->_spawnModelList[this->_selectedModel];
	}

	Faction* uniform = nullptr;
	if (data->type == ARMOUR && -1 < this->_selectedFactionUniform)
		uniform = ou->factionMgr->getFactionByStringID(this->_spawnFactionList[this->_selectedFactionUniform]->stringID);

	int quantity = static_cast<int>(this->_quantity);
	for (int i = 0; i < quantity; i++)
	{
		auto item = ou->theFactory->createItem(data, hand(), weapon, model, level, uniform);
		if (item->coloriseData == nullptr && -1 < this->_selectedItemColor && (item->slotType != ATTACH_NONE && item->slotType != ATTACH_HAIR && item->slotType != ATTACH_BEARD))
			item->coloriseData = this->_itemColorList[this->_selectedItemColor];

		if (item->objectType == LIMB_REPLACEMENT && character->medical.robotLimbs != nullptr)
		{
			RobotLimbs::Limb limb = RobotLimbs::NULL_LIMB;
			auto slot = item->slotType;
			if (slot == ATTACH_LEFT_ARM)
				limb = RobotLimbs::LEFT_ARM;
			else if (slot == ATTACH_RIGHT_ARM)
				limb = RobotLimbs::RIGHT_ARM;
			else if (slot == ATTACH_LEFT_LEG)
				limb = RobotLimbs::LEFT_LEG;
			else if (slot == ATTACH_RIGHT_LEG)
				limb = RobotLimbs::RIGHT_LEG;

			if (limb < RobotLimbs::NULL_LIMB && character->medical.robotLimbs->getState(limb) == LIMB_STUMP)
			{
				character->medical.setRobotLimbItem(limb, item, false);
				return;
			}
		}

		character->giveItem(item, true, false);
	}
}

void KEP::tools::SpawnTool::_triggerCampaign(DataPanelLine* line)
{
	if (this->_selectedCampaign < 0)
		return;

	auto data = this->_campaignList[this->_selectedCampaign];
	auto faction = ou->factionMgr->getFactionByStringID(data->getFromList("faction override", 0));
	if (faction == nullptr)
	{
		lektor<GameData*> datas;
		ou->gamedata.findAllDataThatReferencesThis(datas, data, FACTION, "campaigns");
		if (datas.size() != 0)
			faction = ou->factionMgr->getFactionByStringID(datas[0]->stringID);

		if (faction == nullptr)
		{
			ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Faction not found. Please configure [campaigns] or [faction override]."), true);
			return;
		}
	}
	auto town = shou->townList->getNearestPlayerTown(ou->player->getCameraCenter(), false, 1000000.0f);
	if (town == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Player base not found."), true);
		return;
	}

	faction->warMgr->triggerCampaign(town, data, 0, 0, nullptr, false, faction);
}

void KEP::tools::SpawnTool::_refreshInventory(DataPanelLine* line)
{
	ActivePlatoon* activePlatoon = nullptr;
	auto building = gui->selectedObject.getBuilding();
	if (building != nullptr)
	{
		if (building->isFurnitureOrDoor())
			building = building->furnitureParentBuilding();
	}

	auto character = gui->selectedObject.getCharacter();
	if (character != nullptr)
	{
		if (character->isPlayerCharacter())
			building = character->isInsideBuilding.getBuilding();
		else
		{
			building = character->getOwnerships()->_homeBuilding.getBuilding();
			if (building == nullptr)
				activePlatoon = character->getPlatoon();
		}
	}

	if (building != nullptr)
	{
		auto platoon = building->residentSquad.getPlatoon();
		if (platoon != nullptr)
			activePlatoon = platoon->activePlatoon;
	}

	if (activePlatoon != nullptr)
		activePlatoon->_forceRefreshInventory();
	else if (building != nullptr && building->residentSquadTemplate != nullptr)
	{
		auto invMgr = KEP::functions->InventoryManager_getSingleton();
		KEP::functions->InventoryManager_clearAllBuildingInventory(invMgr, building);
		KEP::functions->InventoryManager_refreshBuildingInventory(invMgr, building, building->residentSquadTemplate, nullptr, true, true);
	}
	else
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Player or unoccupied building inventories cannot be updated."), true);
}

void KEP::tools::SpawnTool::_overwriteRelation(DataPanelLine* line)
{
	if (this->_selectedFromFaction < 0)
		return;

	if (this->_selectedToFaction < 0)
		return;

	auto fromFaction = ou->factionMgr->getFactionByStringID(this->_spawnFactionList[this->_selectedFromFaction]->stringID);
	auto toFaction = ou->factionMgr->getFactionByStringID(this->_spawnFactionList[this->_selectedToFaction]->stringID);

	if (fromFaction->isPlayer != nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: For the \"from\" field, please select a faction other than the player."), true);
		return;
	}

	if (fromFaction == toFaction)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a different faction."), true);
		return;
	}

	fromFaction->relations->setRelation(toFaction, this->_relation);
}

void KEP::tools::SpawnTool::_changeUniqueNpcState(DataPanelLine* line)
{
	if (this->_selectedUniqueCharacter < 0)
		return;

	auto data = this->_uniqueCharacterList[this->_selectedUniqueCharacter];

	auto uniqueNpcMgr = KEP::functions->UniqueNPCManager_getSingleton();
	if (uniqueNpcMgr->uniqueCharacterStates.find(data) != uniqueNpcMgr->uniqueCharacterStates.end())
	{
		if (this->_uniqueState == 0)
		{
			uniqueNpcMgr->uniqueCharacterStates.erase(data);
			if (this->_uniquePrisonerList.find(data) != this->_uniquePrisonerList.end())
				uniqueNpcMgr->uniqueCharacterStates[data].state = IMPRISONED;
		}
		else if (this->_uniqueState == 1)
		{
			auto& stateData = uniqueNpcMgr->uniqueCharacterStates[data];
			stateData.state = ALIVE;
			stateData.playerInvolvement = false;
		}
		else if (this->_uniqueState == 2)
		{
			auto& stateData = uniqueNpcMgr->uniqueCharacterStates[data];
			stateData.state = IMPRISONED;
			stateData.playerInvolvement = false;
		}
		else if (this->_uniqueState == 3)
		{
			auto& stateData = uniqueNpcMgr->uniqueCharacterStates[data];
			stateData.state = IMPRISONED;
			stateData.playerInvolvement = true;
		}
		else if (this->_uniqueState == 4)
		{
			auto& stateData = uniqueNpcMgr->uniqueCharacterStates[data];
			stateData.state = DEAD;
			stateData.playerInvolvement = false;
		}
		else if (this->_uniqueState == 5)
		{
			auto& stateData = uniqueNpcMgr->uniqueCharacterStates[data];
			stateData.state = DEAD;
			stateData.playerInvolvement = true;
		}
	}
	else
	{
		if (this->_uniqueState == 1)
		{
			auto& stateData = uniqueNpcMgr->uniqueCharacterStates[data];
			stateData.characterTemplate = data;
			stateData.handle = KEP::functions->getNULL_HAND();
			stateData.state = ALIVE;
			stateData.playerInvolvement = false;
		}
		else if (this->_uniqueState == 2)
		{
			auto& stateData = uniqueNpcMgr->uniqueCharacterStates[data];
			stateData.characterTemplate = data;
			stateData.handle = KEP::functions->getNULL_HAND();
			stateData.state = IMPRISONED;
			stateData.playerInvolvement = false;
		}
		else if (this->_uniqueState == 3)
		{
			auto& stateData = uniqueNpcMgr->uniqueCharacterStates[data];
			stateData.characterTemplate = data;
			stateData.handle = KEP::functions->getNULL_HAND();
			stateData.state = IMPRISONED;
			stateData.playerInvolvement = true;
		}
		else if (this->_uniqueState == 4)
		{
			auto& stateData = uniqueNpcMgr->uniqueCharacterStates[data];
			stateData.characterTemplate = data;
			stateData.handle = KEP::functions->getNULL_HAND();
			stateData.state = DEAD;
			stateData.playerInvolvement = false;
		}
		else if (this->_uniqueState == 5)
		{
			auto& stateData = uniqueNpcMgr->uniqueCharacterStates[data];
			stateData.characterTemplate = data;
			stateData.handle = KEP::functions->getNULL_HAND();
			stateData.state = DEAD;
			stateData.playerInvolvement = true;
		}
	}
}

void KEP::tools::SpawnTool::_initFactionList()
{
	lektor<GameData*> datas;
	ou->gamedata.getDataOfType(datas, FACTION);
	std::sort(datas.begin(), datas.end(), LessGameData());

	this->_spawnFactionList.reserve(datas.size());

	auto playerFaction = ou->player->participant->data;
	this->_spawnFactionList.push_back(playerFaction);

	for (uint32_t i = 0; i < datas.size(); ++i)
	{
		if (datas[i] != playerFaction)
			this->_spawnFactionList.push_back(datas[i]);
	}
}

void KEP::tools::SpawnTool::_initSquadList()
{
	lektor<GameData*> datas;
	ou->gamedata.getDataOfType(datas, SQUAD_TEMPLATE);
	std::sort(datas.begin(), datas.end(), LessGameData());

	this->_spawnSquadList.resize(datas.size());
	for (uint32_t i = 0; i < datas.size(); ++i)
	{
		this->_spawnSquadList[i] = datas[i];
	}
}

void KEP::tools::SpawnTool::_initCharacterList()
{
	lektor<GameData*> datas;
	ou->gamedata.getDataOfType(datas, CHARACTER);
	ou->gamedata.getDataOfType(datas, ANIMAL_CHARACTER);
	std::sort(datas.begin(), datas.end(), LessGameData());

	this->_spawnCharacterList.resize(datas.size());
	for (uint32_t i = 0; i < datas.size(); ++i)
	{
		this->_spawnCharacterList[i] = datas[i];
		if (datas[i]->bdata["unique"])
			this->_uniqueCharacterList.push_back(datas[i]);
	}
}

void KEP::tools::SpawnTool::_initItemList()
{
	lektor<GameData*> datas;
	ou->gamedata.getDataOfType(datas, ITEM);
	ou->gamedata.getDataOfType(datas, MAP_ITEM);
	ou->gamedata.getDataOfType(datas, NEST_ITEM);
	ou->gamedata.getDataOfType(datas, WEAPON_MANUFACTURER);
	ou->gamedata.getDataOfType(datas, ARMOUR);
	ou->gamedata.getDataOfType(datas, CONTAINER);
	ou->gamedata.getDataOfType(datas, CROSSBOW);
	ou->gamedata.getDataOfType(datas, LIMB_REPLACEMENT);
	ou->gamedata.getDataOfType(datas, RESEARCH);
	std::sort(datas.begin(), datas.end(), LessGameData());
	this->_spawnItemList.reserve(datas.size());
	for (uint32_t i = 0; i < datas.size(); ++i)
	{
		auto dat = datas[i];
		if (dat->type != NEST_ITEM || !dat->filesdata["physics file"].empty())
			this->_spawnItemList.push_back(dat);
	}
}

void KEP::tools::SpawnTool::_initWeaponList()
{
	lektor<GameData*> datas;
	ou->gamedata.getDataOfType(datas, WEAPON);
	std::sort(datas.begin(), datas.end(), LessGameData());

	this->_spawnWeaponList.reserve(datas.size());
	for (uint32_t i = 0; i < datas.size(); ++i)
	{
		if (datas[i]->idata["skill category"] < 9)
			this->_spawnWeaponList.push_back(datas[i]);
	}
}

void KEP::tools::SpawnTool::_initModelList()
{
	lektor<GameData*> datas;
	ou->gamedata.getDataOfType(datas, MATERIAL_SPECS_WEAPON);
	std::sort(datas.begin(), datas.end(), LessGameData());

	this->_spawnModelList.resize(datas.size());
	for (uint32_t i = 0; i < datas.size(); ++i)
	{
		this->_spawnModelList[i] = datas[i];
	}
}

void KEP::tools::SpawnTool::_initCampaignList()
{
	lektor<GameData*> datas;
	ou->gamedata.getDataOfType(datas, FACTION_CAMPAIGN);
	std::sort(datas.begin(), datas.end(), LessGameData());

	this->_campaignList.resize(datas.size());
	for (uint32_t i = 0; i < datas.size(); ++i)
	{
		this->_campaignList[i] = datas[i];
	}
}

void KEP::tools::SpawnTool::_initUniqueCharacters()
{
	lektor<GameData*> datas;
	ou->gamedata.getDataOfType(datas, SQUAD_TEMPLATE);

	lektor<GameData*> prisoners;

	for (auto iter = datas.begin(); iter != datas.end(); ++iter)
	{
		auto& prisonSquads = (*iter)->objectReferences["prisoners"];
		for (auto prisonSquadsIt = prisonSquads.begin(); prisonSquadsIt != prisonSquads.end(); ++prisonSquadsIt)
		{
			auto prisonSquad = ou->gamedata.getData(prisonSquadsIt->sid, SQUAD_TEMPLATE);
			prisonSquad->getAllFromListAsDatas("leader", prisoners, &ou->gamedata, CHARACTER);
			prisonSquad->getAllFromListAsDatas("squad", prisoners, &ou->gamedata, CHARACTER);
			prisonSquad->getAllFromListAsDatas("squad2", prisoners, &ou->gamedata, CHARACTER);
		}
	}

	for (auto iter = prisoners.begin(); iter != prisoners.end(); ++iter)
	{
		this->_uniquePrisonerList.insert(*iter);
	}
}

void KEP::tools::SpawnTool::_initColorList()
{
	lektor<GameData*> datas;
	ou->gamedata.getDataOfType(datas, COLOR_DATA);
	std::sort(datas.begin(), datas.end(), LessGameData());

	this->_itemColorList.resize(datas.size());
	for (uint32_t i = 0; i < datas.size(); ++i)
	{
		this->_itemColorList[i] = datas[i];
	}
}


void KEP::tools::SpawnTool::_updateFactionList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxFaction, this->_category));
	if (dropBox == nullptr)
		return;

	int currentSelected = this->_selectedSpawnFaction;
	if (currentSelected < 0)
		currentSelected = 0;
	int selectVal = 0;
	dropBox->clearValues();
	dropBox->addAValue(lineLabelPlayerFaction, 0);
	if (keyword.empty())
	{
		selectVal = currentSelected;
		for (uint32_t i = 1; i < this->_spawnFactionList.size(); ++i)
		{
			dropBox->addAValue(this->_spawnFactionList[i]->name, i);
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 1; i < this->_spawnFactionList.size(); ++i)
		{
			std::string& name = this->_spawnFactionList[i]->name;
			std::string s2 = name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if (s2.find(s1) != std::string::npos)
			{
				dropBox->addAValue(name, i);
				if (selectVal == 0 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}

void KEP::tools::SpawnTool::_updateSquadList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxSquads, this->_category));
	if (dropBox == nullptr)
		return;

	int currentSelected = this->_selectedSpawnSquad;
	if (currentSelected < 0)
		currentSelected = 0;
	int selectVal = -1;
	dropBox->clearValues();
	if (keyword.empty())
	{
		selectVal = currentSelected;
		for (uint32_t i = 0; i < this->_spawnSquadList.size(); ++i)
		{
			dropBox->addAValue(this->_spawnSquadList[i]->name, i);
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 0; i < this->_spawnSquadList.size(); ++i)
		{
			std::string& name = this->_spawnSquadList[i]->name;
			std::string s2 = name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if (s2.find(s1) != std::string::npos)
			{
				dropBox->addAValue(name, i);
				if (selectVal == -1 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}

void KEP::tools::SpawnTool::_updateCharacterList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxCharacter, this->_category));
	if (dropBox == nullptr)
		return;

	int currentSelected = this->_selectedSpawnCharacter;
	if (currentSelected < 0)
		currentSelected = 0;
	int selectVal = -1;
	dropBox->clearValues();
	if (keyword.empty())
	{
		selectVal = currentSelected;
		for (uint32_t i = 0; i < this->_spawnCharacterList.size(); ++i)
		{
			dropBox->addAValue(this->_spawnCharacterList[i]->name, i);
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 0; i < this->_spawnCharacterList.size(); ++i)
		{
			std::string& name = this->_spawnCharacterList[i]->name;
			std::string s2 = name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if (s2.find(s1) != std::string::npos)
			{
				dropBox->addAValue(name, i);
				if (selectVal == -1 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}

namespace
{
	bool _checkItemCategory(GameData* dat, int category)
	{
		if (category == 0)
			return true;

		if (dat->type == ITEM)
		{
			int itemFunc = dat->idata["item function"];
			int slot = dat->idata["slot"];
			if (category == 1)
				return itemFunc == ITEM_NO_FUNCTION || itemFunc == ITEM____ || itemFunc == ITEM_ANYTHING;
			else if (category == 2)
				return itemFunc == ITEM_FIRSTAID || itemFunc == ITEM_MEDRIGGING || itemFunc == ITEM_ROBOTREPAIR;
			else if (category == 3)
				return itemFunc == ITEM_FOOD;
			else if (category == 4)
				return itemFunc == ITEM_FOOD_RESTRICTED;
			else if (category == 5)
				return itemFunc == ITEM_NARCOTIC;
			else if (category == 6)
				return itemFunc == ITEM_TOOL;
			else if (category == 7)
				return itemFunc == ITEM_AMMO;
			else if (category == 8)
				return dat->bdata["artifact"];
			else if (category == 9)
				return itemFunc == ITEM_BOOK;
			else if (category == 10)
				return itemFunc == ITEM_MONEY;
			else if (category == 11)
				return itemFunc == ITEM_SEVERED_LIMB;
			else if (category == 21)
				return slot == ATTACH_BELT;
			else if (category == 27)
				return slot == ATTACH_EYES;
			else if (category == 28)
				return slot == ATTACH_GLOVES;
			else if (category == 29)
				return slot == ATTACH_NECK;
		}
		else if (dat->type == ARMOUR)
		{
			int slot = dat->idata["slot"];
			if (category == 16)
				return slot == ATTACH_HAT;
			else if (category == 17)
				return slot == ATTACH_BODY;
			else if (category == 18)
				return slot == ATTACH_SHIRT;
			else if (category == 19)
				return slot == ATTACH_LEGS;
			else if (category == 20)
				return slot == ATTACH_BOOTS;
			else if (category == 21)
				return slot == ATTACH_BELT;
			else if (category == 22)
				return slot == ATTACH_BACKPACK;
			else if (category == 27)
				return slot == ATTACH_EYES;
			else if (category == 28)
				return slot == ATTACH_GLOVES;
			else if (category == 29)
				return slot == ATTACH_NECK;
		}
		else if (dat->type == CONTAINER)
		{
			int slot = dat->idata["slot"];
			if (category == 22)
				return slot == ATTACH_BACKPACK;
		}
		else if (dat->type == LIMB_REPLACEMENT)
		{
			int slot = dat->idata["slot"];
			if (category == 23)
				return slot == ATTACH_LEFT_ARM;
			else if (category == 24)
				return slot == ATTACH_RIGHT_ARM;
			else if (category == 25)
				return slot == ATTACH_LEFT_LEG;
			else if (category == 26)
				return slot == ATTACH_RIGHT_LEG;
		}

		return false;
	}

	bool _checkSlot(GameData* dat, int slot)
	{
		if ((dat->type != ITEM && dat->type != ARMOUR && dat->type != LIMB_REPLACEMENT) || slot < 0)
			return true;
		return slot == dat->idata["slot"];
	}
}

void KEP::tools::SpawnTool::_updateItemList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxItem, this->_category));
	if (dropBox == nullptr)
		return;

	int currentSelected = this->_selectedItem;
	if (currentSelected < 0)
		currentSelected = 0;
	int selectVal = -1;
	dropBox->clearValues();
	if (keyword.empty())
	{
		for (uint32_t i = 0; i < this->_spawnItemList.size(); ++i)
		{
			auto data = this->_spawnItemList[i];
			if ((this->_searchItemCategory == NULL_ITEM || data->type == this->_searchItemCategory) && _checkItemCategory(data, this->_searchItemSubCategory))
			{
				if (data->type != RESEARCH)
					dropBox->addAValue(data->name, i);
				else
					dropBox->addAValue(data->name + blueprintSuffix, i);
				if (selectVal == -1 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 0; i < this->_spawnItemList.size(); ++i)
		{
			auto data = this->_spawnItemList[i];
			std::string s2 = data->name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if ((this->_searchItemCategory == NULL_ITEM || data->type == this->_searchItemCategory) && _checkItemCategory(data, this->_searchItemSubCategory) && s2.find(s1) != std::string::npos)
			{
				if (data->type != RESEARCH)
					dropBox->addAValue(data->name, i);
				else
					dropBox->addAValue(data->name + blueprintSuffix, i);
				if (selectVal == -1 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}

void KEP::tools::SpawnTool::_updateWeaponList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxWeapon, this->_category));
	if (dropBox == nullptr)
		return;

	int category = this->_searchWeaponCategory - 1;

	int currentSelected = this->_selectedWeapon;
	int selectVal = -1;
	dropBox->clearValues();
	dropBox->addAValue(lineLabelRandom, -1);
	if (keyword.empty())
	{
		for (uint32_t i = 0; i < this->_spawnWeaponList.size(); ++i)
		{
			auto data = this->_spawnWeaponList[i];
			if (category < 0 || data->idata["skill category"] == category)
			{
				dropBox->addAValue(data->name, i);
				if (selectVal == -1 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 0; i < this->_spawnWeaponList.size(); ++i)
		{
			auto data = this->_spawnWeaponList[i];
			std::string& name = data->name;
			std::string s2 = name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if ((category < 0 || data->idata["skill category"] == category) && s2.find(s1) != std::string::npos)
			{
				dropBox->addAValue(name, i);
				if (selectVal == -1 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}

void KEP::tools::SpawnTool::_updateModelList()
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxModel, this->_category));
	if (dropBox == nullptr)
		return;

	GameData* manufacturer = nullptr;
	if (0 <= this->_selectedItem)
	{
		auto data = this->_spawnItemList[this->_selectedItem];
		if (data->type == WEAPON_MANUFACTURER)
			manufacturer = data;
	}

	int currentSelected = this->_selectedModel;
	int selectVal = -1;
	dropBox->clearValues();
	dropBox->addAValue(lineLabelRandom, -1);
	if (manufacturer != nullptr)
	{
		auto list = manufacturer->getReferenceListIfExists("weapon models");
		if (list != nullptr)
		{
			for (uint32_t i = 0; i < this->_spawnModelList.size(); ++i)
			{
				auto model = this->_spawnModelList[i];
				for (auto iter = list->begin(); iter != list->end(); ++iter)
				{
					if (iter->sid == model->stringID)
					{
						dropBox->addAValue(model->name, i);
						if (selectVal == -1 || i == currentSelected)
							selectVal = i;
						break;
					}
				}
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}

void KEP::tools::SpawnTool::_updateItemSubcategorylList()
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxtemSubCategory, this->_category));
	if (dropBox == nullptr)
		return;

	dropBox->clearValues();
	dropBox->addAValue(KEP::TranslationUtility::gettext("All"), 0);
	if (this->_searchItemCategory == ITEM)
	{
		dropBox->addAValue(KEP::TranslationUtility::gettext("General item"), 1);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Medical or repair"), 2);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Food"), 3);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Restricted food"), 4);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Alcohol or drug"), 5);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Tool"), 6);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Ammo"), 7);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Research"), 8);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Book"), 9);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Money"), 10);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Servered limb"), 11);
	}
	if (this->_searchItemCategory == ARMOUR)
	{
		dropBox->addAValue(KEP::TranslationUtility::gettext_main("Head"), 16);
		dropBox->addAValue(KEP::TranslationUtility::gettext_main("Armor"), 17);
		dropBox->addAValue(KEP::TranslationUtility::gettext_main("Shirt"), 18);
		dropBox->addAValue(KEP::TranslationUtility::gettext_main("Pants"), 19);
		dropBox->addAValue(KEP::TranslationUtility::gettext_main("Boots"), 20);
		dropBox->addAValue(KEP::TranslationUtility::gettext_main("Belt"), 21);
		dropBox->addAValue(KEP::TranslationUtility::gettext_main("Backpack"), 22);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Eyes"), 27);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Gloves"), 28);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Neck"), 29);
	}
	if (this->_searchItemCategory == LIMB_REPLACEMENT)
	{
		dropBox->addAValue(KEP::TranslationUtility::gettext("Left Arm"), 23);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Right Arm"), 24);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Left Leg"), 25);
		dropBox->addAValue(KEP::TranslationUtility::gettext("Right Leg"), 26);
	}
	dropBox->setSelectedValue(0);
}

void KEP::tools::SpawnTool::_updateCampaginList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxCampaign, this->_category));
	if (dropBox == nullptr)
		return;

	int currentSelected = this->_selectedCampaign;
	if (currentSelected < 0)
		currentSelected = 0;
	int selectVal = -1;
	dropBox->clearValues();
	if (keyword.empty())
	{
		selectVal = currentSelected;
		for (uint32_t i = 0; i < this->_campaignList.size(); ++i)
		{
			dropBox->addAValue(this->_campaignList[i]->name, i);
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 0; i < this->_campaignList.size(); ++i)
		{
			std::string& name = this->_campaignList[i]->name;
			std::string s2 = name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if (s2.find(s1) != std::string::npos)
			{
				dropBox->addAValue(name, i);
				if (selectVal == -1 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}

void KEP::tools::SpawnTool::_updateFromFactionList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxFromFaction, this->_category));
	if (dropBox == nullptr)
		return;

	int currentSelected = this->_selectedFromFaction;
	if (currentSelected < 1)
		currentSelected = 1;
	int selectVal = -1;
	dropBox->clearValues();
	if (keyword.empty())
	{
		selectVal = currentSelected;
		for (uint32_t i = 1; i < this->_spawnFactionList.size(); ++i)
		{
			dropBox->addAValue(this->_spawnFactionList[i]->name, i);
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 1; i < this->_spawnFactionList.size(); ++i)
		{
			std::string& name = this->_spawnFactionList[i]->name;
			std::string s2 = name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if (s2.find(s1) != std::string::npos)
			{
				dropBox->addAValue(name, i);
				if (selectVal == -1 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}

void KEP::tools::SpawnTool::_updateToFactionList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxToFaction, this->_category));
	if (dropBox == nullptr)
		return;

	int currentSelected = this->_selectedToFaction;
	if (currentSelected < 0)
		currentSelected = 0;
	int selectVal = 0;
	dropBox->clearValues();
	dropBox->addAValue(lineLabelPlayerFaction, 0);
	if (keyword.empty())
	{
		selectVal = currentSelected;
		for (uint32_t i = 1; i < this->_spawnFactionList.size(); ++i)
		{
			dropBox->addAValue(this->_spawnFactionList[i]->name, i);
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 1; i < this->_spawnFactionList.size(); ++i)
		{
			std::string& name = this->_spawnFactionList[i]->name;
			std::string s2 = name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if (s2.find(s1) != std::string::npos)
			{
				dropBox->addAValue(name, i);
				if (selectVal == 0 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}

void KEP::tools::SpawnTool::_updateUniqueNpcList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxUniqueNpc, this->_category));
	if (dropBox == nullptr)
		return;

	int currentSelected = this->_selectedUniqueCharacter;
	if (currentSelected < 0)
		currentSelected = 0;
	int selectVal = -1;
	dropBox->clearValues();
	if (keyword.empty())
	{
		selectVal = currentSelected;
		for (uint32_t i = 0; i < this->_uniqueCharacterList.size(); ++i)
		{
			dropBox->addAValue(this->_uniqueCharacterList[i]->name, i);
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 0; i < this->_uniqueCharacterList.size(); ++i)
		{
			std::string& name = this->_uniqueCharacterList[i]->name;
			std::string s2 = name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if (s2.find(s1) != std::string::npos)
			{
				dropBox->addAValue(name, i);
				if (selectVal == -1 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}

void KEP::tools::SpawnTool::_updateUniformList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxFactionUniform, this->_category));
	if (dropBox == nullptr)
		return;

	int currentSelected = this->_selectedFactionUniform;
	if (currentSelected < 0)
		currentSelected = -2;
	int selectVal = -2;
	dropBox->clearValues();
	dropBox->addAValue("-", -2);
	dropBox->addAValue(lineLabelPlayerFaction, 0);
	if (keyword.empty())
	{
		selectVal = currentSelected;
		for (uint32_t i = 1; i < this->_spawnFactionList.size(); ++i)
		{
			dropBox->addAValue(this->_spawnFactionList[i]->name, i);
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 1; i < this->_spawnFactionList.size(); ++i)
		{
			std::string& name = this->_spawnFactionList[i]->name;
			std::string s2 = name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if (s2.find(s1) != std::string::npos)
			{
				dropBox->addAValue(name, i);
				if (selectVal == -2 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}

void KEP::tools::SpawnTool::_updateColorList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxItemColor, this->_category));
	if (dropBox == nullptr)
		return;

	int currentSelected = this->_selectedItemColor;
	if (currentSelected < 0)
		currentSelected = -2;
	int selectVal = -2;
	dropBox->clearValues();
	dropBox->addAValue("-", -2);
	if (keyword.empty())
	{
		selectVal = currentSelected;
		for (uint32_t i = 0; i < this->_itemColorList.size(); ++i)
		{
			dropBox->addAValue(this->_itemColorList[i]->name, i);
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 0; i < this->_itemColorList.size(); ++i)
		{
			std::string& name = this->_itemColorList[i]->name;
			std::string s2 = name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if (s2.find(s1) != std::string::npos)
			{
				dropBox->addAValue(name, i);
				if (selectVal == -2 || i == currentSelected)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}
