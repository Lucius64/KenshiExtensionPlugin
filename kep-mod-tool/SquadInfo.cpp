/*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Copyright (C) 2025-2026 Lucius
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include "pch.h"
#include <SquadInfo.h>

#include <ogre/OgreStringConverter.h>

#include <kenshi/Globals.h>
#include <kenshi/GameWorld.h>
#include <kenshi/GameData.h>
#include <kenshi/Platoon.h>
#include <kenshi/Character.h>
#include <kenshi/Town.h>
#include <kenshi/gui/DatapanelGUI.h>
#include <kenshi/gui/DataPanelLine.h>
#include <kenshi/AI/AIPackage.h>
#include <kenshi/AI/AITaskSystem.h>
#include <kenshi/Tasker.h>
#include <kenshi/AI/Blackboard.h>
#include <kenshi/Campaign.h>

#include <kep/functions.h>
#include <kep/utility.h>

void KEP::tools::displayBlackboardInformation(Blackboard* self, DatapanelGUI* panel, int cat)
{
	if (self->contractJob != nullptr)
	{
		std::string s1 = KEP::GUIColor::getGoodBright() + "S: " + self->contractJob->packageData->name;
		std::string s2 = KEP::GUIColor::getMain() + Ogre::StringConverter::toString(self->contractJob->signalStart()) + ":" + Ogre::StringConverter::toString(self->contractJob->signalEnd());
		panel->setLine(s1, s2, cat, false, true);
	}

	for (auto packageIt = self->packagesMain.begin(); packageIt != self->packagesMain.end(); ++packageIt)
	{
		for (auto iter = packageIt->second.begin(); iter != packageIt->second.end(); ++iter)
		{
			auto aiPackage = (*iter);
			std::string s1 = (aiPackage == self->currentPackage ? KEP::GUIColor::getGoodBright() : KEP::GUIColor::getMain()) + Ogre::StringConverter::toString(packageIt->first) + ": " + aiPackage->packageData->name;
			std::string s2 = KEP::GUIColor::getMain() + Ogre::StringConverter::toString(aiPackage->signalStart()) + ":" + Ogre::StringConverter::toString(aiPackage->signalEnd());
			panel->setLine(s1, s2, cat, false, true);
		}
	}

	auto aiPackage = self->contractJob != nullptr ? self->contractJob : self->currentPackage;
	if (aiPackage != nullptr)
	{
		auto missionTarget = aiPackage->missionTarget.getRootObject();
		panel->setLine(KEP::GUIColor::getMain() + "Squad Mission Target:", KEP::GUIColor::getMain() + (missionTarget != nullptr ? missionTarget->getName() : "none"), cat, false, true);

		auto missionEmployer = aiPackage->missionEmployer.getRootObject();
		panel->setLine(KEP::GUIColor::getMain() + "Squad Mission Employer:", KEP::GUIColor::getMain() + (missionEmployer != nullptr ? missionEmployer->getName() : "none"), cat, false, true);

		auto missionTown = aiPackage->missionTown.getRootObject();
		panel->setLine(KEP::GUIColor::getMain() + "Squad Mission Town:", KEP::GUIColor::getMain() + (missionTown != nullptr ? missionTown->getName() : "none"), cat, false, true);
	}

	auto campaign = self->squad->hasCampaign();
	std::string campaignName = "---";
	std::string campaignPhase = "---";
	if (campaign != nullptr)
	{
		campaignName = campaign->getName();
		campaignPhase = Ogre::StringConverter::toString(campaign->currentPhase);
	}
	panel->setLine(KEP::GUIColor::getMain() + "Squad Campaign:", KEP::GUIColor::getMain() + campaignName, cat, false, true);
	panel->setLine(KEP::GUIColor::getMain() + "Campaign phase:", KEP::GUIColor::getMain() + campaignPhase, cat, false, true);

	std::string townName = self->currentTownLocation != nullptr ? self->currentTownLocation->getName() : "none";
	panel->setLine(KEP::GUIColor::getMain() + "Squad town:", KEP::GUIColor::getMain() + townName, cat, false, true);

	panel->setLine(KEP::GUIColor::getMain() + "permanent:", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(self->squad->isPersistentSquad()), cat, false, true);
	panel->setLine(KEP::GUIColor::getMain() + "Town time:", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(ou->getTimeFromStamp(self->townArrivalTime).time), cat, false, true);

	townName = self->targetTown != nullptr ? self->targetTown->getName() : "none";
	panel->setLine(KEP::GUIColor::getMain() + "Target town:", KEP::GUIColor::getMain() + townName, cat, false, true);
	panel->setLine(KEP::GUIColor::getMain() + "house locked", KEP::GUIColor::getMain() + Ogre::StringConverter::toString(self->homeIsLockedUp), cat, false, true);
	panel->setLine(KEP::GUIColor::getMain() + "Squad template:", KEP::GUIColor::getMain() + self->squad->squadTemplate->name, cat, false, true);
	panel->setLine(KEP::GUIColor::getMain() + "Squad ID:", KEP::GUIColor::getMain() + self->squad->getName(), cat, false, true);
}

void KEP::tools::displayAIGoalInformation(AI* self, DatapanelGUI* panel, int cat)
{

}
