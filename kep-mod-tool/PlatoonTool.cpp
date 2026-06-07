#include "pch.h"
#include <algorithm>
#include <PlatoonTool.h>

#include <mygui/MyGUI.h>

#include <Debug.h>
#include <kenshi/Kenshi.h>
#include <core/Functions.h>
#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/PlayerInterface.h>
#include <kenshi/Faction.h>
#include <kenshi/Platoon.h>
#include <kenshi/Character.h>
#include <kenshi/CharMovement.h>
#include <kenshi/CharBody.h>
#include <kenshi/Dialogue.h>
#include <kenshi/gui/ForgottenGUI.h>
#include <kenshi/gui/DatapanelGUI.h>
#include <kenshi/gui/DataPanelLine.h>
#include <extern/AI.h>
#include <extern/Blackboard.h>
#include <extern/CharacterMemory.h>

#include <kep/translation.h>
#include <kep/functions.h>
#include <kep/utility.h>
#include <utility.h>

namespace
{
	std::string lineBoxAIPackage;
	std::string lineBoxDialogue;

	void initLineKey()
	{
		lineBoxAIPackage = KEP::TranslationUtility::gettext("AI package");
		lineBoxDialogue = KEP::TranslationUtility::gettext("Diagloue");
	}
}

KEP::tools::PlatoonTool::PlatoonTool()
	: _panel(nullptr)
	, _category(0x5)
	, _selectedAIPackage(0)
{
	initLineKey();

	_initAIPackageList();
	_initDialogList();
}

void KEP::tools::PlatoonTool::refresh()
{
	if (this->_panel == nullptr)
		return;

	this->_panel->clearPage(this->_category);

	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext("[Task]"), "", this->_category, false, true);

	auto textbox = this->_panel->setLineTextEditable(KEP::TranslationUtility::gettext("Search for ai package"), "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<PlatoonTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &PlatoonTool::_changeAIPackageSearchText);

	this->_panel->setLineDropBox(lineBoxAIPackage, this->_category, &this->_selectedAIPackage, false, 0.7f);
	_updateAIPackageList("");

	auto button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Change AI"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<PlatoonTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &PlatoonTool::_changeAI);

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Reset AI"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<PlatoonTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &PlatoonTool::_resetAI);

	this->_panel->addSpace(this->_category, 1.0f);

	this->_panel->setLine(KEP::GUIColor::getMain() + KEP::TranslationUtility::gettext("[Dialog]"), "", this->_category, false, true);

	textbox = this->_panel->setLineTextEditable(KEP::TranslationUtility::gettext("Search for dialog"), "", this->_category, true, false, MyGUI::Align::Left, 0.7f);
	textbox->callback = new MyGUI::delegates::CMethodDelegate1<PlatoonTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &PlatoonTool::_changeDialogSearchText);

	this->_panel->setLineDropBox(lineBoxDialogue, this->_category, &this->_selectedDialog, false, 0.7f);
	_updateDialogList("");

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Trigger dialog"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<PlatoonTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &PlatoonTool::_triggerDialog);

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Clear all dialog timers"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<PlatoonTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &PlatoonTool::_clearDialogTimers);

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Lock dialog"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<PlatoonTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &PlatoonTool::_lockDialog);

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Unlock dialog"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<PlatoonTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &PlatoonTool::_unlockDialog);

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Clear all dialog locks and unlocks"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<PlatoonTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &PlatoonTool::_clearAllDialogLocksAndUnlocks);

	this->_panel->addSpace(this->_category, 1.0f);

	button = this->_panel->setLineTextButton("", KEP::TranslationUtility::gettext("Clear all tags"), this->_category, 0.7f, "Kenshi_Button2");
	button->callback = new MyGUI::delegates::CMethodDelegate1<PlatoonTool, DataPanelLine*>(MyGUI::delegates::GetDelegateUnlink(this), this, &PlatoonTool::_clearAllTag);

	this->_panel->addSpace(this->_category, 1.0f);
}

void KEP::tools::PlatoonTool::_changeAI(DataPanelLine* line)
{
	if (this->_selectedAIPackage < 0)
		return;

	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr || character->isPlayerCharacter())
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a NPC."), true);
		return;
	}

	auto platoon = character->getPlatoon();
	if (platoon == nullptr)
		return;

	if (platoon->me->blackboard == nullptr)
		return;

	for (auto iter = platoon->things.begin(); iter != platoon->things.end(); ++iter)
	{
		auto obj = reinterpret_cast<Character*>(*iter);
		obj->getMovement()->halt();
		KEP::functions->OrdersReceiver_deleteAllTask(obj->ai->taskSystem);
		obj->getBody()->_endAction();
	}

	auto data = this->_AIPackageList[this->_selectedAIPackage];

	KEP::functions->Blackboard_changeAI(platoon->me->blackboard, data);
}

void KEP::tools::PlatoonTool::_resetAI(DataPanelLine* line)
{
	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr || character->isPlayerCharacter())
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a NPC."), true);
		return;
	}

	auto platoon = character->getPlatoon();
	if (platoon == nullptr)
		return;

	auto blackboard = platoon->me->blackboard;
	if (blackboard == nullptr)
		return;

	blackboard->replacementAI = "";

	KEP::functions->Blackboard_deleteAllPackage(platoon->me->blackboard);
	KEP::functions->Blackboard_setFallbackPackage(blackboard, platoon->me->squadTemplate);
	KEP::functions->Blackboard_setSquadPackage(blackboard, platoon->me->squadTemplate);

	for (auto iter = platoon->things.begin(); iter != platoon->things.end(); ++iter)
	{
		auto obj = reinterpret_cast<Character*>(*iter);
		KEP::functions->OrdersReceiver_deleteAllPassiveJob(obj->ai->taskSystem);
		KEP::functions->ActionDeque_clear(&obj->ai->taskSystem->_0x300);
		KEP::functions->AITaskSystem_clear(obj->ai->taskSystem);
		obj->getMovement()->halt();
		KEP::functions->OrdersReceiver_deleteAllTask(obj->ai->taskSystem);
		obj->getBody()->_endAction();
	}
}

void KEP::tools::PlatoonTool::_clearAllTag(DataPanelLine* line)
{
	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr || character->isPlayerCharacter())
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a NPC."), true);
		return;
	}

	auto activePlatoon = character->getPlatoon();
	if (activePlatoon == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Platoon not found"), true);
		return;
	}

	if (activePlatoon->_myMemory->tags.size() != 0)
	{
		activePlatoon->_myMemory->tags.clear();
	}
}

void KEP::tools::PlatoonTool::_triggerDialog(DataPanelLine* line)
{
	if (this->_selectedDialog < 1)
		return;

	auto gamedata = this->_dialogList[this->_selectedDialog];
	if (DialogDataManager::getData(gamedata) == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Dialogue not found."), true);
		return;
	}

	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character."), true);
		return;
	}

	if (character->isUnconcious())
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character who is conscious."), true);
		return;
	}

	lektor<RootObject*> characters;
	auto pos = character->getPosition();
	ou->getCharactersWithinSphere(characters, pos, 250.0f, 0.0f, 30.0f, 40, 0, character);

	Character* nearestTarget = nullptr;
	float minDistance = 100000.0f;

	for (auto iter = characters.begin(); iter != characters.end(); ++iter)
	{
		auto target = reinterpret_cast<Character*>(*iter);
		if (target != nullptr && !character->isAlly(target, false) && !target->isAnimal() && !target->isUnconcious())
		{
			float distance = target->getPosition().distance(pos);
			if (distance < minDistance)
			{
				minDistance = distance;
				nearestTarget = target;
			}
		}
	}

	DialogChoiceList list;
	list.add(gamedata, nullptr);
	auto dialogLine = character->dialogue->_chooseDialog(&list, nearestTarget, false);
	character->dialogue->startConversation(nearestTarget, dialogLine, EV_I_SEE_NEUTRAL_SQUAD, true);
}

void KEP::tools::PlatoonTool::_lockDialog(DataPanelLine* line)
{
	if (this->_selectedDialog < 1)
		return;

	auto dialogLine = DialogDataManager::getData(this->_dialogList[this->_selectedDialog]);
	if (dialogLine == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Dialogue not found."), true);
		return;
	}

	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character."), true);
		return;
	}

	character->dialogue->locked[dialogLine] = true;
}

void KEP::tools::PlatoonTool::_unlockDialog(DataPanelLine* line)
{
	if (this->_selectedDialog < 1)
		return;

	auto dialogLine = DialogDataManager::getData(this->_dialogList[this->_selectedDialog]);
	if (dialogLine == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Dialogue not found."), true);
		return;
	}

	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character."), true);
		return;
	}

	character->dialogue->locked[dialogLine] = false;
}

void KEP::tools::PlatoonTool::_clearDialogTimers(DataPanelLine* line)
{
	auto& allDialogData = KEP::functions->getDialogDataManager_allDatas();
	for (auto iter = allDialogData.begin(); iter != allDialogData.end(); ++iter)
	{
		if (iter->second)
			iter->second->lastTimeSaid = -99;
	}
}

void KEP::tools::PlatoonTool::_clearAllDialogLocksAndUnlocks(DataPanelLine* line)
{
	auto character = gui->selectedObject.getCharacter();
	if (character == nullptr)
	{
		ou->showPlayerAMessage(KEP::TranslationUtility::gettext("Error: Please select a character."), true);
		return;
	}

	character->dialogue->locked.clear();
}

void KEP::tools::PlatoonTool::_changeAIPackageSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateAIPackageList(keyword);
}

void KEP::tools::PlatoonTool::_changeDialogSearchText(DataPanelLine* line)
{
	std::string keyword = "";
	if (line != nullptr && line->classType == DataPanelLine::DPL_TEXT_EDIT)
		keyword = reinterpret_cast<DataPanelLine_TextEditable*>(line)->editBox->getOnlyText();

	_updateDialogList(keyword);
}

void KEP::tools::PlatoonTool::_initAIPackageList()
{
	lektor<GameData*> datas;
	ou->gamedata.getDataOfType(datas, AI_PACKAGE);
	std::sort(datas.begin(), datas.end(), LessGameData());

	this->_AIPackageList.resize(datas.size());
	for (uint32_t i = 0; i < datas.size(); ++i)
	{
		this->_AIPackageList[i] = datas[i];
	}
}

void KEP::tools::PlatoonTool::_updateAIPackageList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxAIPackage, this->_category));
	if (dropBox == nullptr)
		return;

	int selectVal = -1;
	dropBox->clearValues();
	if (keyword.empty())
	{
		selectVal = 0;
		for (uint32_t i = 0; i < this->_AIPackageList.size(); ++i)
		{
			dropBox->addAValue(this->_AIPackageList[i]->name, i);
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 0; i < this->_AIPackageList.size(); ++i)
		{
			std::string& name = this->_AIPackageList[i]->name;
			std::string s2 = name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if (s2.find(s1) != std::string::npos)
			{
				dropBox->addAValue(name, i);
				if (selectVal == -1)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}

void KEP::tools::PlatoonTool::_initDialogList()
{
	lektor<GameData*> datas;
	ou->gamedata.getDataOfType(datas, DIALOGUE);
	std::sort(datas.begin(), datas.end(), LessGameData());

	this->_dialogList.resize(datas.size());
	for (uint32_t i = 0; i < datas.size(); ++i)
	{
		this->_dialogList[i] = datas[i];
	}
}

void KEP::tools::PlatoonTool::_updateDialogList(const std::string& keyword)
{
	auto dropBox = reinterpret_cast<DataPanelLine_DropBox*>(this->_panel->getLine(lineBoxDialogue, this->_category));
	if (dropBox == nullptr)
		return;

	int selectVal = -1;
	dropBox->clearValues();
	if (keyword.empty())
	{
		selectVal = 0;
		for (uint32_t i = 0; i < this->_dialogList.size(); ++i)
		{
			dropBox->addAValue(this->_dialogList[i]->name, i);
		}
	}
	else
	{
		std::string s1 = keyword;
		std::transform(s1.begin(), s1.end(), s1.begin(), [](char c) { return std::toupper(c); });
		for (uint32_t i = 0; i < this->_dialogList.size(); ++i)
		{
			std::string& name = this->_dialogList[i]->name;
			std::string s2 = name;
			std::transform(s2.begin(), s2.end(), s2.begin(), [](char c) { return std::toupper(c); });
			if (s2.find(s1) != std::string::npos)
			{
				dropBox->addAValue(name, i);
				if (selectVal == -1)
					selectVal = i;
			}
		}
	}
	dropBox->setSelectedValue(selectVal);
}
