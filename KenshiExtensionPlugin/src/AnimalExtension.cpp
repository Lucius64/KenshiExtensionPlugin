#include <boost/locale.hpp>

#include <ogre/OgrePrerequisites.h>

#include <mygui/MyGUI_TSize.h>
#include <mygui/MyGUI_TextBox.h>
#include <mygui/MyGUI_Button.h>

#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <Debug.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/RootObjectFactory.h>
#include <kenshi/CharacterAnimal.h>
#include <kenshi/Inventory.h>
#include <kenshi/util/UtilityT.h>
#include <kenshi/FitnessSelector.h>

#include <extern/InventoryLayout.h>

#include <ExternalFunctions.h>
#include <Settings.h>
#include <AnimalExtension.h>


namespace
{
	bool (*CharacterAnimal_setupInventorySections_orig)(CharacterAnimal*, GameSaveState*);
	AnimalInventoryLayout* (*AnimalInventoryLayout__CONSTRUCTOR_orig)(AnimalInventoryLayout*);
	void (*AnimalInventoryLayout_FUN_0014F530_orig)(AnimalInventoryLayout*, InventoryGUI*, Ogre::map<std::string, InventorySectionGUI*>::type&, Inventory*);

	GameData* _chooseClothingItemFromListForAnimal(GameData* dataList, const std::string& listName, AttachSlot slot, RaceData* race)
	{
		FitnessSelector<int> selector;
		auto list = dataList->getReferenceList(listName);
		int count = 0;
		for (auto iter = list->begin(); iter != list->end(); ++iter)
		{
			auto baseData = iter->getPtr(&ou->gamedata);
			if (baseData->idata["slot"] == slot)
			{
				auto raceLimiter = RaceLimiter::getSingleton();
				raceLimiter->addLimit(baseData);
				if (raceLimiter->canEquip(baseData, race, true))
					selector.addItem(count, static_cast<float>(iter->values.value[1]));
			}
			++count;
		}

		if (selector.size() == 0)
			return nullptr;

		auto listIndex = selector.chooseAnItem();
		auto& reference = (*list)[listIndex];
		auto quantity = reference.values.value[0];
		if (quantity < 1)
			return nullptr;

		return reference.getPtr(&ou->gamedata);
	}

	int getLevel(int rarity)
	{
		switch (rarity) {
		case 0:
			return 5;
		case 1:
			return 40;
		case 3:
			return 60;
		case 4:
			return 80;
		case 5:
			return 95;
		}
		return 20;
	}
}

bool KEP::AnimalExtension::CharacterAnimal_setupInventorySections_hook(CharacterAnimal* self, GameSaveState* state)
{
	self->inventory->initialiseNewSection("armour", 5, 4, ATTACH_BODY, true, false, true, 1); // 先にセクションを作らないと装備が消える！
	CharacterAnimal_setupInventorySections_orig(self, state);

	if (state == nullptr) // 初回スポーンのみ処理する
	{
		auto armourGrade = self->data->idata["armour grade"];
		auto upgradeChance = self->data->idata["armour upgrade chance"];
		auto baseData = _chooseClothingItemFromListForAnimal(self->data, "clothing", ATTACH_BODY, self->getRace());
		if (baseData != nullptr)
		{
			if (armourGrade < 5 && UtilityT::randomInt(0, 100) < upgradeChance)
				++armourGrade;

			Item* item = ou->theFactory->createItem(baseData, hand(), nullptr, nullptr, getLevel(armourGrade), nullptr);
			if (item != nullptr)
				self->inventory->addItem(item, 1, false, true);
		}
	}

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
			MyGUI::TextBox* lbBackpack;
			externalFunctions->FUN_0015D810(self, &lbBackpack, "lbBackpack", true, true);
			MyGUI::Button* btOpenBag;
			externalFunctions->FUN_0011CD50(self, &btOpenBag, "OpenBagButton", true, true);
			lbArmor = panel->createWidgetReal<MyGUI::TextBox>("Kenshi_TextboxPaintedText", MyGUI::FloatCoord(0.0102881f, 0.126437f, 0.253086f, 0.045977f), MyGUI::Align::Default, self->mPrefix + "lbArmor");
			lbArmor->setCoord(lbArmor->getLeft(), btOpenBag->getBottom(), lbBackpack->getWidth(), lbBackpack->getHeight());
			lbArmor->setTextAlign(MyGUI::Align::Center);
			lbArmor->setTextColour(lbBackpack->getTextColour());
		}
		lbArmor->setCaption((boost::locale::translate("Armor").*externalFunctions->FUN_000A9580)());

		MyGUI::Widget* armourWidget;
		externalFunctions->FUN_0011DDC0(self, &armourWidget, "armour", false, false);
		if (armourWidget == nullptr)
		{
			armourWidget = panel->createWidgetRealT(MyGUI::Widget::getClassTypeName(), "Kenshi_InventorySlotSkin", MyGUI::FloatCoord(0.0102881f, 0.172414f, 0.253086f, 0.232184f), MyGUI::Align::Default, self->mPrefix + "armour");
			armourWidget->setPosition(lbArmor->getLeft(), lbArmor->getBottom());
		}
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


/*
* KenshiLibからGameDataReferenceの定義が欠落しているので暫定的に追加する
*/

GameData* GameDataReference::getPtr(GameDataContainer* source) const
{
	if (this->ptr == nullptr)
	{
		auto newPtr = source->getData(this->sid);
		const_cast<GameDataReference*>(this)->ptr = newPtr;
		return newPtr;
	}
	return this->ptr;
}
