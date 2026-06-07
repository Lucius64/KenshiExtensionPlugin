#pragma once
#include <string>
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreFastArray.h>

class GameData;
class DatapanelGUI;
class DataPanelLine;
class DataPanelLine_TextEditable;
class DataPanelLine_DropBox;

namespace KEP
{
	namespace tools
	{
		class PlatoonTool
		{
		public:
			PlatoonTool();
			int getCategory() const { return _category; }
			void setPanel(DatapanelGUI* panel) { _panel = panel; };
			void refresh();

		private:
			void _changeAI(DataPanelLine* line);
			void _resetAI(DataPanelLine* line);
			void _clearAllTag(DataPanelLine* line);
			void _triggerDialog(DataPanelLine* line);
			void _lockDialog(DataPanelLine* line);
			void _unlockDialog(DataPanelLine* line);
			void _clearDialogTimers(DataPanelLine* line);
			void _clearAllDialogLocksAndUnlocks(DataPanelLine* line);

			void _changeAIPackageSearchText(DataPanelLine* line);
			void _changeDialogSearchText(DataPanelLine* line);

			void _initAIPackageList();
			void _updateAIPackageList(const std::string& keyword);
			void _initDialogList();
			void _updateDialogList(const std::string& keyword);

			DatapanelGUI* _panel;
			const int _category;
			Ogre::FastArray<GameData*> _AIPackageList;
			Ogre::FastArray<GameData*> _dialogList;
			int _selectedAIPackage;
			int _selectedDialog;

		};
	}
}
