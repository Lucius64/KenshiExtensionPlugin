#pragma once
#include "common.h"
#include <string>

namespace MyGUI
{
	struct Colour;
}

namespace KEP
{
	DECLSPEC const std::string& getPluginPath();

	namespace GUIColor
	{
		void init(unsigned int platform, const std::string& version, uintptr_t baseAddr);
		DECLSPEC const std::string& getMain();
		DECLSPEC const std::string& getSecondary();
		DECLSPEC const std::string& getTitle();
		DECLSPEC const std::string& getBad();
		DECLSPEC const std::string& getBadBright();
		DECLSPEC const std::string& getGood();
		DECLSPEC const std::string& getGooddBright();
		DECLSPEC const std::string& getGreyed();
		DECLSPEC const std::string& getGreyedBright();
		DECLSPEC const std::string& getSpecial();
		DECLSPEC const MyGUI::Colour& getNameTagSelected();
		DECLSPEC const MyGUI::Colour& getNameTagUnselected();
		DECLSPEC const MyGUI::Colour& getNameTagStealthHidden();
		DECLSPEC const MyGUI::Colour& getNameTagStealthAlmostSeen();
		DECLSPEC const MyGUI::Colour& getTagStealthSeen();
		DECLSPEC const MyGUI::Colour& geDialogueBubbleText();
	}

#ifdef KEPCORE_EXPORTS
	void initialize();
#endif
}
