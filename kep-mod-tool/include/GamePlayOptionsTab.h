#pragma once
#include <string>
#include <ogre/OgrePrerequisites.h>

class GameData;
class DatapanelGUI;
class DataPanelLine;
class DataPanelLine_TextEditable;
class DataPanelLine_DropBox;

namespace KEP
{
	namespace tools
	{
		class GamePlayOptionsTab
		{
		public:
			GamePlayOptionsTab();
			int getCategory() const { return _category; }
			void setPanel(DatapanelGUI* panel) { _panel = panel; };
			void refresh();
			bool useLevelEditorCamera() const;
			bool enableNpcInventory() const;
			bool enableFastTravelMap() const;

		private:
			void _saveSettings(DataPanelLine* line);

			void _loadJson(const std::string& path);
			void _saveJson(const std::string& path) const;

			DatapanelGUI* _panel;
			const int _category;
			std::string _settingsPath;
			bool _useLevelEditorCamera;
			bool _enableNpcInventory;
			bool _enableFastTravelMap;

		};
	}
}
