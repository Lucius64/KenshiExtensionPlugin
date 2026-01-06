#pragma once
#include <ogre/OgrePrerequisites.h>
#include <kenshi/TitleScreen.h>

namespace MyGUI
{
	class Window;
	class Widget;
}

class DatapanelGUI;
class GameData;
class InventoryGUI;
class Inventory;
class InventoryIcon;

class InventorySectionGUI
{
public:
	MyGUI::Widget* _widget;
	Ogre::vector<InventoryIcon*>::type _icons;
};

class InventoryLayout : public wraps::BaseLayout, public Ogre::GeneralAllocatedObject
{
public:
	DatapanelGUI* datapanel;
	GameData* data;
	MyGUI::Window* root;

	virtual ~InventoryLayout();
	virtual void createGUI(InventoryGUI*, Ogre::map<std::string, InventorySectionGUI*>::type*, Inventory*) = 0;
	virtual DatapanelGUI* getDatapanel();
};

class AnimalInventoryLayout : public InventoryLayout
{
public:
	virtual ~AnimalInventoryLayout();
	virtual void createGUI(InventoryGUI*, Ogre::map<std::string, InventorySectionGUI*>::type*, Inventory*) override;
};
