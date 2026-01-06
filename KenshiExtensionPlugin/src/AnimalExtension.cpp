#include <boost/locale.hpp>

#include <mygui/MyGUI_TSize.h>
#include <mygui/MyGUI_TextBox.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/CharacterAnimal.h>
#include <kenshi/Inventory.h>

#include <extern/InventoryLayout.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <AnimalExtension.h>


namespace
{
	bool (*CharacterAnimal_setupInventorySections_orig)(CharacterAnimal*, GameSaveState*);
	AnimalInventoryLayout* (*AnimalInventoryLayout__CONSTRUCTOR_orig)(AnimalInventoryLayout*);
	void (*AnimalInventoryLayout_FUN_0014F530_orig)(AnimalInventoryLayout*, InventoryGUI*, Ogre::map<std::string, InventorySectionGUI*>::type&, Inventory*);
}

bool KEP::AnimalExtension::CharacterAnimal_setupInventorySections_hook(CharacterAnimal* self, GameSaveState* state)
{
	self->inventory->initialiseNewSection("armour", 4, 3, ATTACH_BODY, true, false, true, 1);
	CharacterAnimal_setupInventorySections_orig(self, state);
	return true;
}

AnimalInventoryLayout* KEP::AnimalExtension::AnimalInventoryLayout__CONSTRUCTOR_hook(AnimalInventoryLayout* self)
{
	AnimalInventoryLayout__CONSTRUCTOR_orig(self);

	auto root = self->mListWindowRoot[0]; // 範囲チェック不要
	if (root->getChildCount() != 0)
	{
		auto panel = root->getChildAt(0);

		MyGUI::TextBox* lbArmor;
		externalFunctions->FUN_0015D810(self, &lbArmor, "lbArmor", false, false);
		if (lbArmor == nullptr) // レイアウト変更Modとの競合によるクラッシュを回避するためにデフォルトのレイアウトを実装する
		{
			lbArmor = panel->createWidgetReal<MyGUI::TextBox>("Kenshi_TextboxPaintedText", MyGUI::FloatCoord(0.0194553f, 0.141463f, 0.234127f, 0.0390244f), MyGUI::Align::Default, self->mPrefix + "lbArmor");
			lbArmor->setTextAlign(MyGUI::Align::Center);
			lbArmor->setTextColour(MyGUI::Colour(0.327485f, 0.284279f, 0.23556f, 1.0f));
		}
		lbArmor->setCaption((boost::locale::translate("Armor").*externalFunctions->FUN_000A9580)());

		MyGUI::Widget* armourWidget;
		externalFunctions->FUN_0011DDC0(self, &armourWidget, "armour", false, false);
		if (armourWidget == nullptr)
			panel->createWidgetRealT(MyGUI::Widget::getClassTypeName(), "Kenshi_InventorySlotSkin", MyGUI::FloatCoord(0.0350195f, 0.195122f, 0.234127f, 0.139024f), MyGUI::Align::Default, self->mPrefix + "armour");
	}
	return self;
}

void KEP::AnimalExtension::AnimalInventoryLayout_FUN_0014F530_hook(AnimalInventoryLayout* self, InventoryGUI* invGui, Ogre::map<std::string, InventorySectionGUI*>::type& sectionGuis, Inventory* inv)
{
	auto section = inv->getSection("armour");
	auto sectionGui = externalFunctions->FUN_0014E390(self, section);
	auto widget = externalFunctions->FUN_0070C0A0(sectionGui);
	widget->setSize(MyGUI::IntSize(section->width * externalGlobals->_InventorySlotSkinSize->width, section->height * externalGlobals->_InventorySlotSkinSize->height));
	sectionGuis["armour"] = sectionGui;
	externalFunctions->FUN_0070C0B0(sectionGui, section->enabled);

	AnimalInventoryLayout_FUN_0014F530_orig(self, invGui, sectionGuis, inv);
}

void KEP::AnimalExtension::init()
{
	if (settings._animalArmor)
	{
		if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CharacterAnimal::_NV_setupInventorySections), &CharacterAnimal_setupInventorySections_hook, &CharacterAnimal_setupInventorySections_orig))
			ErrorLog("KenshiExtensionPlugin: [animal armor] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_00155350, &AnimalInventoryLayout__CONSTRUCTOR_hook, &AnimalInventoryLayout__CONSTRUCTOR_orig))
			ErrorLog("KenshiExtensionPlugin: [animal armor] could not install hook!");

		if (KenshiLib::SUCCESS != KenshiLib::AddHook(externalFunctions->FUN_0014F530, &AnimalInventoryLayout_FUN_0014F530_hook, &AnimalInventoryLayout_FUN_0014F530_orig))
			ErrorLog("KenshiExtensionPlugin: [animal armor] could not install hook!");
	}
}
