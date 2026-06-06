#include "pch.h"
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <kenshi/gui/DatapanelGUI.h>
#include <kenshi/gui/DataPanelLine.h>

#include <kep/settings.h>
#include <kep/utility.h>
#include <kep/translation.h>

KEP::CoreOption KEP::coreOptions;

namespace
{
	rapidjson::Document generateDocument(const KEP::CoreOption& settings)
	{
		rapidjson::Document settingsDocument;
		settingsDocument.SetObject();
		settingsDocument.AddMember("enable_kep", settings.getEnablePlugin(), settingsDocument.GetAllocator());
		settingsDocument.AddMember("enable_patch_system", settings.getEnablePatchSystem(), settingsDocument.GetAllocator());
		return settingsDocument;
	}
}

KEP::CoreOption::CoreOption()
	: _path("")
	, _isEnabledPlugin(true)
	, _patchSystem(false)
{
}

KEP::CoreOption::~CoreOption()
{
}

void KEP::CoreOption::init(const std::string& path)
{
	_path = path;
	loadSettings(_path);
	save();
}

void KEP::CoreOption::loadSettings(const std::string& path)
{
	std::ifstream ifs(path);
	if (!ifs.is_open())
		return;

	rapidjson::IStreamWrapper isw(ifs);
	rapidjson::Document settingsDocument;
	settingsDocument.ParseStream(isw);
	if (settingsDocument.HasParseError() || !settingsDocument.IsObject())
		return;

	if (settingsDocument.HasMember("enable_kep"))
		this->_isEnabledPlugin = settingsDocument["enable_kep"].GetBool();
	if (settingsDocument.HasMember("enable_patch_system"))
		this->_patchSystem = settingsDocument["enable_patch_system"].GetBool();
}

bool KEP::CoreOption::getEnablePlugin() const
{
	return _isEnabledPlugin;
}

bool KEP::CoreOption::getEnablePatchSystem() const
{
	return _patchSystem;
}

void KEP::CoreOption::save() const
{
	std::ofstream ofs(_path);
	if (!ofs.is_open())
	{
		DebugLog("save settings fialed");
		return;
	}

	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	auto settingsDocument = generateDocument(*this);
	settingsDocument.Accept(writer);
}

void KEP::CoreOption::create(DatapanelGUI* panel, int category, ToolTip* tooltip)
{
	panel->setLineCheckbox(KEP::TranslationUtility::gettext("Enable patch function"), &this->_patchSystem, category)
		->setToolTip(KEP::TranslationUtility::gettext("Load additional mod files from each mod's patch folder. Mod creators can provide features to localize their mods to match language settings and apply compatibility patches with other active mods. (REQURIES RESTART)"), tooltip);

	panel->addSpace(category, 0.25f);
}
