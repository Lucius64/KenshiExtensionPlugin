#include "pch.h"
#include <GamePlayOptionsTab.h>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <mygui/MyGUI.h>

#include <Debug.h>
#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <kenshi/Globals.h>
#include <kenshi/SaveInfo.h>
#include <kenshi/gui/DatapanelGUI.h>
#include <kenshi/gui/DataPanelLine.h>

#include <kep/translation.h>
#include <kep/functions.h>
#include <kep/utility.h>
#include <utility.h>

namespace
{
	rapidjson::Document settingsDocument;

	void generateDefaultDocument()
	{
		settingsDocument.SetObject();
		settingsDocument.AddMember("use_level_editor_camera", false, settingsDocument.GetAllocator());
		settingsDocument.AddMember("fast_travel_mode", false, settingsDocument.GetAllocator());
		settingsDocument.AddMember("npc_inventory", false, settingsDocument.GetAllocator());
	}
}

KEP::tools::GamePlayOptionsTab::GamePlayOptionsTab()
	: _panel(nullptr)
	, _category(0x6)
	, _settingsPath(getModulePath() + "\\kep_devtools_settings.json")
	, _useLevelEditorCamera(false)
	, _enableFastTravelMap(false)
	, _enableNpcInventory(false)
{
	_loadJson(this->_settingsPath);
}

void KEP::tools::GamePlayOptionsTab::refresh()
{
	if (this->_panel == nullptr)
		return;

	this->_panel->clearPage(this->_category);

	auto gameplayOptions = KEP::functions->getGameplayOptions();

	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext("Developer tools"), "", this->_category, false, true);

	this->_panel->setLineCheckbox(KEP::TranslationUtility::gettext("Use Level Editor camera"), &this->_useLevelEditorCamera, this->_category);
	this->_panel->setLineCheckbox(KEP::TranslationUtility::gettext("Fast travel mode"), &this->_enableFastTravelMap, this->_category);
	this->_panel->setLineCheckbox(KEP::TranslationUtility::gettext("NPC Inventory"), &this->_enableNpcInventory, this->_category);

	auto button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext_main("Save config"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<GamePlayOptionsTab, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &GamePlayOptionsTab::_saveSettings);

	this->_panel->addSpace(this->_category, 1.0f);

	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext_main("Advanced options"), "", this->_category, false, true);

	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("Hunger time"), this->_category, true, 0.25f, 8.0f, &gameplayOptions->hungerTime);
	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("Chance of death"), this->_category, true, 0.5f, 4.0f, &gameplayOptions->deathFrequency);
	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("Global damage multiplier"), this->_category, true, 0.5f, 4.0f, &gameplayOptions->globalDamageMultiplier);
	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("Production speed"), this->_category, true, 0.5f, 2.0f, &gameplayOptions->productionSpeed);
	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("Research speed"), this->_category, true, 0.5f, 2.0f, &gameplayOptions->researchSpeed);
	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("Building speed"), this->_category, true, 0.5f, 2.0f, &gameplayOptions->buildingSpeed);
	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("Number of nests multiplier"), this->_category, true, 0.5f, 4.0f, &gameplayOptions->numNestsMult);
	this->_panel->setLineCheckbox(KEP::TranslationUtility::gettext_main("Bandits loot the player"), &gameplayOptions->banditsLootPlayer, this->_category);

	this->_panel->addSpace(this->_category, 1.0f);
}

bool KEP::tools::GamePlayOptionsTab::useLevelEditorCamera() const
{
	return _useLevelEditorCamera;
}

bool KEP::tools::GamePlayOptionsTab::enableNpcInventory() const
{
	return _enableNpcInventory;
}

bool KEP::tools::GamePlayOptionsTab::enableFastTravelMap() const
{
	return _enableFastTravelMap;
}

void KEP::tools::GamePlayOptionsTab::_saveSettings(DataPanelLine* line)
{
	_saveJson(this->_settingsPath);
}

void KEP::tools::GamePlayOptionsTab::_loadJson(const std::string& path)
{
	std::ifstream ifs(path);
	if (!ifs.is_open())
	{
		generateDefaultDocument();
		return;
	}

	rapidjson::IStreamWrapper isw(ifs);
	settingsDocument.ParseStream(isw);
	if (settingsDocument.HasParseError() || !settingsDocument.IsObject())
	{
		generateDefaultDocument();
		return;
	}

	if (settingsDocument.HasMember("use_level_editor_camera"))
		this->_useLevelEditorCamera = settingsDocument["use_level_editor_camera"].GetBool();
	if (settingsDocument.HasMember("fast_travel_mode"))
		this->_enableFastTravelMap = settingsDocument["fast_travel_mode"].GetBool();
	if (settingsDocument.HasMember("npc_inventory"))
		this->_enableNpcInventory = settingsDocument["npc_inventory"].GetBool();
}

void KEP::tools::GamePlayOptionsTab::_saveJson(const std::string& path) const
{
	settingsDocument["use_level_editor_camera"].SetBool(this->_useLevelEditorCamera);
	settingsDocument["fast_travel_mode"].SetBool(this->_enableFastTravelMap);
	settingsDocument["npc_inventory"].SetBool(this->_enableNpcInventory);

	std::ofstream ofs(path);
	if (!ofs.is_open())
	{
		DebugLog("save settings fialed");
		return;
	}

	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	settingsDocument.Accept(writer);
}
