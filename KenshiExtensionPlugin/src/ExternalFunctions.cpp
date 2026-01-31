#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <boost/scoped_ptr.hpp>

#include <ExternalFunctions.h>


boost::scoped_ptr<KEP::ExternalFunctions> KEP::externalFunctions(new KEP::ExternalFunctions());
boost::scoped_ptr<KEP::ExternalGlobals> KEP::externalGlobals(new KEP::ExternalGlobals());


KEP::ExternalGlobals::ExternalGlobals()
	: _gBaseXpCombat(nullptr)
	, _optionsAdvanced(nullptr)
	, _playerMorphCounter(nullptr)
	, _KenshiFolder(nullptr)
	, _UtilityT(nullptr)
	, _CLASS_02133098(nullptr)
	, _CLASS_021322B0(nullptr)
	, _hand_01E395F8(nullptr)
	, _MainColorCode(nullptr)
	, _SecondarColorCode(nullptr)
	, _TitleColorCode(nullptr)
	, _BadColorCode(nullptr)
	, _BadBrightColorCode(nullptr)
	, _GoodColorCode(nullptr)
	, _GoodBrightColorCode(nullptr)
	, _GreyedColorCode(nullptr)
	, _GreyedBrightColorCode(nullptr)
	, _SpecialColorCode(nullptr)
	, _NameTagSelectedColor(nullptr)
	, _NameTagUnselectedColor(nullptr)
	, _NameTagStealthHiddenColor(nullptr)
	, _NameTagStealthAlmostSeenColor(nullptr)
	, _NameTagStealthSeenColor(nullptr)
	, _DialogueBubbleTextColor(nullptr)
	, _KenshiGUI(nullptr)
	, _LocaleInfoManagerPtr(nullptr)
	, _TemporaryLimbInterface(nullptr)
	, _InventorySlotSkinSize(nullptr)
	, _dropItemReturnAddress(0)
{

}

void KEP::ExternalGlobals::init(unsigned int platform, const std::string& version, uintptr_t baseAddr)
{
	if (platform == 1)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&this->_gBaseXpCombat                 = baseAddr + 0x02133e7c;
			*(uintptr_t*)&this->_optionsAdvanced               = baseAddr + 0x02132528;
			*(uintptr_t*)&this->_playerMorphCounter            = baseAddr + 0x02126c80;
			*(uintptr_t*)&this->_KenshiFolder                  = baseAddr + 0x01f9ee38;
			*(uintptr_t*)&this->_UtilityT                      = baseAddr + 0x02134b10;
			*(uintptr_t*)&this->_CLASS_02133098                = baseAddr + 0x02133098;
			*(uintptr_t*)&this->_CLASS_021322B0                = baseAddr + 0x021322b0;
			*(uintptr_t*)&this->_hand_01E395F8                 = baseAddr + 0x01e395f8;
			*(uintptr_t*)&this->_MainColorCode                 = baseAddr + 0x01f48238;
			*(uintptr_t*)&this->_SecondarColorCode             = baseAddr + 0x01f48260;
			*(uintptr_t*)&this->_TitleColorCode                = baseAddr + 0x01f48288;
			*(uintptr_t*)&this->_BadColorCode                  = baseAddr + 0x01f482b0;
			*(uintptr_t*)&this->_BadBrightColorCode            = baseAddr + 0x01f482d8;
			*(uintptr_t*)&this->_GoodColorCode                 = baseAddr + 0x01f48300;
			*(uintptr_t*)&this->_GoodBrightColorCode           = baseAddr + 0x01f48328;
			*(uintptr_t*)&this->_GreyedColorCode               = baseAddr + 0x01f48350;
			*(uintptr_t*)&this->_GreyedBrightColorCode         = baseAddr + 0x01f48378;
			*(uintptr_t*)&this->_SpecialColorCode              = baseAddr + 0x01f483a0;
			*(uintptr_t*)&this->_NameTagSelectedColor          = baseAddr + 0x02131aa0;
			*(uintptr_t*)&this->_NameTagUnselectedColor        = baseAddr + 0x02131ab0;
			*(uintptr_t*)&this->_NameTagStealthHiddenColor     = baseAddr + 0x02131ac0;
			*(uintptr_t*)&this->_NameTagStealthAlmostSeenColor = baseAddr + 0x02131ad0;
			*(uintptr_t*)&this->_NameTagStealthSeenColor       = baseAddr + 0x02131ae0;
			*(uintptr_t*)&this->_DialogueBubbleTextColor       = baseAddr + 0x02131c00;
			*(uintptr_t*)&this->_KenshiGUI                     = baseAddr + 0x02132750;
			*(uintptr_t*)&this->_LocaleInfoManagerPtr          = baseAddr + 0x02127370;
			*(uintptr_t*)&this->_TemporaryLimbInterface        = baseAddr + 0x02126cd0;
			*(uintptr_t*)&this->_InventorySlotSkinSize         = baseAddr + 0x02127440;
			this->_dropItemReturnAddress                       = baseAddr + 0x00712c49;
		}
		else if (version == "1.0.68")
		{
			*(uintptr_t*)&this->_gBaseXpCombat                 = baseAddr + 0x02134edc;
			*(uintptr_t*)&this->_optionsAdvanced               = baseAddr + 0x02133578;
			*(uintptr_t*)&this->_playerMorphCounter            = baseAddr + 0x02127c90;
			*(uintptr_t*)&this->_KenshiFolder                  = baseAddr + 0x01f9fe48;
			*(uintptr_t*)&this->_UtilityT                      = baseAddr + 0x02135b70;
			*(uintptr_t*)&this->_CLASS_02133098                = baseAddr + 0x021340f8;
			*(uintptr_t*)&this->_CLASS_021322B0                = baseAddr + 0x02133300;
			*(uintptr_t*)&this->_hand_01E395F8                 = baseAddr + 0x01e3a5f8;
			*(uintptr_t*)&this->_MainColorCode                 = baseAddr + 0x01f49248;
			*(uintptr_t*)&this->_SecondarColorCode             = baseAddr + 0x01f49270;
			*(uintptr_t*)&this->_TitleColorCode                = baseAddr + 0x01f49298;
			*(uintptr_t*)&this->_BadColorCode                  = baseAddr + 0x01f492c0;
			*(uintptr_t*)&this->_BadBrightColorCode            = baseAddr + 0x01f492e8;
			*(uintptr_t*)&this->_GoodColorCode                 = baseAddr + 0x01f49310;
			*(uintptr_t*)&this->_GoodBrightColorCode           = baseAddr + 0x01f49338;
			*(uintptr_t*)&this->_GreyedColorCode               = baseAddr + 0x01f49360;
			*(uintptr_t*)&this->_GreyedBrightColorCode         = baseAddr + 0x01f49388;
			*(uintptr_t*)&this->_SpecialColorCode              = baseAddr + 0x01f493b0;
			*(uintptr_t*)&this->_NameTagSelectedColor          = baseAddr + 0x02132ae0;
			*(uintptr_t*)&this->_NameTagUnselectedColor        = baseAddr + 0x02132af0;
			*(uintptr_t*)&this->_NameTagStealthHiddenColor     = baseAddr + 0x02132b00;
			*(uintptr_t*)&this->_NameTagStealthAlmostSeenColor = baseAddr + 0x02132b10;
			*(uintptr_t*)&this->_NameTagStealthSeenColor       = baseAddr + 0x02132b20;
			*(uintptr_t*)&this->_DialogueBubbleTextColor       = baseAddr + 0x02132c50;
			*(uintptr_t*)&this->_KenshiGUI                     = baseAddr + 0x021337b0;
			*(uintptr_t*)&this->_LocaleInfoManagerPtr          = baseAddr + 0x02128380;
			*(uintptr_t*)&this->_TemporaryLimbInterface        = baseAddr + 0x02127ce0;
			*(uintptr_t*)&this->_InventorySlotSkinSize         = baseAddr + 0x02128450;
			this->_dropItemReturnAddress                       = baseAddr + 0x00713819;
		}
	}
	else if(platform == 0)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&this->_gBaseXpCombat                 = baseAddr + 0x02131dec;
			*(uintptr_t*)&this->_optionsAdvanced               = baseAddr + 0x02130498;
			*(uintptr_t*)&this->_playerMorphCounter            = baseAddr + 0x02124c90;
			*(uintptr_t*)&this->_KenshiFolder                  = baseAddr + 0x01f9ce48;
			*(uintptr_t*)&this->_UtilityT                      = baseAddr + 0x02132a80;
			*(uintptr_t*)&this->_CLASS_02133098                = baseAddr + 0x02131008;
			*(uintptr_t*)&this->_CLASS_021322B0                = baseAddr + 0x02130220;
			*(uintptr_t*)&this->_hand_01E395F8                 = baseAddr + 0x01e375f8;
			*(uintptr_t*)&this->_MainColorCode                 = baseAddr + 0x01f46248;
			*(uintptr_t*)&this->_SecondarColorCode             = baseAddr + 0x01f46270;
			*(uintptr_t*)&this->_TitleColorCode                = baseAddr + 0x01f46298;
			*(uintptr_t*)&this->_BadColorCode                  = baseAddr + 0x01f462c0;
			*(uintptr_t*)&this->_BadBrightColorCode            = baseAddr + 0x01f462e8;
			*(uintptr_t*)&this->_GoodColorCode                 = baseAddr + 0x01f46310;
			*(uintptr_t*)&this->_GoodBrightColorCode           = baseAddr + 0x01f46338;
			*(uintptr_t*)&this->_GreyedColorCode               = baseAddr + 0x01f46360;
			*(uintptr_t*)&this->_GreyedBrightColorCode         = baseAddr + 0x01f46388;
			*(uintptr_t*)&this->_SpecialColorCode              = baseAddr + 0x01f463b0;
			*(uintptr_t*)&this->_NameTagSelectedColor          = baseAddr + 0x0212fa10;
			*(uintptr_t*)&this->_NameTagUnselectedColor        = baseAddr + 0x0212fa20;
			*(uintptr_t*)&this->_NameTagStealthHiddenColor     = baseAddr + 0x0212fa30;
			*(uintptr_t*)&this->_NameTagStealthAlmostSeenColor = baseAddr + 0x0212fa40;
			*(uintptr_t*)&this->_NameTagStealthSeenColor       = baseAddr + 0x0212fa50;
			*(uintptr_t*)&this->_DialogueBubbleTextColor       = baseAddr + 0x0212fb70;
			*(uintptr_t*)&this->_KenshiGUI                     = baseAddr + 0x021306c0;
			*(uintptr_t*)&this->_LocaleInfoManagerPtr          = baseAddr + 0x02125380;
			*(uintptr_t*)&this->_TemporaryLimbInterface        = baseAddr + 0x02124ce0;
			*(uintptr_t*)&this->_InventorySlotSkinSize         = baseAddr + 0x02125450;
			this->_dropItemReturnAddress                       = baseAddr + 0x007125a9;
		}
		else if (version == "1.0.68")
		{
			*(uintptr_t*)&this->_gBaseXpCombat                 = baseAddr + 0x02133e0c;
			*(uintptr_t*)&this->_optionsAdvanced               = baseAddr + 0x021324a8;
			*(uintptr_t*)&this->_playerMorphCounter            = baseAddr + 0x02126c90;
			*(uintptr_t*)&this->_KenshiFolder                  = baseAddr + 0x01f9ee48;
			*(uintptr_t*)&this->_UtilityT                      = baseAddr + 0x02134aa0;
			*(uintptr_t*)&this->_CLASS_02133098                = baseAddr + 0x02133028;
			*(uintptr_t*)&this->_CLASS_021322B0                = baseAddr + 0x02132230;
			*(uintptr_t*)&this->_hand_01E395F8                 = baseAddr + 0x01e395f8;
			*(uintptr_t*)&this->_MainColorCode                 = baseAddr + 0x01f48248;
			*(uintptr_t*)&this->_SecondarColorCode             = baseAddr + 0x01f48270;
			*(uintptr_t*)&this->_TitleColorCode                = baseAddr + 0x01f48298;
			*(uintptr_t*)&this->_BadColorCode                  = baseAddr + 0x01f482c0;
			*(uintptr_t*)&this->_BadBrightColorCode            = baseAddr + 0x01f482e8;
			*(uintptr_t*)&this->_GoodColorCode                 = baseAddr + 0x01f48310;
			*(uintptr_t*)&this->_GoodBrightColorCode           = baseAddr + 0x01f48338;
			*(uintptr_t*)&this->_GreyedColorCode               = baseAddr + 0x01f48360;
			*(uintptr_t*)&this->_GreyedBrightColorCode         = baseAddr + 0x01f48388;
			*(uintptr_t*)&this->_SpecialColorCode              = baseAddr + 0x01f483b0;
			*(uintptr_t*)&this->_NameTagSelectedColor          = baseAddr + 0x02131a10;
			*(uintptr_t*)&this->_NameTagUnselectedColor        = baseAddr + 0x02131a20;
			*(uintptr_t*)&this->_NameTagStealthHiddenColor     = baseAddr + 0x02131a30;
			*(uintptr_t*)&this->_NameTagStealthAlmostSeenColor = baseAddr + 0x02131a40;
			*(uintptr_t*)&this->_NameTagStealthSeenColor       = baseAddr + 0x02131a50;
			*(uintptr_t*)&this->_DialogueBubbleTextColor       = baseAddr + 0x02131b80;
			*(uintptr_t*)&this->_KenshiGUI                     = baseAddr + 0x021326e0;
			*(uintptr_t*)&this->_LocaleInfoManagerPtr          = baseAddr + 0x02127380;
			*(uintptr_t*)&this->_TemporaryLimbInterface        = baseAddr + 0x02126ce0;
			*(uintptr_t*)&this->_InventorySlotSkinSize         = baseAddr + 0x02127450;
			this->_dropItemReturnAddress                       = baseAddr + 0x007131b9;
		}
	}
}

KEP::ExternalFunctions::ExternalFunctions()
	: FUN_00071920(nullptr)
	, FUN_00071A80(nullptr)
	, FUN_0007B780(nullptr)
	, FUN_0007C030(nullptr)
	, FUN_008C4F20(nullptr)
	, FUN_0043BDE0(nullptr)
	, FUN_00354500(nullptr)
	, FUN_00505D50(nullptr)
	, FUN_00591720(nullptr)
	, FUN_005E7D60(nullptr)
	, FUN_009A7AA0(nullptr)
	, FUN_009A93A0(nullptr)
	, FUN_009AFCA0(nullptr)
	, FUN_009C4000(nullptr)
	, FUN_009C4170(nullptr)
	, FUN_009C3280(nullptr)
	, FUN_002834C0(nullptr)
	, FUN_009C3510(nullptr)
	, FUN_009429B0(nullptr)
	, FUN_008F47E0(nullptr)
	, FUN_008F4200(nullptr)
	, FUN_005375D0(nullptr)
	, FUN_00531B70(nullptr)
	, FUN_00536C00(nullptr)
	, FUN_000B70A0(nullptr)
	, FUN_005BC680(nullptr)
	, FUN_0084A9C0(nullptr)
	, FUN_00845060(nullptr)
	, FUN_00844C90(nullptr)
	, FUN_008DBF80(nullptr)
	, FUN_008DC0D0(nullptr)
	, FUN_00958550(nullptr)
	, FUN_004FDE00(nullptr)
	, FUN_0095A340(nullptr)
	, FUN_002B7860(nullptr)
	, FUN_00371480(nullptr)
	, FUN_009FD8D0(nullptr)
	, FUN_000D5FD0(nullptr)
	, FUN_000A9580(nullptr)
	, FUN_00830b90(nullptr)
	, FUN_00406B90(nullptr)
	, FUN_003FB250(nullptr)
	, FUN_006FDE20(nullptr)
	, FUN_006FE210(nullptr)
	, FUN_006FE880(nullptr)
	, FUN_006FE2D0(nullptr)
	, FUN_0073F4B0(nullptr)
	, FUN_006F57E0(nullptr)
	, FUN_006F6340(nullptr)
	, FUN_003F0120(nullptr)
	, FUN_003EC950(nullptr)
	, FUN_0048EFA0(nullptr)
	, FUN_0048EA30(nullptr)
	, FUN_005065E0(nullptr)
	, FUN_002715E0(nullptr)
	, FUN_0050C720(nullptr)
	, FUN_005067F0(nullptr)
	, FUN_004AB930(nullptr)
	, FUN_00927900(nullptr)
	, FUN_0029E770(nullptr)
	, FUN_0029F940(nullptr)
	, FUN_002A5F00(nullptr)
	, FUN_002A5710(nullptr)
	, FUN_0029C810(nullptr)
	, FUN_00299750(nullptr)
	, FUN_005A3B60(nullptr)
	, FUN_00340EB0(nullptr)
	, FUN_00343720(nullptr)
	, FUN_001A4FB0(nullptr)
	, FUN_000CE290(nullptr)
	, FUN_00954340(nullptr)
	, FUN_006BEAC0(nullptr)
	, FUN_0052B1E0(nullptr)
	, FUN_0007F0B0(nullptr)
	, FUN_0053E320(nullptr)
	, FUN_0006F8F0(nullptr)
	, FUN_005B97F0(nullptr)
	, FUN_00413FE0(nullptr)
	, FUN_006B3460(nullptr)
	, FUN_006B4A70(nullptr)
	, FUN_0051DA50(nullptr)
	, FUN_005B7240(nullptr)
	, FUN_005B7030(nullptr)
	, FUN_005B3020(nullptr)
	, FUN_005B33B0(nullptr)
	, FUN_005B3380(nullptr)
	, FUN_00155350(nullptr)
	, FUN_0015D810(nullptr)
	, FUN_0011DDC0(nullptr)
	, FUN_0011CD50(nullptr)
	, FUN_00119C00(nullptr)
	, FUN_0014F530(nullptr)
	, FUN_0014E390(nullptr)
	, FUN_0070C0A0(nullptr)
	, FUN_0070C0B0(nullptr)
	, FUN_000CD830(nullptr)
	, FUN_00A09840(nullptr)
	, FUN_000D8620(nullptr)
	, FUN_000D6360(nullptr)
	, FUN_000D8310(nullptr)
	, FUN_004B0B60(nullptr)
	, FUN_002C9840(nullptr)
	, FUN_002BB270(nullptr)
	, FUN_002CE000(nullptr)
	, FUN_002BBCE0(nullptr)
	, FUN_002B8260(nullptr)
	, FUN_0082E2E0(nullptr)
	, FUN_002CA350(nullptr)
	, FUN_00912CA0(nullptr)
	, target_Ogre_Skeleton__refreshAnimationState(nullptr)
{

}

void KEP::ExternalFunctions::init(unsigned int platform, const std::string& version, uintptr_t baseAddr)
{

	if (platform == 1)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&this->FUN_00071920 = baseAddr + 0x71920;
			*(uintptr_t*)&this->FUN_00071A80 = baseAddr + 0x71a80;
			*(uintptr_t*)&this->FUN_0007B780 = baseAddr + 0x7b780;
			*(uintptr_t*)&this->FUN_0007C030 = baseAddr + 0x7c030;
			*(uintptr_t*)&this->FUN_008C4F20 = baseAddr + 0x8c4f20;
			*(uintptr_t*)&this->FUN_0043BDE0 = baseAddr + 0x43bde0;
			*(uintptr_t*)&this->FUN_00354500 = baseAddr + 0x354500;
			*(uintptr_t*)&this->FUN_00505D50 = baseAddr + 0x505d50;
			*(uintptr_t*)&this->FUN_00591720 = baseAddr + 0x591720;
			*(uintptr_t*)&this->FUN_005E7D60 = baseAddr + 0x5e7d60;
			*(uintptr_t*)&this->FUN_009A7AA0 = baseAddr + 0x9a7aa0;
			*(uintptr_t*)&this->FUN_009A93A0 = baseAddr + 0x9a93a0;
			*(uintptr_t*)&this->FUN_009AFCA0 = baseAddr + 0x9afca0;
			*(uintptr_t*)&this->FUN_009C4000 = baseAddr + 0x9c4000;
			*(uintptr_t*)&this->FUN_009C4170 = baseAddr + 0x9c4170;
			*(uintptr_t*)&this->FUN_009C3280 = baseAddr + 0x9c3280;
			*(uintptr_t*)&this->FUN_002834C0 = baseAddr + 0x2834c0;
			*(uintptr_t*)&this->FUN_009C3510 = baseAddr + 0x9c3510;
			*(uintptr_t*)&this->FUN_009429B0 = baseAddr + 0x9429b0;
			*(uintptr_t*)&this->FUN_008F47E0 = baseAddr + 0x8f47e0;
			*(uintptr_t*)&this->FUN_008F4200 = baseAddr + 0x8f4200;
			*(uintptr_t*)&this->FUN_005375D0 = baseAddr + 0x5375d0;
			*(uintptr_t*)&this->FUN_00531B70 = baseAddr + 0x531b70;
			*(uintptr_t*)&this->FUN_00536C00 = baseAddr + 0x536c00;
			*(uintptr_t*)&this->FUN_000B70A0 = baseAddr + 0xb70a0;
			*(uintptr_t*)&this->FUN_005BC680 = baseAddr + 0x5bc680;
			*(uintptr_t*)&this->FUN_0084A9C0 = baseAddr + 0x84a9c0;
			*(uintptr_t*)&this->FUN_00845060 = baseAddr + 0x845060;
			*(uintptr_t*)&this->FUN_00844C90 = baseAddr + 0x844c90;
			*(uintptr_t*)&this->FUN_008DBF80 = baseAddr + 0x8dbf80;
			*(uintptr_t*)&this->FUN_008DC0D0 = baseAddr + 0x8dc0d0;
			*(uintptr_t*)&this->FUN_00958550 = baseAddr + 0x958550;
			*(uintptr_t*)&this->FUN_004FDE00 = baseAddr + 0x4fde00;
			*(uintptr_t*)&this->FUN_0095A340 = baseAddr + 0x95a340;
			*(uintptr_t*)&this->FUN_002B7860 = baseAddr + 0x2b7860;
			*(uintptr_t*)&this->FUN_00371480 = baseAddr + 0x371480;
			*(uintptr_t*)&this->FUN_009FD8D0 = baseAddr + 0x9fd8d0;
			*(uintptr_t*)&this->FUN_000D5FD0 = baseAddr + 0xd5fd0;
			*(uintptr_t*)&this->FUN_000A9580 = baseAddr + 0xa9580;
			*(uintptr_t*)&this->FUN_00830b90 = baseAddr + 0x830b90;
			*(uintptr_t*)&this->FUN_00406B90 = baseAddr + 0x406b90;
			*(uintptr_t*)&this->FUN_003FB250 = baseAddr + 0x3fb250;
			*(uintptr_t*)&this->FUN_006FDE20 = baseAddr + 0x6fde20;
			*(uintptr_t*)&this->FUN_006FE210 = baseAddr + 0x6fe210;
			*(uintptr_t*)&this->FUN_006FE880 = baseAddr + 0x6fe880;
			*(uintptr_t*)&this->FUN_006FE2D0 = baseAddr + 0x6fe2d0;
			*(uintptr_t*)&this->FUN_0073F4B0 = baseAddr + 0x73f4b0;
			*(uintptr_t*)&this->FUN_006F57E0 = baseAddr + 0x6f57e0;
			*(uintptr_t*)&this->FUN_006F6340 = baseAddr + 0x6f6340;
			*(uintptr_t*)&this->FUN_003F0120 = baseAddr + 0x3f0120;
			*(uintptr_t*)&this->FUN_003EC950 = baseAddr + 0x3ec950;
			*(uintptr_t*)&this->FUN_0048EFA0 = baseAddr + 0x48efa0;
			*(uintptr_t*)&this->FUN_0048EA30 = baseAddr + 0x48ea30;
			*(uintptr_t*)&this->FUN_005065E0 = baseAddr + 0x5065e0;
			*(uintptr_t*)&this->FUN_002715E0 = baseAddr + 0x2715e0;
			*(uintptr_t*)&this->FUN_0050C720 = baseAddr + 0x50c720;
			*(uintptr_t*)&this->FUN_005067F0 = baseAddr + 0x5067f0;
			*(uintptr_t*)&this->FUN_004AB930 = baseAddr + 0x4ab930;
			*(uintptr_t*)&this->FUN_00927900 = baseAddr + 0x927900;
			*(uintptr_t*)&this->FUN_0029E770 = baseAddr + 0x29e770;
			*(uintptr_t*)&this->FUN_0029F940 = baseAddr + 0x29f940;
			*(uintptr_t*)&this->FUN_002A5F00 = baseAddr + 0x2a5f00;
			*(uintptr_t*)&this->FUN_002A5710 = baseAddr + 0x2a5710;
			*(uintptr_t*)&this->FUN_0029C810 = baseAddr + 0x29c810;
			*(uintptr_t*)&this->FUN_00299750 = baseAddr + 0x299750;
			*(uintptr_t*)&this->FUN_005A3B60 = baseAddr + 0x5a3b60;
			*(uintptr_t*)&this->FUN_00340EB0 = baseAddr + 0x340eb0;
			*(uintptr_t*)&this->FUN_00343720 = baseAddr + 0x343720;
			*(uintptr_t*)&this->FUN_001A4FB0 = baseAddr + 0x1a4fb0;
			*(uintptr_t*)&this->FUN_000CE290 = baseAddr + 0xce290;
			*(uintptr_t*)&this->FUN_00954340 = baseAddr + 0x954340;
			*(uintptr_t*)&this->FUN_006BEAC0 = baseAddr + 0x6beac0;
			*(uintptr_t*)&this->FUN_0052B1E0 = baseAddr + 0x52b1e0;
			*(uintptr_t*)&this->FUN_0007F0B0 = baseAddr + 0x7f0b0;
			*(uintptr_t*)&this->FUN_0053E320 = baseAddr + 0x53e320;
			*(uintptr_t*)&this->FUN_0006F8F0 = baseAddr + 0x6f8f0;
			*(uintptr_t*)&this->FUN_005B97F0 = baseAddr + 0x5b97f0;
			*(uintptr_t*)&this->FUN_00413FE0 = baseAddr + 0x413fe0;
			*(uintptr_t*)&this->FUN_006B3460 = baseAddr + 0x6b3460;
			*(uintptr_t*)&this->FUN_006B4A70 = baseAddr + 0x6b4a70;
			*(uintptr_t*)&this->FUN_0051DA50 = baseAddr + 0x51da50;
			*(uintptr_t*)&this->FUN_005B7240 = baseAddr + 0x5b7240;
			*(uintptr_t*)&this->FUN_005B7030 = baseAddr + 0x5b7030;
			*(uintptr_t*)&this->FUN_005B3020 = baseAddr + 0x5b3020;
			*(uintptr_t*)&this->FUN_005B33B0 = baseAddr + 0x5b33b0;
			*(uintptr_t*)&this->FUN_005B3380 = baseAddr + 0x5b3380;
			*(uintptr_t*)&this->FUN_00155350 = baseAddr + 0x155350;
			*(uintptr_t*)&this->FUN_0015D810 = baseAddr + 0x15d810;
			*(uintptr_t*)&this->FUN_0011DDC0 = baseAddr + 0x11ddc0;
			*(uintptr_t*)&this->FUN_0011CD50 = baseAddr + 0x11cd50;
			*(uintptr_t*)&this->FUN_00119C00 = baseAddr + 0x119c00;
			*(uintptr_t*)&this->FUN_0014F530 = baseAddr + 0x14f530;
			*(uintptr_t*)&this->FUN_0014E390 = baseAddr + 0x14e390;
			*(uintptr_t*)&this->FUN_0070C0A0 = baseAddr + 0x70c0a0;
			*(uintptr_t*)&this->FUN_0070C0B0 = baseAddr + 0x70c0b0;
			*(uintptr_t*)&this->FUN_000CD830 = baseAddr + 0xcd830;
			*(uintptr_t*)&this->FUN_00A09840 = baseAddr + 0xa09840;
			*(uintptr_t*)&this->FUN_000D8620 = baseAddr + 0xd8620;
			*(uintptr_t*)&this->FUN_000D6360 = baseAddr + 0xd6360;
			*(uintptr_t*)&this->FUN_000D8310 = baseAddr + 0xd8310;
			*(uintptr_t*)&this->FUN_004B0B60 = baseAddr + 0x4b0b60;
			*(uintptr_t*)&this->FUN_002C9840 = baseAddr + 0x2c9840;
			*(uintptr_t*)&this->FUN_002BB270 = baseAddr + 0x2bb270;
			*(uintptr_t*)&this->FUN_002CE000 = baseAddr + 0x2ce000;
			*(uintptr_t*)&this->FUN_002BBCE0 = baseAddr + 0x2bbce0;
			*(uintptr_t*)&this->FUN_002B8260 = baseAddr + 0x2b8260;
			*(uintptr_t*)&this->FUN_0082E2E0 = baseAddr + 0x82e2e0;
			*(uintptr_t*)&this->FUN_002CA350 = baseAddr + 0x2ca350;
			*(uintptr_t*)&this->FUN_00912CA0 = baseAddr + 0x912ca0;
		}
		else if (version == "1.0.68")
		{
			*(uintptr_t*)&this->FUN_00071920 = baseAddr + 0x00071960;
			*(uintptr_t*)&this->FUN_00071A80 = baseAddr + 0x00071ac0;
			*(uintptr_t*)&this->FUN_0007B780 = baseAddr + 0x0007b7c0;
			*(uintptr_t*)&this->FUN_0007C030 = baseAddr + 0x0007c070;
			*(uintptr_t*)&this->FUN_008C4F20 = baseAddr + 0x008c5df0;
			*(uintptr_t*)&this->FUN_0043BDE0 = baseAddr + 0x0043c180;
			*(uintptr_t*)&this->FUN_00354500 = baseAddr + 0x00354560;
			*(uintptr_t*)&this->FUN_00505D50 = baseAddr + 0x005067e0;
			*(uintptr_t*)&this->FUN_00591720 = baseAddr + 0x005921b0;
			*(uintptr_t*)&this->FUN_005E7D60 = baseAddr + 0x005e87f0;
			*(uintptr_t*)&this->FUN_009A7AA0 = baseAddr + 0x009a8970;
			*(uintptr_t*)&this->FUN_009A93A0 = baseAddr + 0x009aa270;
			*(uintptr_t*)&this->FUN_009AFCA0 = baseAddr + 0x009b0b70;
			*(uintptr_t*)&this->FUN_009C4000 = baseAddr + 0x009c4ed0;
			*(uintptr_t*)&this->FUN_009C4170 = baseAddr + 0x009c5040;
			*(uintptr_t*)&this->FUN_009C3280 = baseAddr + 0x009c4150;
			*(uintptr_t*)&this->FUN_002834C0 = baseAddr + 0x00283500;
			*(uintptr_t*)&this->FUN_009C3510 = baseAddr + 0x009c43e0;
			*(uintptr_t*)&this->FUN_009429B0 = baseAddr + 0x00943880;
			*(uintptr_t*)&this->FUN_008F47E0 = baseAddr + 0x008f56b0;
			*(uintptr_t*)&this->FUN_008F4200 = baseAddr + 0x008f50d0;
			*(uintptr_t*)&this->FUN_005375D0 = baseAddr + 0x00538060;
			*(uintptr_t*)&this->FUN_00531B70 = baseAddr + 0x00532600;
			*(uintptr_t*)&this->FUN_00536C00 = baseAddr + 0x00537690;
			*(uintptr_t*)&this->FUN_000B70A0 = baseAddr + 0x000b70e0;
			*(uintptr_t*)&this->FUN_005BC680 = baseAddr + 0x005bd110;
			*(uintptr_t*)&this->FUN_0084A9C0 = baseAddr + 0x0084b890;
			*(uintptr_t*)&this->FUN_00845060 = baseAddr + 0x00845f30;
			*(uintptr_t*)&this->FUN_00844C90 = baseAddr + 0x00845b60;
			*(uintptr_t*)&this->FUN_008DBF80 = baseAddr + 0x008dce50;
			*(uintptr_t*)&this->FUN_008DC0D0 = baseAddr + 0x008dcfa0;
			*(uintptr_t*)&this->FUN_00958550 = baseAddr + 0x00959420;
			*(uintptr_t*)&this->FUN_004FDE00 = baseAddr + 0x004fe890;
			*(uintptr_t*)&this->FUN_0095A340 = baseAddr + 0x0095b210;
			*(uintptr_t*)&this->FUN_002B7860 = baseAddr + 0x002b78a0;
			*(uintptr_t*)&this->FUN_00371480 = baseAddr + 0x003715c0;
			*(uintptr_t*)&this->FUN_009FD8D0 = baseAddr + 0x009fe7a0;
			*(uintptr_t*)&this->FUN_000D5FD0 = baseAddr + 0x000d6010;
			*(uintptr_t*)&this->FUN_000A9580 = baseAddr + 0x000a95c0;
			*(uintptr_t*)&this->FUN_00830b90 = baseAddr + 0x00831a60;
			*(uintptr_t*)&this->FUN_00406B90 = baseAddr + 0x00406f30;
			*(uintptr_t*)&this->FUN_003FB250 = baseAddr + 0x003fb570;
			*(uintptr_t*)&this->FUN_006FDE20 = baseAddr + 0x006fe9f0;
			*(uintptr_t*)&this->FUN_006FE210 = baseAddr + 0x006fede0;
			*(uintptr_t*)&this->FUN_006FE880 = baseAddr + 0x006ff450;
			*(uintptr_t*)&this->FUN_006FE2D0 = baseAddr + 0x006feea0;
			*(uintptr_t*)&this->FUN_0073F4B0 = baseAddr + 0x0073ffe0;
			*(uintptr_t*)&this->FUN_006F57E0 = baseAddr + 0x006f63b0;
			*(uintptr_t*)&this->FUN_006F6340 = baseAddr + 0x006f6f10;
			*(uintptr_t*)&this->FUN_003F0120 = baseAddr + 0x003f0260;
			*(uintptr_t*)&this->FUN_003EC950 = baseAddr + 0x003eca90;
			*(uintptr_t*)&this->FUN_0048EFA0 = baseAddr + 0x0048fa30;
			*(uintptr_t*)&this->FUN_0048EA30 = baseAddr + 0x0048f4c0;
			*(uintptr_t*)&this->FUN_005065E0 = baseAddr + 0x00507070;
			*(uintptr_t*)&this->FUN_002715E0 = baseAddr + 0x00271620;
			*(uintptr_t*)&this->FUN_0050C720 = baseAddr + 0x0050d1b0;
			*(uintptr_t*)&this->FUN_005067F0 = baseAddr + 0x00507280;
			*(uintptr_t*)&this->FUN_004AB930 = baseAddr + 0x004ac3c0;
			*(uintptr_t*)&this->FUN_00927900 = baseAddr + 0x009287d0;
			*(uintptr_t*)&this->FUN_0029E770 = baseAddr + 0x0029e7b0;
			*(uintptr_t*)&this->FUN_0029F940 = baseAddr + 0x0029f980;
			*(uintptr_t*)&this->FUN_002A5F00 = baseAddr + 0x002a5f40;
			*(uintptr_t*)&this->FUN_002A5710 = baseAddr + 0x002a5750;
			*(uintptr_t*)&this->FUN_0029C810 = baseAddr + 0x0029c850;
			*(uintptr_t*)&this->FUN_00299750 = baseAddr + 0x00299790;
			*(uintptr_t*)&this->FUN_005A3B60 = baseAddr + 0x005a45f0;
			*(uintptr_t*)&this->FUN_00340EB0 = baseAddr + 0x00340f10;
			*(uintptr_t*)&this->FUN_00343720 = baseAddr + 0x00343780;
			*(uintptr_t*)&this->FUN_001A4FB0 = baseAddr + 0x001a4ff0;
			*(uintptr_t*)&this->FUN_000CE290 = baseAddr + 0x000ce2d0;
			*(uintptr_t*)&this->FUN_00954340 = baseAddr + 0x00955210;
			*(uintptr_t*)&this->FUN_006BEAC0 = baseAddr + 0x006bee10;
			*(uintptr_t*)&this->FUN_0052B1E0 = baseAddr + 0x0052bc70;
			*(uintptr_t*)&this->FUN_0007F0B0 = baseAddr + 0x0007f0f0;
			*(uintptr_t*)&this->FUN_0053E320 = baseAddr + 0x0053edb0;
			*(uintptr_t*)&this->FUN_0006F8F0 = baseAddr + 0x0006f930;
			*(uintptr_t*)&this->FUN_005B97F0 = baseAddr + 0x005ba280;
			*(uintptr_t*)&this->FUN_00413FE0 = baseAddr + 0x00414380;
			*(uintptr_t*)&this->FUN_006B3460 = baseAddr + 0x006b37b0;
			*(uintptr_t*)&this->FUN_006B4A70 = baseAddr + 0x006b4dc0;
			*(uintptr_t*)&this->FUN_0051DA50 = baseAddr + 0x0051e4e0;
			*(uintptr_t*)&this->FUN_005B7240 = baseAddr + 0x005b7cd0;
			*(uintptr_t*)&this->FUN_005B7030 = baseAddr + 0x005b7ac0;
			*(uintptr_t*)&this->FUN_005B3020 = baseAddr + 0x005b3ab0;
			*(uintptr_t*)&this->FUN_005B33B0 = baseAddr + 0x005b3e40;
			*(uintptr_t*)&this->FUN_005B3380 = baseAddr + 0x005b3e10;
			*(uintptr_t*)&this->FUN_00155350 = baseAddr + 0x00155220;
			*(uintptr_t*)&this->FUN_0015D810 = baseAddr + 0x0015d850;
			*(uintptr_t*)&this->FUN_0011DDC0 = baseAddr + 0x0011de00;
			*(uintptr_t*)&this->FUN_0011CD50 = baseAddr + 0x0011cd90;
			*(uintptr_t*)&this->FUN_00119C00 = baseAddr + 0x00119c40;
			*(uintptr_t*)&this->FUN_0014F530 = baseAddr + 0x0014f570;
			*(uintptr_t*)&this->FUN_0014E390 = baseAddr + 0x0014e3d0;
			*(uintptr_t*)&this->FUN_0070C0A0 = baseAddr + 0x0070cc70;
			*(uintptr_t*)&this->FUN_0070C0B0 = baseAddr + 0x0070cc80;
			*(uintptr_t*)&this->FUN_000CD830 = baseAddr + 0x000cd870;
			*(uintptr_t*)&this->FUN_00A09840 = baseAddr + 0x00a0a710;
			*(uintptr_t*)&this->FUN_000D8620 = baseAddr + 0x000d8660;
			*(uintptr_t*)&this->FUN_000D6360 = baseAddr + 0x000d63a0;
			*(uintptr_t*)&this->FUN_000D8310 = baseAddr + 0x000d8350;
			*(uintptr_t*)&this->FUN_004B0B60 = baseAddr + 0x004b15f0;
			*(uintptr_t*)&this->FUN_002C9840 = baseAddr + 0x002c9880;
			*(uintptr_t*)&this->FUN_002BB270 = baseAddr + 0x002bb2b0;
			*(uintptr_t*)&this->FUN_002CE000 = baseAddr + 0x002ce040;
			*(uintptr_t*)&this->FUN_002BBCE0 = baseAddr + 0x002bbd20;
			*(uintptr_t*)&this->FUN_002B8260 = baseAddr + 0x002b82a0;
			*(uintptr_t*)&this->FUN_0082E2E0 = baseAddr + 0x0082f1b0;
			*(uintptr_t*)&this->FUN_002CA350 = baseAddr + 0x002ca390;
			*(uintptr_t*)&this->FUN_00912CA0 = baseAddr + 0x00913b70;
		}
	}
	else if (platform == 0)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&this->FUN_00071920 = baseAddr + 0x71920;
			*(uintptr_t*)&this->FUN_00071A80 = baseAddr + 0x71a80;
			*(uintptr_t*)&this->FUN_0007B780 = baseAddr + 0x7b780;
			*(uintptr_t*)&this->FUN_0007C030 = baseAddr + 0x7c030;
			*(uintptr_t*)&this->FUN_008C4F20 = baseAddr + 0x8c4640;
			*(uintptr_t*)&this->FUN_0043BDE0 = baseAddr + 0x43ba00;
			*(uintptr_t*)&this->FUN_00354500 = baseAddr + 0x3540b0;
			*(uintptr_t*)&this->FUN_00505D50 = baseAddr + 0x506060;
			*(uintptr_t*)&this->FUN_00591720 = baseAddr + 0x591a30;
			*(uintptr_t*)&this->FUN_005E7D60 = baseAddr + 0x5e8070;
			*(uintptr_t*)&this->FUN_009A7AA0 = baseAddr + 0x9a71c0;
			*(uintptr_t*)&this->FUN_009A93A0 = baseAddr + 0x9a8ac0;
			*(uintptr_t*)&this->FUN_009AFCA0 = baseAddr + 0x9af3c0;
			*(uintptr_t*)&this->FUN_009C4000 = baseAddr + 0x9c3720;
			*(uintptr_t*)&this->FUN_009C4170 = baseAddr + 0x9c3890;
			*(uintptr_t*)&this->FUN_009C3280 = baseAddr + 0x9c29a0;
			*(uintptr_t*)&this->FUN_002834C0 = baseAddr + 0x283050;
			*(uintptr_t*)&this->FUN_009C3510 = baseAddr + 0x9c2c30;
			*(uintptr_t*)&this->FUN_009429B0 = baseAddr + 0x9420d0;
			*(uintptr_t*)&this->FUN_008F47E0 = baseAddr + 0x8f3f00;
			*(uintptr_t*)&this->FUN_008F4200 = baseAddr + 0x8f3920;
			*(uintptr_t*)&this->FUN_005375D0 = baseAddr + 0x5378e0;
			*(uintptr_t*)&this->FUN_00531B70 = baseAddr + 0x531e80;
			*(uintptr_t*)&this->FUN_00536C00 = baseAddr + 0x536f10;
			*(uintptr_t*)&this->FUN_000B70A0 = baseAddr + 0xb70a0;
			*(uintptr_t*)&this->FUN_005BC680 = baseAddr + 0x5bc990;
			*(uintptr_t*)&this->FUN_0084A9C0 = baseAddr + 0x84a300;
			*(uintptr_t*)&this->FUN_00845060 = baseAddr + 0x8449a0;
			*(uintptr_t*)&this->FUN_00844C90 = baseAddr + 0x8445d0;
			*(uintptr_t*)&this->FUN_008DBF80 = baseAddr + 0x8db6a0;
			*(uintptr_t*)&this->FUN_008DC0D0 = baseAddr + 0x8db7f0;
			*(uintptr_t*)&this->FUN_00958550 = baseAddr + 0x957c70;
			*(uintptr_t*)&this->FUN_004FDE00 = baseAddr + 0x4fe110;
			*(uintptr_t*)&this->FUN_0095A340 = baseAddr + 0x959a60;
			*(uintptr_t*)&this->FUN_002B7860 = baseAddr + 0x2b73f0;
			*(uintptr_t*)&this->FUN_00371480 = baseAddr + 0x3710a0;
			*(uintptr_t*)&this->FUN_009FD8D0 = baseAddr + 0x9fcff0;
			*(uintptr_t*)&this->FUN_000D5FD0 = baseAddr + 0xd5fd0;
			*(uintptr_t*)&this->FUN_000A9580 = baseAddr + 0xa9580;
			*(uintptr_t*)&this->FUN_00830b90 = baseAddr + 0x8304d0;
			*(uintptr_t*)&this->FUN_00406B90 = baseAddr + 0x4067b0;
			*(uintptr_t*)&this->FUN_003FB250 = baseAddr + 0x3fae70;
			*(uintptr_t*)&this->FUN_006FDE20 = baseAddr + 0x6fd780;
			*(uintptr_t*)&this->FUN_006FE210 = baseAddr + 0x6fdb70;
			*(uintptr_t*)&this->FUN_006FE880 = baseAddr + 0x6fe1e0;
			*(uintptr_t*)&this->FUN_006FE2D0 = baseAddr + 0x6fdc30;
			*(uintptr_t*)&this->FUN_0073F4B0 = baseAddr + 0x73ee10;
			*(uintptr_t*)&this->FUN_006F57E0 = baseAddr + 0x6f5140;
			*(uintptr_t*)&this->FUN_006F6340 = baseAddr + 0x6f5ca0;
			*(uintptr_t*)&this->FUN_003F0120 = baseAddr + 0x3efd40;
			*(uintptr_t*)&this->FUN_003EC950 = baseAddr + 0x3ec570;
			*(uintptr_t*)&this->FUN_0048EFA0 = baseAddr + 0x48f2b0;
			*(uintptr_t*)&this->FUN_0048EA30 = baseAddr + 0x48ed40;
			*(uintptr_t*)&this->FUN_005065E0 = baseAddr + 0x5068f0;
			*(uintptr_t*)&this->FUN_002715E0 = baseAddr + 0x271170;
			*(uintptr_t*)&this->FUN_0050C720 = baseAddr + 0x50ca30;
			*(uintptr_t*)&this->FUN_005067F0 = baseAddr + 0x506b00;
			*(uintptr_t*)&this->FUN_004AB930 = baseAddr + 0x4abc40;
			*(uintptr_t*)&this->FUN_00927900 = baseAddr + 0x927020;
			*(uintptr_t*)&this->FUN_0029E770 = baseAddr + 0x29e300;
			*(uintptr_t*)&this->FUN_0029F940 = baseAddr + 0x29f4d0;
			*(uintptr_t*)&this->FUN_002A5F00 = baseAddr + 0x2a5a90;
			*(uintptr_t*)&this->FUN_002A5710 = baseAddr + 0x2a52a0;
			*(uintptr_t*)&this->FUN_0029C810 = baseAddr + 0x29c3a0;
			*(uintptr_t*)&this->FUN_00299750 = baseAddr + 0x2992e0;
			*(uintptr_t*)&this->FUN_005A3B60 = baseAddr + 0x5a3e70;
			*(uintptr_t*)&this->FUN_00340EB0 = baseAddr + 0x340a60;
			*(uintptr_t*)&this->FUN_00343720 = baseAddr + 0x3432d0;
			*(uintptr_t*)&this->FUN_001A4FB0 = baseAddr + 0x1a4b40;
			*(uintptr_t*)&this->FUN_000CE290 = baseAddr + 0xce290;
			*(uintptr_t*)&this->FUN_00954340 = baseAddr + 0x953a60;
			*(uintptr_t*)&this->FUN_006BEAC0 = baseAddr + 0x6be3d0;
			*(uintptr_t*)&this->FUN_0052B1E0 = baseAddr + 0x52b4f0;
			*(uintptr_t*)&this->FUN_0007F0B0 = baseAddr + 0x7f0b0;
			*(uintptr_t*)&this->FUN_0053E320 = baseAddr + 0x53e630;
			*(uintptr_t*)&this->FUN_0006F8F0 = baseAddr + 0x6f8f0;
			*(uintptr_t*)&this->FUN_005B97F0 = baseAddr + 0x5b9b00;
			*(uintptr_t*)&this->FUN_00413FE0 = baseAddr + 0x413c00;
			*(uintptr_t*)&this->FUN_006B3460 = baseAddr + 0x6b2d70;
			*(uintptr_t*)&this->FUN_006B4A70 = baseAddr + 0x6b4380;
			*(uintptr_t*)&this->FUN_0051DA50 = baseAddr + 0x51dd60;
			*(uintptr_t*)&this->FUN_005B7240 = baseAddr + 0x5b7550;
			*(uintptr_t*)&this->FUN_005B7030 = baseAddr + 0x5b7340;
			*(uintptr_t*)&this->FUN_005B3020 = baseAddr + 0x5b3330;
			*(uintptr_t*)&this->FUN_005B33B0 = baseAddr + 0x5b36c0;
			*(uintptr_t*)&this->FUN_005B3380 = baseAddr + 0x5b3690;
			*(uintptr_t*)&this->FUN_00155350 = baseAddr + 0x155270;
			*(uintptr_t*)&this->FUN_0015D810 = baseAddr + 0x15d900;
			*(uintptr_t*)&this->FUN_0011DDC0 = baseAddr + 0x11ddc0;
			*(uintptr_t*)&this->FUN_0011CD50 = baseAddr + 0x11cd50;
			*(uintptr_t*)&this->FUN_00119C00 = baseAddr + 0x119c00;
			*(uintptr_t*)&this->FUN_0014F530 = baseAddr + 0x14f450;
			*(uintptr_t*)&this->FUN_0014E390 = baseAddr + 0x14e2b0;
			*(uintptr_t*)&this->FUN_0070C0A0 = baseAddr + 0x70ba00;
			*(uintptr_t*)&this->FUN_0070C0B0 = baseAddr + 0x70ba10;
			*(uintptr_t*)&this->FUN_000CD830 = baseAddr + 0xcd830;
			*(uintptr_t*)&this->FUN_00A09840 = baseAddr + 0xa08f60;
			*(uintptr_t*)&this->FUN_000D8620 = baseAddr + 0xd8620;
			*(uintptr_t*)&this->FUN_000D6360 = baseAddr + 0xd6360;
			*(uintptr_t*)&this->FUN_000D8310 = baseAddr + 0xd8310;
			*(uintptr_t*)&this->FUN_004B0B60 = baseAddr + 0x4b0e70;
			*(uintptr_t*)&this->FUN_002C9840 = baseAddr + 0x2c93d0;
			*(uintptr_t*)&this->FUN_002BB270 = baseAddr + 0x2bae00;
			*(uintptr_t*)&this->FUN_002CE000 = baseAddr + 0x2cdb90;
			*(uintptr_t*)&this->FUN_002BBCE0 = baseAddr + 0x2bb870;
			*(uintptr_t*)&this->FUN_002B8260 = baseAddr + 0x2b7df0;
			*(uintptr_t*)&this->FUN_0082E2E0 = baseAddr + 0x82dc20;
			*(uintptr_t*)&this->FUN_002CA350 = baseAddr + 0x2c9ee0;
			*(uintptr_t*)&this->FUN_00912CA0 = baseAddr + 0x9123c0;
		}
		else if (version == "1.0.68")
		{
			*(uintptr_t*)&this->FUN_00071920 = baseAddr + 0x00071940;
			*(uintptr_t*)&this->FUN_00071A80 = baseAddr + 0x00071aa0;
			*(uintptr_t*)&this->FUN_0007B780 = baseAddr + 0x0007b7a0;
			*(uintptr_t*)&this->FUN_0007C030 = baseAddr + 0x0007c050;
			*(uintptr_t*)&this->FUN_008C4F20 = baseAddr + 0x008c5530;
			*(uintptr_t*)&this->FUN_0043BDE0 = baseAddr + 0x0043bb20;
			*(uintptr_t*)&this->FUN_00354500 = baseAddr + 0x00353f00;
			*(uintptr_t*)&this->FUN_00505D50 = baseAddr + 0x00506180;
			*(uintptr_t*)&this->FUN_00591720 = baseAddr + 0x00591b50;
			*(uintptr_t*)&this->FUN_005E7D60 = baseAddr + 0x005e8190;
			*(uintptr_t*)&this->FUN_009A7AA0 = baseAddr + 0x009a80b0;
			*(uintptr_t*)&this->FUN_009A93A0 = baseAddr + 0x009a99b0;
			*(uintptr_t*)&this->FUN_009AFCA0 = baseAddr + 0x009b02b0;
			*(uintptr_t*)&this->FUN_009C4000 = baseAddr + 0x009c4610;
			*(uintptr_t*)&this->FUN_009C4170 = baseAddr + 0x009c4780;
			*(uintptr_t*)&this->FUN_009C3280 = baseAddr + 0x009c3890;
			*(uintptr_t*)&this->FUN_002834C0 = baseAddr + 0x00282ea0;
			*(uintptr_t*)&this->FUN_009C3510 = baseAddr + 0x009c3b20;
			*(uintptr_t*)&this->FUN_009429B0 = baseAddr + 0x00942fc0;
			*(uintptr_t*)&this->FUN_008F47E0 = baseAddr + 0x008f4df0;
			*(uintptr_t*)&this->FUN_008F4200 = baseAddr + 0x008f4810;
			*(uintptr_t*)&this->FUN_005375D0 = baseAddr + 0x00537a00;
			*(uintptr_t*)&this->FUN_00531B70 = baseAddr + 0x00531fa0;
			*(uintptr_t*)&this->FUN_00536C00 = baseAddr + 0x00537030;
			*(uintptr_t*)&this->FUN_000B70A0 = baseAddr + 0x000b70c0;
			*(uintptr_t*)&this->FUN_005BC680 = baseAddr + 0x005bcab0;
			*(uintptr_t*)&this->FUN_0084A9C0 = baseAddr + 0x0084b1f0;
			*(uintptr_t*)&this->FUN_00845060 = baseAddr + 0x00845890;
			*(uintptr_t*)&this->FUN_00844C90 = baseAddr + 0x008454c0;
			*(uintptr_t*)&this->FUN_008DBF80 = baseAddr + 0x008dc590;
			*(uintptr_t*)&this->FUN_008DC0D0 = baseAddr + 0x008dc6e0;
			*(uintptr_t*)&this->FUN_00958550 = baseAddr + 0x00958b60;
			*(uintptr_t*)&this->FUN_004FDE00 = baseAddr + 0x004fe230;
			*(uintptr_t*)&this->FUN_0095A340 = baseAddr + 0x0095a950;
			*(uintptr_t*)&this->FUN_002B7860 = baseAddr + 0x002b7240;
			*(uintptr_t*)&this->FUN_00371480 = baseAddr + 0x00370f60;
			*(uintptr_t*)&this->FUN_009FD8D0 = baseAddr + 0x009fdee0;
			*(uintptr_t*)&this->FUN_000D5FD0 = baseAddr + 0x000d5ff0;
			*(uintptr_t*)&this->FUN_000A9580 = baseAddr + 0x000a95a0;
			*(uintptr_t*)&this->FUN_00830b90 = baseAddr + 0x008313c0;
			*(uintptr_t*)&this->FUN_00406B90 = baseAddr + 0x004068d0;
			*(uintptr_t*)&this->FUN_003FB250 = baseAddr + 0x003faf10;
			*(uintptr_t*)&this->FUN_006FDE20 = baseAddr + 0x006fe390;
			*(uintptr_t*)&this->FUN_006FE210 = baseAddr + 0x006fe780;
			*(uintptr_t*)&this->FUN_006FE880 = baseAddr + 0x006fedf0;
			*(uintptr_t*)&this->FUN_006FE2D0 = baseAddr + 0x006fe840;
			*(uintptr_t*)&this->FUN_0073F4B0 = baseAddr + 0x0073f980;
			*(uintptr_t*)&this->FUN_006F57E0 = baseAddr + 0x006f5d50;
			*(uintptr_t*)&this->FUN_006F6340 = baseAddr + 0x006f68b0;
			*(uintptr_t*)&this->FUN_003F0120 = baseAddr + 0x003efc00;
			*(uintptr_t*)&this->FUN_003EC950 = baseAddr + 0x003ec430;
			*(uintptr_t*)&this->FUN_0048EFA0 = baseAddr + 0x0048f3d0;
			*(uintptr_t*)&this->FUN_0048EA30 = baseAddr + 0x0048ee60;
			*(uintptr_t*)&this->FUN_005065E0 = baseAddr + 0x00506a10;
			*(uintptr_t*)&this->FUN_002715E0 = baseAddr + 0x00270fc0;
			*(uintptr_t*)&this->FUN_0050C720 = baseAddr + 0x0050cb50;
			*(uintptr_t*)&this->FUN_005067F0 = baseAddr + 0x00506c20;
			*(uintptr_t*)&this->FUN_004AB930 = baseAddr + 0x004abd60;
			*(uintptr_t*)&this->FUN_00927900 = baseAddr + 0x00927f10;
			*(uintptr_t*)&this->FUN_0029E770 = baseAddr + 0x0029e150;
			*(uintptr_t*)&this->FUN_0029F940 = baseAddr + 0x0029f320;
			*(uintptr_t*)&this->FUN_002A5F00 = baseAddr + 0x002a58e0;
			*(uintptr_t*)&this->FUN_002A5710 = baseAddr + 0x002a50f0;
			*(uintptr_t*)&this->FUN_0029C810 = baseAddr + 0x0029c1f0;
			*(uintptr_t*)&this->FUN_00299750 = baseAddr + 0x00299130;
			*(uintptr_t*)&this->FUN_005A3B60 = baseAddr + 0x005a3f90;
			*(uintptr_t*)&this->FUN_00340EB0 = baseAddr + 0x003408b0;
			*(uintptr_t*)&this->FUN_00343720 = baseAddr + 0x00343120;
			*(uintptr_t*)&this->FUN_001A4FB0 = baseAddr + 0x001a4990;
			*(uintptr_t*)&this->FUN_000CE290 = baseAddr + 0x000ce2b0;
			*(uintptr_t*)&this->FUN_00954340 = baseAddr + 0x00954950;
			*(uintptr_t*)&this->FUN_006BEAC0 = baseAddr + 0x006be7b0;
			*(uintptr_t*)&this->FUN_0052B1E0 = baseAddr + 0x0052b610;
			*(uintptr_t*)&this->FUN_0007F0B0 = baseAddr + 0x0007f0d0;
			*(uintptr_t*)&this->FUN_0053E320 = baseAddr + 0x0053e750;
			*(uintptr_t*)&this->FUN_0006F8F0 = baseAddr + 0x0006f910;
			*(uintptr_t*)&this->FUN_005B97F0 = baseAddr + 0x005b9c20;
			*(uintptr_t*)&this->FUN_00413FE0 = baseAddr + 0x00413d20;
			*(uintptr_t*)&this->FUN_006B3460 = baseAddr + 0x006b3150;
			*(uintptr_t*)&this->FUN_006B4A70 = baseAddr + 0x006b4760;
			*(uintptr_t*)&this->FUN_0051DA50 = baseAddr + 0x0051de80;
			*(uintptr_t*)&this->FUN_005B7240 = baseAddr + 0x005b7670;
			*(uintptr_t*)&this->FUN_005B7030 = baseAddr + 0x005b7460;
			*(uintptr_t*)&this->FUN_005B3020 = baseAddr + 0x005b3450;
			*(uintptr_t*)&this->FUN_005B33B0 = baseAddr + 0x005b37e0;
			*(uintptr_t*)&this->FUN_005B3380 = baseAddr + 0x005b37b0;
			*(uintptr_t*)&this->FUN_00155350 = baseAddr + 0x00155120;
			*(uintptr_t*)&this->FUN_0015D810 = baseAddr + 0x0015d750;
			*(uintptr_t*)&this->FUN_0011DDC0 = baseAddr + 0x0011dde0;
			*(uintptr_t*)&this->FUN_0011CD50 = baseAddr + 0x0011cd70;
			*(uintptr_t*)&this->FUN_00119C00 = baseAddr + 0x00119c20;
			*(uintptr_t*)&this->FUN_0014F530 = baseAddr + 0x0014f470;
			*(uintptr_t*)&this->FUN_0014E390 = baseAddr + 0x0014e2d0;
			*(uintptr_t*)&this->FUN_0070C0A0 = baseAddr + 0x0070c610;
			*(uintptr_t*)&this->FUN_0070C0B0 = baseAddr + 0x0070c620;
			*(uintptr_t*)&this->FUN_000CD830 = baseAddr + 0x000cd850;
			*(uintptr_t*)&this->FUN_00A09840 = baseAddr + 0x00a09e50;
			*(uintptr_t*)&this->FUN_000D8620 = baseAddr + 0x000d8640;
			*(uintptr_t*)&this->FUN_000D6360 = baseAddr + 0x000d6380;
			*(uintptr_t*)&this->FUN_000D8310 = baseAddr + 0x000d8330;
			*(uintptr_t*)&this->FUN_004B0B60 = baseAddr + 0x004b0f90;
			*(uintptr_t*)&this->FUN_002C9840 = baseAddr + 0x002c9220;
			*(uintptr_t*)&this->FUN_002BB270 = baseAddr + 0x002bac50;
			*(uintptr_t*)&this->FUN_002CE000 = baseAddr + 0x002cd9e0;
			*(uintptr_t*)&this->FUN_002BBCE0 = baseAddr + 0x002bb6c0;
			*(uintptr_t*)&this->FUN_002B8260 = baseAddr + 0x002b7c40;
			*(uintptr_t*)&this->FUN_0082E2E0 = baseAddr + 0x0082eb10;
			*(uintptr_t*)&this->FUN_002CA350 = baseAddr + 0x002c9d30;
			*(uintptr_t*)&this->FUN_00912CA0 = baseAddr + 0x009132b0;
		}
	}

	auto hOgreMain = GetModuleHandleA("OgreMain_x64.dll");
	*(uintptr_t*)&target_Ogre_Skeleton__refreshAnimationState = reinterpret_cast<uintptr_t>(GetProcAddress(hOgreMain, "?_refreshAnimationState@Skeleton@Ogre@@UEAAXPEAVAnimationStateSet@2@@Z"));
}
