#pragma once
#include <vector>
#include "common.h"

class OptionsWindow;
class DatapanelGUI;
class ToolTip;

namespace KEP
{
	class DECLSPEC IPluginOption
	{
	public:
		virtual ~IPluginOption();
		virtual void save() const = 0;
		virtual void create(DatapanelGUI* panel, int category, ToolTip* tooltip) = 0;
	};

	class ConfigManager
	{
	public:
		static void init(unsigned int platform, const std::string& version, uintptr_t baseAddr);
		DECLSPEC static ConfigManager& getSingleton();
		DECLSPEC void addItem(IPluginOption* option);
		void create(OptionsWindow* win);
		void save(OptionsWindow* win) const;

	private:
		ConfigManager();
		~ConfigManager();

		std::vector<IPluginOption*> options;
		int category;
	};
}
