#pragma once
#include <string>
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreFastArray.h>

enum StatsEnumerated;
class GameData;
class DatapanelGUI;
class DataPanelLine;
class DataPanelLine_TextEditable;
class DataPanelLine_DropBox;

namespace KEP
{
	namespace tools
	{
		class StatsTool
		{
		public:
			StatsTool();
			int getCategory() const { return _category; }
			void setPanel(DatapanelGUI* panel) { _panel = panel; };
			void refresh();

		private:
			void _increaseStats(DataPanelLine* line);
			void _overwriteStats(DataPanelLine* line);
			void _setSlaveState(DataPanelLine* line);
			void _changePersonality(DataPanelLine* line);

			void _initStatsEnumList();
			void _initStatsDataList();
			void _updateStatsEnumList();
			void _updateStatsDataList();
			void _updateSlaveSateList();
			void _updatePersonalityList();

			DatapanelGUI* _panel;
			const int _category;
			Ogre::FastArray<StatsEnumerated> _statsEnumList;
			Ogre::FastArray<GameData*> _statsDataList;
			int _selectedStatsEnum;
			int _selectedStatCategory;
			float _amount;
			float _xp;
			int _selectedStatsData;
			int _selectedSlaveState;
			int _selectedPersonality;

		};
	}
}