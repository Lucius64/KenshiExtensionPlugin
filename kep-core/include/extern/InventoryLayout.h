#pragma once
#include <ogre/OgrePrerequisites.h>
#include <kenshi/gui/InventoryGUI.h>

class AnimalInventoryLayout : public InventoryLayout
{
public:
	virtual ~AnimalInventoryLayout();
	virtual void setupSections(InventoryGUI* _a1, std::map<std::string, InventorySectionGUI*, std::less<std::string >, Ogre::STLAllocator<std::pair<std::string const, InventorySectionGUI*>, Ogre::GeneralAllocPolicy > >& _a2, Inventory* _a3) override;// public vtable offset = 0x8 missing arg names
};
