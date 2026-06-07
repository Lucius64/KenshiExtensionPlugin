#pragma once
#include <string>
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreFastArray.h>
#include <kenshi/util/hand.h>

class GameData;
class DatapanelGUI;
class DataPanelLine;
class DataPanelLine_TextEditable;
class DataPanelLine_DropBox;

namespace KEP
{
	namespace tools
	{
		class InformationPanel
		{
		public:
			static InformationPanel* getSingletonPtr();
			static bool initialized();
			InformationPanel();

			void create();
			void refresh();
			void show();
			void hide();
			bool isVisible();

		private:
			void _displayBuildingInformation();
			void _displayCharacterInformation();
			void _displayCombatInformation();
			void _displayUniqueNpcInformation();

			DatapanelGUI* _panel;
			hand _lastSelected;
		};
	}
}