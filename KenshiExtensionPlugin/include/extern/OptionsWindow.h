#pragma once
#include <kenshi/util/lektor.h>
#include <kenshi/TitleScreen.h>

class DataPanelLine_KeyConfig;
class DatapanelGUI;
class ToolTip;

// KenshiLibでは未定義
class OptionsWindow : public GUIWindow, public wraps::BaseLayout
{
public:
	char _0xd0;
	lektor<std::string> _0xd8;
	int _0xf0;
	DataPanelLine_KeyConfig* _0xf8;
	DatapanelGUI* datapanel;
	MyGUI::TabControl* optionsTab;
	bool _0x110;
	ToolTip* tooltip;
	bool _0x120;
};