#include "pch.h"

#include <boost/locale.hpp>

#include <kep/config_manager.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <kenshi/Globals.h>
#include <kenshi/InputHandler.h>
#include <kenshi/gui/ForgottenGUI.h>
#include <kenshi/gui/OptionsWindow.h>
#include <kenshi/gui/DatapanelGUI.h>
#include <kenshi/gui/DataPanelLine.h>

namespace
{
	void (*OptionsWindow_create_orig)(OptionsWindow*);
	void OptionsWindow_create_hook(OptionsWindow* self)
	{
		OptionsWindow_create_orig(self);

		auto& confMgr = KEP::ConfigManager::getSingleton();
		confMgr.create(self);
	}

	void (*OptionsWindow_saveOptions_orig)(OptionsWindow*);
	void OptionsWindow_saveOptions_hook(OptionsWindow* self)
	{
		OptionsWindow_saveOptions_orig(self);
		auto& confMgr = KEP::ConfigManager::getSingleton();
		confMgr.save(self);
	}

	void (*InputHandler_loadConfig_orig)(InputHandler*);
	void InputHandler_loadConfig_hook(InputHandler* self)
	{
		self->addCommand("toggle_devtools", 0, OIS::KC_F12, OIS::KC_UNASSIGNED, InputHandler::NONE_MASK, InputHandler::GLOBAL);
		InputHandler_loadConfig_orig(self);
	}
}

KEP::IPluginOption::~IPluginOption()
{
}

void KEP::ConfigManager::init(unsigned int platform, const std::string& version, uintptr_t baseAddr)
{
	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&OptionsWindow::create), OptionsWindow_create_hook, &OptionsWindow_create_orig))
		ErrorLog("[OptionsWindow::create] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&OptionsWindow::saveOptions), OptionsWindow_saveOptions_hook, &OptionsWindow_saveOptions_orig))
		ErrorLog("[OptionsWindow::saveOptions] Could not add hook!");

	//if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&InputHandler::loadConfig), &InputHandler_loadConfig_hook, &InputHandler_loadConfig_orig))
	//	ErrorLog("[InputHandler::loadConfig] could not install hook!");
}

KEP::ConfigManager& KEP::ConfigManager::getSingleton()
{
	static ConfigManager singleton;
	return singleton;
}

void KEP::ConfigManager::addItem(IPluginOption* option)
{
	this->options.push_back(option);
}

void KEP::ConfigManager::create(OptionsWindow* win)
{
	// 以下は使用済みのカテゴリ
	// Genral = 0x1
	// Gameplay = 0x22
	// Graphics = 0x17
	// Audio = 0x18
	// Controls = 0x19
	// Mods = 0x0

	auto tabCount = win->tabs->getChildCount();
	std::vector<int> catList(tabCount);
	int maxCat = 0;
	for (size_t i = 0; i < tabCount; i++)
	{
		auto panel = win->tabs->getChildAt(i)->getUserData<DatapanelGUI*>(false);
		if (panel != nullptr)
		{
			int cat = (*panel)->currentCategory;
			catList[i] = cat;
			if (maxCat < cat)
				maxCat = cat;
		}
	}

	size_t index = 5;
	if (tabCount < 5)
		index = tabCount;

	auto pluginOptionTab = win->tabs->insertItemAt(index, boost::locale::gettext("KEP")); // キーコンフィグの次にプラグイン設定を表示させる
	auto pluginOptionPanel = gui->createDatapanel("kep_options", pluginOptionTab, true);

	this->category = 0x30;
	auto result = std::find(catList.begin(), catList.end(), this->category);
	if (result != catList.end())
		this->category = maxCat + 1;

	pluginOptionPanel->changeCategory(this->category);
	pluginOptionPanel->setLineSpacing(25.0f);

	for (auto it = this->options.begin(); it != this->options.end(); ++it)
	{
		(*it)->create(pluginOptionPanel, this->category, win->tooltip);
	}

	pluginOptionTab->setVisible(false);
	win->tabs->setItemData(pluginOptionTab, pluginOptionPanel);
}

void KEP::ConfigManager::save(OptionsWindow* win) const
{
	auto endIt = this->options.end();
	for (auto it = this->options.begin(); it != endIt; ++it)
	{
		(*it)->save();
	}
}

KEP::ConfigManager::ConfigManager()
	: category(0)
{
}

KEP::ConfigManager::~ConfigManager()
{
}
