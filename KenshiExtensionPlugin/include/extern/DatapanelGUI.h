#pragma once
#include <string>
#include <ogre/OgrePrerequisites.h>
#include <kenshi/util/lektor.h>
#include <kenshi/TitleScreen.h>

namespace MyGUI
{
	class Button;
	class ScrollView;
	class Widget;
	class ScrollBar;
	class ComboBox;
}

class ToolTip;
class DataPanelLine;

// KenshiLibでは未定義
class DatapanelGUI : public GUIWindow
{
public:
	virtual ~DatapanelGUI();
	size_t _0x30;
	size_t _0x38;
	size_t _0x40;
	size_t _0x48;
	size_t _0x50;
	bool _0x58;
	bool _0x59;
	bool _0x5a;
	Ogre::map<int, Ogre::map<std::string, DataPanelLine*>::type>::type _0x60;
	Ogre::map<int, DataPanelLine*>::type _0x88;
	int _0xb0;
	size_t _0xb8;
	size_t _0xc0;
	bool _0xc8;
	bool _0xc9;
	size_t _0xd0;
	lektor<MyGUI::Button*> _0xd8;
	std::string _0xf0;
	float _0x118;
	float _0x11c;
	float _0x120;
	float _0x124;
	float _0x128;
	float _0x12c;
	bool _0x130;
	std::string _0x138;
	MyGUI::ScrollView* _0x160;

	virtual void vfunc0x68();
	virtual void vfunc0x70();
	virtual void vfunc0x78();
	virtual void vfunc0x80();
	virtual void vfunc0x88();
	virtual void vfunc0x90();
	virtual void vfunc0x98();
	virtual void vfunc0xa0();
	virtual void vfunc0xa8();
	virtual void vfunc0xb0();
	virtual void vfunc0xb8();
	virtual void vfunc0xc0(int);
	virtual void vfunc0xc8();
	virtual void vfunc0xd0();
	virtual void vfunc0xd8();
	virtual void vfunc0xe0(float);
	virtual void vfunc0xe8();
	virtual void vfunc0xf0();
};

// KenshiLibでは未定義
class DataPanelLine : public Ogre::GeneralAllocatedObject
{
public:
	size_t _0x8;
	size_t _0x10;
	int id;
	MyGUI::delegates::CStaticDelegate1<DataPanelLine*>* _0x20;
	std::string _0x28;
	std::string _0x50;
	std::string _0x78;
	bool _0xa0;
	std::string _0xa8;
	std::string _0xd0;
	Ogre::vector<MyGUI::Widget*>::type _0xf8;
	int type;

	virtual ~DataPanelLine();
	virtual void setVisible(bool);
	virtual void setEnabled(bool);
	virtual void change();
	virtual void input(float&);
	virtual void setTooltip(const std::string&, ToolTip*);
	//virtual void setTooltip(const std::string&);

};

// KenshiLibでは未定義
class DataPanelLine_CheckBox : public DataPanelLine
{
public:
	bool* valuePtr;
	size_t _0x128;
	MyGUI::Button* _0x130;
};

// KenshiLibでは未定義
class DataPanelLine_SliderEditable : public DataPanelLine
{
	int _0x120;
	bool _0x124;
	float min;
	float max;
	float* valuePtr;
	float _0x138;
	size_t _0x140;
	size_t _0x148;
	MyGUI::ScrollBar* _0x150;
};

// KenshiLibでは未定義
class DataPanelLine_DropBox : public DataPanelLine
{
	MyGUI::ComboBox* _0x120;
	float _0x128;
	float* valuePtr;
	float _0x138;
	size_t _0x140;
};
