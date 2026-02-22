#pragma once

#include <kenshi/TitleScreen.h>

class DatapanelGUI;
class NewGameOptionsWindow;

class NewGameWindow : public GUIWindow, public wraps::BaseLayout
{
public:
	int selected;
	DatapanelGUI* datapanel;
	MyGUI::TextBox* titleText;
	Ogre::FastArray<GameData*> newGameStartList;
	NewGameOptionsWindow* optionsWindow;
};
