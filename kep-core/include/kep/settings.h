#pragma once
#include <kep/config_manager.h>

namespace KEP
{
	class DECLSPEC CoreOption : public IPluginOption
	{
	public:
		CoreOption();
		virtual ~CoreOption();
		virtual void save() const;
		virtual void create(DatapanelGUI* panel, int category, ToolTip* tooltip);

		void init(const std::string& path);
		void loadSettings(const std::string& path);

		bool getEnablePlugin() const;
		bool getEnablePatchSystem() const;

	private:
		std::string _path;
		bool _isEnabledPlugin;
		bool _patchSystem;
	};

	extern DECLSPEC CoreOption coreOptions;
}