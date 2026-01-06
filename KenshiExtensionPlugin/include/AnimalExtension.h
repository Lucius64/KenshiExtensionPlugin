#pragma once
#include <ogre/OgrePrerequisites.h>

class Inventory;
class CharacterAnimal;
class AnimalInventoryLayout;
class InventoryGUI;
class InventorySectionGUI;

namespace KEP
{
	namespace AnimalExtension
	{
		bool CharacterAnimal_setupInventorySections_hook(CharacterAnimal* self, GameSaveState* state);
		AnimalInventoryLayout* AnimalInventoryLayout__CONSTRUCTOR_hook(AnimalInventoryLayout* self);
		void AnimalInventoryLayout_FUN_0014F530_hook(AnimalInventoryLayout* self, InventoryGUI* invGui, Ogre::map<std::string, InventorySectionGUI*>::type& sectionGuis, Inventory* inv);
		void init();
	}
}