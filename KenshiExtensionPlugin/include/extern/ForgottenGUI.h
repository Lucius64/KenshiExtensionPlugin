#pragma once
#include <ogre/OgrePrerequisites.h>
#include <ogre/OgreVector2.h>

class MainBarGUI;
class DatapanelGUI;
class DialogueWindow;
class TutorialGUI;
class TutorialpediaGUI;

namespace MyGUI
{
	class Gui;
}

class ForgottenGUI : public Ogre::GeneralAllocatedObject
{
public:
	char _0x8;
	MainBarGUI* _0x10;
	DatapanelGUI* _0x18;
	DialogueWindow* _0x20;
	TutorialGUI* _0x28;
	TutorialpediaGUI* _0x30;
	int _0x38;
	Ogre::Vector2 scaleFactor;
	MyGUI::Gui* _0x48;
	char _0x50[0x2e8];

	virtual ~ForgottenGUI();
};
