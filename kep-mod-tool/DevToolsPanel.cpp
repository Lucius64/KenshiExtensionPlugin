#include "pch.h"
#include <algorithm>
#include <DevToolsPanel.h>
#include <SpawnTool.h>
#include <StatsTool.h>
#include <PlatoonTool.h>
#include <GamePlayOptionsTab.h>
#include <InformationPanel.h>
#include <utility.h>

#include <ois/OIS.h>
#include <ogre/OgreIdString.h>
#include <mygui/MyGUI.h>

#include <Debug.h>
#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/ZoneManager.h>
#include <kenshi/InputHandler.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/CameraClass.h>
#include <kenshi/Damages.h>
#include <kenshi/RaceData.h>
#include <kenshi/Faction.h>
#include <kenshi/Platoon.h>
#include <kenshi/Character.h>
#include <kenshi/CharStats.h>
#include <kenshi/CharMovement.h>
#include <kenshi/CharBody.h>
#include <kenshi/MedicalSystem.h>
#include <kenshi/Gear.h>
#include <kenshi/Inventory.h>
#include <kenshi/Building/Building.h>
#include <kenshi/Building/DoorStuff.h>
#include <kenshi/Building/UseableStuff.h>
#include <kenshi/Town.h>
#include <kenshi/AppearanceManager.h>
#include <kenshi/Appearance.h>
#include <kenshi/Dialogue.h>
#include <kenshi/gui/ForgottenGUI.h>
#include <kenshi/gui/DatapanelGUI.h>
#include <kenshi/gui/DataPanelLine.h>
#include <kenshi/gui/TitleScreen.h>
#include <kenshi/gui/MainBarGUI.h>
#include <kenshi/gui/ManagementScreen.h>
#include <kenshi/gui/MapScreen.h>
#include <kenshi/gui/OptionsWindow.h>
#include <extern/ZoneMapContent.h>
#include <extern/TownBase.h>
#include <extern/UseableStuff.h>
#include <extern/AI.h>
#include <extern/AreaBiomeGroup.h>

#include <kep/translation.h>
#include <kep/functions.h>
#include <kep/utility.h>

namespace
{
	bool initialized = false;
}

KEP::tools::DevToolsPanel* KEP::tools::DevToolsPanel::getSingletonPtr()
{
	static boost::scoped_ptr<DevToolsPanel> singleton(new DevToolsPanel());
	return singleton.get();
}

KEP::tools::DevToolsPanel::DevToolsPanel()
	: _panel(nullptr)
	, _category(0)
	, _spawnTool(nullptr)
	, _statsTool(nullptr)
	, _platoonTool(nullptr)
	, _gamePlayOptionsTab(nullptr)
	, _cats(10000.0f)
	, _squishParts(0)
	, _damageCut(120.0f)
	, _damageBlunt(0.0f)
	, _damagePierce(0.0f)
	, _damageBleedMult(1.0f)
	, _damageArmourPenetration(0.0f)
	, _dismembermentLimb(0)
{
	_spawnTool = new SpawnTool();
	_statsTool = new StatsTool();
	_platoonTool = new PlatoonTool();
	_gamePlayOptionsTab = new GamePlayOptionsTab();

	create();

	initialized = true;
}

KEP::tools::DevToolsPanel::~DevToolsPanel()
{
	if (_spawnTool != nullptr)
		delete _spawnTool;
	if (_statsTool != nullptr)
		delete _statsTool;
	if (_platoonTool != nullptr)
		delete _platoonTool;
	if (_gamePlayOptionsTab != nullptr)
		delete _gamePlayOptionsTab;
}

void KEP::tools::DevToolsPanel::create()
{
	if (this->_panel != nullptr)
	{
		this->_panel->show(false);
		gui->destroy(this->_panel);
	}

	this->_panel = gui->createDatapanel(0.05f, 0.7f, 0.28f, 0.58f, true, "Window", true);
	this->_panel->setCaption(KEP::TranslationUtility::gettext("Developer tools"));
	this->_panel->setPanelName("Development");

	refresh();

	if (this->_spawnTool != nullptr)
	{
		this->_spawnTool->setPanel(this->_panel);
		this->_spawnTool->refresh();
	}

	if (this->_statsTool != nullptr)
	{
		this->_statsTool->setPanel(this->_panel);
		this->_statsTool->refresh();
	}

	if (this->_platoonTool != nullptr)
	{
		this->_platoonTool->setPanel(this->_panel);
		this->_platoonTool->refresh();
	}

	if (this->_gamePlayOptionsTab != nullptr)
	{
		this->_gamePlayOptionsTab->setPanel(this->_panel);
		this->_gamePlayOptionsTab->refresh();
	}

	this->_panel->showTabs(true);
	this->_panel->changeCategory(this->_category);
	this->_panel->addTab(this->_category, KEP::TranslationUtility::gettext("Main"), "");
	this->_panel->addTab(this->_spawnTool->getCategory(), KEP::TranslationUtility::gettext("Spawn"), "");
	this->_panel->addTab(this->_statsTool->getCategory(), KEP::TranslationUtility::gettext("Stats"), "");
	this->_panel->addTab(this->_platoonTool->getCategory(), KEP::TranslationUtility::gettext("Task"), "");
	this->_panel->addTab(this->_gamePlayOptionsTab->getCategory(), KEP::TranslationUtility::gettext_main("Options"), "");
	this->_panel->show(false);
}

void KEP::tools::DevToolsPanel::refresh()
{
	this->_panel->setLineSpacing(32.0f);

	this->_panel->clearPage(this->_category);

	auto slider = this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("Money"), this->_category, true, 0.0f, 100000.0f, &this->_cats);
	this->_panel->addSpace(this->_category, 0.25f);

	slider->setPrecision(0);
	setButton(KEP::TranslationUtility::gettext("Give money"), this->_category, 0.7f, &DevToolsPanel::_addMoney);
	setButton(KEP::TranslationUtility::gettext("Take money"), this->_category, 0.7f, &DevToolsPanel::_takeMoney);

	setButton(KEP::TranslationUtility::gettext("Complete Reseach"), this->_category, 0.7f, &DevToolsPanel::_completeResearch);
	setButton(KEP::TranslationUtility::gettext("Complete All Reseach"), this->_category, 0.7f, &DevToolsPanel::_completeAllResearch);

	setButton(KEP::TranslationUtility::gettext("Complete Building"), this->_category, 0.7f, &DevToolsPanel::_completeBuilding);
	setButton(KEP::TranslationUtility::gettext("Complete All Building"), this->_category, 0.7f, &DevToolsPanel::_completeAllBuilding);

	setButton(KEP::TranslationUtility::gettext("Discover all towns"), this->_category, 0.7f, &DevToolsPanel::_discoverAllTowns);

	setButton(KEP::TranslationUtility::gettext("Teleport"), this->_category, 0.7f, &DevToolsPanel::_teleportTheSelected);
	setButton(KEP::TranslationUtility::gettext("Unlock cage"), this->_category, 0.7f, &DevToolsPanel::_unlockCage);
	setButton(KEP::TranslationUtility::gettext("Free selected"), this->_category, 0.7f, &DevToolsPanel::_freeTheSelected);

	setButton(KEP::TranslationUtility::gettext("Destroy"), this->_category, 0.7f, &DevToolsPanel::_destroyTheSelected);

	this->_panel->addSpace(this->_category, 1.0f);
	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext("[Health]"), "", this->_category, false, true);

	setButton(KEP::TranslationUtility::gettext("Heal selected"), this->_category, 0.7f, &DevToolsPanel::_healTheSelected);
	this->_panel->addSpace(this->_category, 0.25f);

	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext("Damage"), "", this->_category, false, true);

	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("Cutting damage"), this->_category, true, 0.0f, 200.0f, &this->_damageCut);
	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("Blunt damage"), this->_category, true, 0.0f, 200.0f, &this->_damageBlunt);
	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext("Harpoon damage"), this->_category, true, 0.0f, 200.0f, &this->_damagePierce);
	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("Blood loss"), this->_category, true, 0.0f, 10.0f, &this->_damageBleedMult);
	this->_panel->setLineSliderEditable(KEP::TranslationUtility::gettext_main("Armour penetration"), this->_category, true, 0.0f, 1.0f, &this->_damageArmourPenetration);
	this->_panel->addSpace(this->_category, 0.25f);

	setButton(KEP::TranslationUtility::gettext("Squish"), this->_category, 0.7f, &DevToolsPanel::_squishTheSelected);

	this->_panel->addSpace(this->_category, 1.0f);
	auto dropbox = this->_panel->setLineDropBox(KEP::TranslationUtility::gettext_main("Limbs"), this->_category, &this->_dismembermentLimb, false, 0.7f);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Left Arm"), 0);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Right Arm"), 1);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Left Leg"), 2);
	dropbox->addAValue(KEP::TranslationUtility::gettext("Right Leg"), 3);
	dropbox->setSelectedValue(0);

	setButton(KEP::TranslationUtility::gettext("Amputate"), this->_category, 0.7f, &DevToolsPanel::_amputate);
	setButton(KEP::TranslationUtility::gettext("Join limb"), this->_category, 0.7f, &DevToolsPanel::_joinLimb);

	this->_panel->addSpace(this->_category, 1.0f);
	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext("[Misc]"), "", this->_category, false, true);
	setButton(KEP::TranslationUtility::gettext("Show character editor"), this->_category, 0.7f, &DevToolsPanel::_editTheSelected);

	this->_panel->addSpace(this->_category, 0.25f);

	auto checkBox = this->_panel->setLineCheckbox(KEP::TranslationUtility::gettext("Show roads on map"), nullptr, this->_category);
	checkBox->getCheckBox()->setStateSelected(false);
	checkBox->callback = new MyGUI::delegates::CMethodDelegate1<DevToolsPanel, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &DevToolsPanel::_showRoadsOnMap);

	this->_panel->addSpace(this->_category, 0.25f);

	auto button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Information Panel"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<DevToolsPanel, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &DevToolsPanel::_toggleInformationPanel);

	this->_panel->addSpace(this->_category, 1.0f);
}

void KEP::tools::DevToolsPanel::show()
{
	this->_panel->show(true);
}

void KEP::tools::DevToolsPanel::hide()
{
	this->_panel->show(false);
}

bool KEP::tools::DevToolsPanel::isVisible()
{
	return this->_panel->isVisible();
}

void KEP::tools::DevToolsPanel::_addMoney(DataPanelLine* line)
{
	int money = static_cast<int>(this->_cats);
	auto character = gui->selectedObject.getCharacter();
	if (character != nullptr)
		character->getOwnerships()->addMoney(money);
	else
		ou->player->participant->factionOwnerships->addMoney(money);
}

void KEP::tools::DevToolsPanel::_takeMoney(DataPanelLine* line)
{
	int money = static_cast<int>(this->_cats);
	auto character = gui->selectedObject.getCharacter();
	if (character != nullptr)
		character->getOwnerships()->takeMoney(money);
	else
		ou->player->participant->factionOwnerships->takeMoney(money);
}

namespace
{
	void teleportAndStopCharactersMovement(Character* character, const Ogre::Vector3& pos)
	{
		character->getMovement()->halt();
		KEP::functions->OrdersReceiver_deleteAllTask(character->ai->taskSystem);
		character->getBody()->_endAction();
		character->teleport(pos);
	}
}

void KEP::tools::DevToolsPanel::_teleportTheSelected(DataPanelLine* line)
{
	auto pos = ou->getCameraCenter();
	auto character = gui->selectedObject.getCharacter();
	if (character != nullptr && !character->isPlayerCharacter())
	{
		teleportAndStopCharactersMovement(character, pos);
	}
	else
	{
		for (auto iter = ou->player->selectedCharacters.begin(); iter != ou->player->selectedCharacters.end(); ++iter)
		{
			auto character = iter->getCharacter();
			if (character != nullptr)
				teleportAndStopCharactersMovement(character, pos);
		}
	}
}

void KEP::tools::DevToolsPanel::_completeResearch(DataPanelLine* line)
{
	KEP::functions->Research_addResearchProgress(ou->player->technology, 1000000.0f);
}

void KEP::tools::DevToolsPanel::_completeAllResearch(DataPanelLine* line)
{
	lektor<GameData*> list;
	ou->gamedata.getDataOfType(list, RESEARCH);
	for (auto iter = list.begin(); iter != list.end(); ++iter)
	{
		if (!functions->Research_completed(ou->player->technology, *iter))
		{
			functions->Research_complete(ou->player->technology, *iter);
		}
	}
	KEP::functions->MessageRoller_clear(ou->messageRoller);
}

namespace
{
	void completeConstruction(Building* building)
	{
		auto buildState = building->getBuildState();
		if (!buildState->isComplete)
		{
			for (auto iter = buildState->mats.begin(); iter != buildState->mats.end(); ++iter)
			{
				(*iter)->amountOfMaterials = (*iter)->buildMatsTotal;
			}
			building->addConstructionProgress(10000000.f);
			buildState->constructionProgress = buildState->totalMats;
			buildState->isPaused = false;
		}
	}
}

void KEP::tools::DevToolsPanel::_completeBuilding(DataPanelLine* line)
{
	auto building = gui->selectedObject.getBuilding();
	if (building == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a building."), true);
		return;
	}

	if (building->imADoor)
		building = building->doorStuff()->doorParentBuilding();

	if (building == nullptr)
		return;

	completeConstruction(building);
}

void KEP::tools::DevToolsPanel::_completeAllBuilding(DataPanelLine* line)
{
	auto mapContent = ou->zoneMgr->getCurrentZoneMap()->mapContent;
	if (mapContent != nullptr)
	{
		for (auto iter = mapContent->things.begin(); iter != mapContent->things.end(); ++iter)
		{
			if ((*iter)->getDataType() == BUILDING)
				completeConstruction(reinterpret_cast<Building*>(*iter));
		}
	}
	KEP::functions->MessageRoller_clear(ou->messageRoller);
}

void KEP::tools::DevToolsPanel::_discoverAllTowns(DataPanelLine* line)
{
	auto towns = KEP::functions->getLevelManager()->townList;
	for (auto iter = towns->things.begin(); iter != towns->things.end(); ++iter)
	{
		auto town = reinterpret_cast<TownBase*>(*iter);
		town->setDiscovered(true);
		town->setExplored(true);
	}
	for (auto iter = towns->nests.begin(); iter != towns->nests.end(); ++iter)
	{
		auto town = reinterpret_cast<TownBase*>(*iter);
		town->setDiscovered(true);
	}
	ManagementScreen::getSingleton()->refreshMap(false);
}

void KEP::tools::DevToolsPanel::_unlockCage(DataPanelLine* line)
{
	Building* building = nullptr;
	auto character = gui->selectedObject.getCharacter();
	if (character != nullptr && character->inSomething == IN_PRISON)
		building = character->inWhat.getBuilding();

	if (building == nullptr)
		building = gui->selectedObject.getBuilding();

	if (building != nullptr)
	{
		auto lock = building->getDoorLock();
		if (lock != nullptr && character != nullptr)
		{
			lock->locked = false;
			auto shackle = character->getChainedModeShackles();
			if (shackle != nullptr)
				shackle->lock->locked = false;
			auto target = ou->player->getNearestCharacterTo(character->getPosition());
			character->dialogue->sendEvent(target, EV_UNLOCK_MY_CAGE_OR_SHACKLES);
		}
	}
}

void KEP::tools::DevToolsPanel::_freeTheSelected(DataPanelLine* line)
{
	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character."), true);
		return;
	}

	auto inv = character->getInventory();
	auto section = inv->getSectionOfType(ATTACH_BOOTS);
	if (!section->isEmpty())
	{
		auto item = section->getItem();
		if (item != nullptr)
		{
			auto shackle = item->isLockedArmour();
			if (shackle != nullptr)
				shackle->lock->locked = false;

		}
	}
	section = inv->getSectionOfType(ATTACH_BODY);
	if (!section->isEmpty())
	{
		auto item = section->getItem();
		if (item != nullptr)
		{
			auto shackle = item->isLockedArmour();
			if (shackle != nullptr)
				shackle->lock->locked = false;
		}
	}
}

namespace
{
	void heal(Character* character)
	{
		for (auto iter = character->medical.status.begin(); iter != character->medical.status.end(); ++iter)
		{
			iter->second.hitChanceMult = 1.0f;
			iter->second.wearDamage = 0.0f;
			iter->second.juryRigging = 0.0f;
		}
		character->healCompletely();
	}
}

void KEP::tools::DevToolsPanel::_healTheSelected(DataPanelLine* line)
{
	auto character = gui->selectedObject.getCharacter();
	if (character != nullptr && !character->isPlayerCharacter())
	{
		heal(character);
	}
	else
	{
		for (auto handIt = ou->player->selectedCharacters.begin(); handIt != ou->player->selectedCharacters.end(); ++handIt)
		{
			auto character = handIt->getCharacter();
			if (character != nullptr)
				heal(character);
		}
	}
}

void KEP::tools::DevToolsPanel::_squishTheSelected(DataPanelLine* line)
{
	auto character = gui->selectedObject.getCharacter();
	if (character != nullptr)
	{
		Damages dmg(this->_damageCut, this->_damageBlunt, this->_damagePierce, this->_damageBleedMult, this->_damageArmourPenetration);
		character->hitByMeleeAttack(CUT_DOWNWARD, dmg, character, nullptr, 1);
	}
}

void KEP::tools::DevToolsPanel::_amputate(DataPanelLine* line)
{
	auto character = gui->selectedObject.getCharacter();
	if (character != nullptr)
	{
		auto limb = static_cast<RobotLimbs::Limb>(this->_dismembermentLimb);
		if (character->medical.robotLimbs != nullptr)
		{
			auto state = character->medical.robotLimbs->getState(limb);
			if (state != LIMB_ORIGINAL)
				return;
		}
		character->medical.amputate(limb, false, Ogre::Vector3::ZERO);

		MedicalSystem::HealthPartStatus* parts = nullptr;
		if (limb == RobotLimbs::LEFT_ARM)
			parts = character->medical.leftArm;
		else if (limb == RobotLimbs::RIGHT_ARM)
			parts = character->medical.rightArm;
		else if (limb == RobotLimbs::LEFT_LEG)
			parts = character->medical.leftLeg;
		else if (limb == RobotLimbs::RIGHT_LEG)
			parts = character->medical.rightLeg;
		if (parts != nullptr)
		{
			parts->hitChanceMult = 1.0f;
			parts->wearDamage = 0.0f;
			parts->flesh = parts->maxHealth();
			parts->fleshStun = 0.0f;
			parts->bandaging = 0.0f;
			parts->juryRigging = 0.0f;
		}
	}
}

void KEP::tools::DevToolsPanel::_joinLimb(DataPanelLine* line)
{
	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character."), true);
		return;
	}

	if (character->medical.robotLimbs == nullptr)
		return;

	auto limb = static_cast<RobotLimbs::Limb>(this->_dismembermentLimb);
	auto state = character->medical.robotLimbs->getState(limb);
	if (state != LIMB_REPLACED && state != LIMB_STUMP)
		return;

	if (state == LIMB_REPLACED)
	{
		auto item = character->medical.robotLimbs->getLimbItem(limb);
		character->medical.robotLimbs->getInventoryInterface(true)->getInventory()->dropItem(item);
	}
	character->medical.robotLimbs->setLimb(limb, LIMB_ORIGINAL, nullptr);

	MedicalSystem::HealthPartStatus* parts = nullptr;
	if (limb == RobotLimbs::LEFT_ARM)
		parts = character->medical.leftArm;
	else if (limb == RobotLimbs::RIGHT_ARM)
		parts = character->medical.rightArm;
	else if (limb == RobotLimbs::LEFT_LEG)
		parts = character->medical.leftLeg;
	else if (limb == RobotLimbs::RIGHT_LEG)
		parts = character->medical.rightLeg;
	if (parts != nullptr)
	{
		auto anatomyList = character->getRace()->data->getReferenceListIfExists("combat anatomy");
		if (anatomyList != nullptr)
		{
			for (auto iter = anatomyList->begin(); iter != anatomyList->end(); ++iter)
			{
				if (iter->getPtr(&ou->gamedata) == parts->data)
				{
					parts->_maxHealth = static_cast<float>(iter->values.value[1]);
					break;
				}
			}
		}
		parts->hitChanceMult = 1.0f;
		parts->wearDamage = 0.0f;
		parts->flesh = parts->maxHealth();
		parts->fleshStun = 0.0f;
		parts->bandaging = 0.0f;
		parts->juryRigging = 0.0f;
	}
}

void KEP::tools::DevToolsPanel::_destroyTheSelected(DataPanelLine* line)
{
	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character."), true);
		return;
	}

	ou->destroy(character, false, "devtools panel delete");
}

void KEP::tools::DevToolsPanel::_editTheSelected(DataPanelLine* line)
{
	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character."), true);
		return;
	}

	if (!character->isPlayerCharacter())
	{
		gui->messageBox(KEP::TranslationUtility::gettext("Show character editor"), KEP::TranslationUtility::gettext("Warning: Editing NPCs may cause the game to crash. Do you want to continue?"), 6, true, new MyGUI::delegates::CMethodDelegate1<DevToolsPanel, int>(MyGUI::delegates::GetDelegateUnlink(this), this, &DevToolsPanel::_confirmEditTheSelected));
		return;
	}

	_confirmEditTheSelected(2);
}

void KEP::tools::DevToolsPanel::_confirmEditTheSelected(int c)
{
	if (c == 2)
	{
		auto character = gui->selectedObject.getCharacter();
		if (character == nullptr)
			return;

		lektor<Character*> chars;
		chars.push_back(character);

		ou->userPause(false);
		gui->showCharacterEditor(chars, EDIT_DEBUG, nullptr);
	}
}

void KEP::tools::DevToolsPanel::_showRoadsOnMap(DataPanelLine* line)
{
	ManagementScreen::getSingleton()->showRoads(reinterpret_cast<DataPanelLine_CheckBox*>(line)->getCheckBox()->getStateSelected());
}

void KEP::tools::DevToolsPanel::_toggleInformationPanel(DataPanelLine* line)
{
	auto infoPanel = KEP::tools::InformationPanel::getSingletonPtr();
	if (!infoPanel->isVisible())
		infoPanel->show();
	else
		infoPanel->hide();
}

void KEP::tools::DevToolsPanel::setButton(const std::string& caption, int cat, float width, void (DevToolsPanel::* callback)(DataPanelLine*))
{
	auto button = this->_panel->setLineTextButton("", caption, cat, width, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<DevToolsPanel, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, callback);
}

void KEP::tools::DevToolsPanel::setDropBox(const std::string& caption, int cat, float width, int* valPtr, void (DevToolsPanel::* callback)(DataPanelLine*))
{
	auto dropbox = this->_panel->setLineDropBox(caption, cat, valPtr, true, width);
	dropbox->callback = new MyGUI::delegates::CMethodDelegate1<DevToolsPanel, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, callback);
}

void KEP::tools::DevToolsPanel::setEditBox(const std::string& caption, int cat, float width, const std::string& text, void (DevToolsPanel::* callback)(DataPanelLine*))
{
	auto textbox = this->_panel->setLineTextEditable(caption, text, cat, true, false, MyGUI::Align::Left, width);
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<DevToolsPanel, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, callback);
}

class Renderer;
class Terrain;

class KingOfRenderThread : private Ogre::GeneralAllocatedObject
{
public:
	float TIME;
	Renderer* renderer;
	CameraClass* camera;
	Terrain* terrain;
	TitleScreen* titleScreen;
	ogre_unordered_map<Ogre::IdString, MeshDataLookup*>::type globalMeshDataLookup;
};

namespace
{
	void (*GameWorld_processKeys_orig)(GameWorld*);
	void GameWorld_processKeys_hook(GameWorld* self)
	{
		GameWorld_processKeys_orig(self);
		if (!gui->isLoadingMessageVisible() && !gui->isPaused())
		{
			for (auto iter = key->events.begin(); iter != key->events.end(); ++iter)
			{
				if ((*iter)->name == "toggle_devtools")
				{
					auto toolsPanel = KEP::tools::DevToolsPanel::getSingletonPtr();
					if (!toolsPanel->isVisible())
						toolsPanel->show();
					else
						toolsPanel->hide();
				}
				else if ((*iter)->name == "toggle_info_panel")
				{
					auto infoPanel = KEP::tools::InformationPanel::getSingletonPtr();
					if (!infoPanel->isVisible())
						infoPanel->show();
					else
						infoPanel->hide();
				}
			}
			KEP::tools::InformationPanel::getSingletonPtr()->refresh();
		}
	}

	bool (*EscMenu_openedOtherWindows_orig)(void*);
	bool EscMenu_openedOtherWindows_hook(void* self)
	{
		auto out = EscMenu_openedOtherWindows_orig(self);
		if (out)
		{
			KEP::tools::DevToolsPanel::getSingletonPtr()->hide();
			KEP::tools::InformationPanel::getSingletonPtr()->hide();
		}
		else
		{
			auto toolsPanel = KEP::tools::DevToolsPanel::getSingletonPtr();
			if (toolsPanel->isVisible())
			{
				toolsPanel->hide();
				out = true;
			}
			auto infoPanel = KEP::tools::InformationPanel::getSingletonPtr();
			if (infoPanel->isVisible())
			{
				infoPanel->hide();
				out = true;
			}
		}
		return out;
	}

	void (*CameraClass_restrictPosition_orig)(CameraClass*, lektor<Character*>&);
	void CameraClass_restrictPosition_hook(CameraClass* self, lektor<Character*>& objects)
	{
		if (!KEP::tools::DevToolsPanel::getSingletonPtr()->getGamePlayOptionsTab()->useLevelEditorCamera())
			CameraClass_restrictPosition_orig(self, objects);
	}

	void (*MainBarGUI_toggleInventory_orig)(MainBarGUI*, MyGUI::Widget*);
	void MainBarGUI_toggleInventory_hook(MainBarGUI* self, MyGUI::Widget* _sender)
	{
		if (KEP::tools::DevToolsPanel::getSingletonPtr()->getGamePlayOptionsTab()->enableNpcInventory())
		{
			auto character = gui->selectedObject.getCharacter();
			if (character != nullptr && !character->isPlayerCharacter())
			{
				if (gui->hasInventoryWindowOpen(gui->selectedObject))
				{
					gui->closeInventory(gui->selectedObject);
					return;
				}
				auto layout = character->createInventoryLayout();
				if (layout != nullptr)
				{
					gui->createInventoryWindow(gui->selectedObject, layout);
					gui->inventoryWindowNPC = gui->selectedObject;
				}
				return;
			}
		}
		MainBarGUI_toggleInventory_orig(self, _sender);
	}

	void (*Building_select_orig)(Building*);
	void Building_select_hook(Building* self)
	{
		Building_select_orig(self);
		if (KEP::tools::DevToolsPanel::getSingletonPtr()->getGamePlayOptionsTab()->enableNpcInventory())
			if (self->getBuildState()->isComplete && self->getFaction()->isPlayer == nullptr)
				gui->showInventoryBuilding(self->handle);
	}

	void (*MainBarGUI_autoChangeSelectedObject_orig)(MainBarGUI*, const hand&);
	void MainBarGUI_autoChangeSelectedObject_hook(MainBarGUI* self, const hand& obj)
	{
		MainBarGUI_autoChangeSelectedObject_orig(self, obj);
		if (KEP::tools::DevToolsPanel::getSingletonPtr()->getGamePlayOptionsTab()->enableNpcInventory())
		{
			auto building = obj.getBuilding();
			if (building != nullptr)
				if (building->getBuildState()->isComplete && building->getFaction()->isPlayer == nullptr)
					gui->showInventoryBuilding(obj);
		}
	}

	void (*MapScreen_mapMousePressed_orig)(MapScreen*, MyGUI::Widget*, int, int, MyGUI::MouseButton);
	void MapScreen_mapMousePressed_hook(MapScreen* self, MyGUI::Widget* _sender, int _left, int _top, MyGUI::MouseButton _id)
	{
		if (KEP::tools::DevToolsPanel::getSingletonPtr()->getGamePlayOptionsTab()->enableFastTravelMap() && _id.getValue() == MyGUI::MouseButton::Left && key->shift)
			return;
		MapScreen_mapMousePressed_orig(self, _sender, _left, _top, _id);
	}

	void (*MapScreen_mapMouseReleased_orig)(MapScreen*, MyGUI::Widget*, int, int, MyGUI::MouseButton);
	void MapScreen_mapMouseReleased_hook(MapScreen* self, MyGUI::Widget* _sender, int _left, int _top, MyGUI::MouseButton _id)
	{
		if (KEP::tools::DevToolsPanel::getSingletonPtr()->getGamePlayOptionsTab()->enableFastTravelMap() && _id.getValue() == MyGUI::MouseButton::Left && key->shift)
		{
			Ogre::Vector3 pos;
			if (self->getMouseWorldPosition(pos))
			{
				pos.y = UtilityT::getTerrainWithWaterHeight(pos.x, pos.z);
				ou->player->camera->teleport(pos);
			}

			return;
		}
		MapScreen_mapMouseReleased_orig(self, _sender, _left, _top, _id);
	}

	void (*ForgottenGUI_changeFontSize_orig)();
	void ForgottenGUI_changeFontSize_hook()
	{
		ForgottenGUI_changeFontSize_orig();
		if (initialized)
			KEP::tools::DevToolsPanel::getSingletonPtr()->create();
		if (KEP::tools::InformationPanel::initialized())
			KEP::tools::InformationPanel::getSingletonPtr()->create();

	}

	void (*OptionsWindow_create_orig)(OptionsWindow*);
	void OptionsWindow_create_hook(OptionsWindow* self)
	{
		OptionsWindow_create_orig(self);

		DatapanelGUI* controlsTab = nullptr;

		auto tabCount = self->tabs->getItemCount();
		for (size_t i = 0; i < tabCount; i++)
		{
			auto panel = self->tabs->getItemDataAt<DatapanelGUI*>(i, false);
			if (*panel != nullptr && (*panel)->currentCategory == 0x19)
			{
				controlsTab = *panel;
				break;
			}
		}

		if (controlsTab != nullptr)
		{
			controlsTab->addCustomLine(new DataPanelLine_KeyConfig("toggle_devtools", KEP::TranslationUtility::gettext("KEP: Developer tools"), 0x19));
			controlsTab->addCustomLine(new DataPanelLine_KeyConfig("toggle_info_panel", KEP::TranslationUtility::gettext("KEP: Information Panel"), 0x19));
			controlsTab->addSpace(0x19, 1.0f);
		}
	}

	void (*AppearanceManager_getEditorData_orig)(AppearanceManager*, ogre_unordered_map<RaceGroupData*, Ogre::FastArray<GameData*>>::type&, ogre_unordered_map<GameData*, ogre_unordered_map<AppearanceManager::Gender::Enum, AppearanceManager::AppearanceData>::type>::type&, bool, const Ogre::vector<GameDataReference>::type*);
	void AppearanceManager_getEditorData_hook(AppearanceManager* self, ogre_unordered_map<RaceGroupData*, Ogre::FastArray<GameData*>>::type& _racesGroups, ogre_unordered_map<GameData*, ogre_unordered_map<AppearanceManager::Gender::Enum, AppearanceManager::AppearanceData>::type>::type& _raceAppearanceData, bool playableOnly, const Ogre::vector<GameDataReference>::type* filter)
	{
		AppearanceManager_getEditorData_orig(self, _racesGroups, _raceAppearanceData, false, filter);
	}
}

void KEP::tools::initHook()
{
	void* AppearanceManager_getEditorData = nullptr;

	auto baseAddr = reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr));

	auto versionInfo = KenshiLib::GetKenshiVersion();
	auto platform = versionInfo.GetPlatform();
	auto version = versionInfo.GetVersion();
	if (platform == KenshiLib::BinaryVersion::UNKNOWN || (version != "1.0.65"))
	{
		ErrorLog("error!");
		return;
	}

	if (platform == KenshiLib::BinaryVersion::STEAM)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&AppearanceManager_getEditorData = baseAddr + 0x79b70;
		}
	}
	else if (platform == KenshiLib::BinaryVersion::GOG)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&AppearanceManager_getEditorData = baseAddr + 0x79b70;
		}
	}

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&GameWorld::processKeys), &GameWorld_processKeys_hook, &GameWorld_processKeys_orig))
		ErrorLog("[GameWorld::processKeys] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KEP::functions->EscMenu_openedOtherWindows, &EscMenu_openedOtherWindows_hook, &EscMenu_openedOtherWindows_orig))
		ErrorLog("[EscMenu::openedOtherWindows] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&CameraClass::restrictPosition), (void(*)(CameraClass*, lektor<Character*>&))(&CameraClass_restrictPosition_hook), &CameraClass_restrictPosition_orig))
		ErrorLog("[CameraClass::restrictPosition] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&MainBarGUI::toggleInventory), &MainBarGUI_toggleInventory_hook, &MainBarGUI_toggleInventory_orig))
		ErrorLog("[MainBarGUI::toggleInventory] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&MapScreen::mapMousePressed), &MapScreen_mapMousePressed_hook, &MapScreen_mapMousePressed_orig))
		ErrorLog("[MapScreen::mapMousePressed] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&MapScreen::mapMouseReleased), &MapScreen_mapMouseReleased_hook, &MapScreen_mapMouseReleased_orig))
		ErrorLog("[MapScreen::mapMouseReleased] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&Building::_NV_select), &Building_select_hook, &Building_select_orig))
		ErrorLog("[Building::select] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&MainBarGUI::_NV_autoChangeSelectedObject), &MainBarGUI_autoChangeSelectedObject_hook, &MainBarGUI_autoChangeSelectedObject_orig))
		ErrorLog("[MainBarGUI::autoChangeSelectedObject] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&ForgottenGUI::changeFontSize), &ForgottenGUI_changeFontSize_hook, &ForgottenGUI_changeFontSize_orig))
		ErrorLog("[ForgottenGUI::changeFontSize] could not install hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(KenshiLib::GetRealAddress(&OptionsWindow::create), OptionsWindow_create_hook, &OptionsWindow_create_orig))
		ErrorLog("[OptionsWindow::create] Could not add hook!");

	if (KenshiLib::SUCCESS != KenshiLib::AddHook(AppearanceManager_getEditorData, AppearanceManager_getEditorData_hook, &AppearanceManager_getEditorData_orig))
		ErrorLog("[AppearanceManager::getEditorData] Could not add hook!");
}
