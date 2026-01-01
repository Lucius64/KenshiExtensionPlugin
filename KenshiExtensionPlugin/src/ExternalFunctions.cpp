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
	, _dropItemReturnAddress(0)
{

}

void KEP::ExternalGlobals::init(unsigned int platform, const std::string& version, uintptr_t baseAddr)
{
	if (platform == 1)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&this->_gBaseXpCombat = baseAddr + 0x2133e7c;
			*(uintptr_t*)&this->_optionsAdvanced = baseAddr + 0x2132528;
			*(uintptr_t*)&this->_playerMorphCounter = baseAddr + 0x2126c80;
			*(uintptr_t*)&this->_KenshiFolder = baseAddr + 0x1f9ee38;
			*(uintptr_t*)&this->_UtilityT = baseAddr + 0x2134b10;
			*(uintptr_t*)&this->_CLASS_02133098 = baseAddr + 0x2133098;
			*(uintptr_t*)&this->_CLASS_021322B0 = baseAddr + 0x21322b0;
			*(uintptr_t*)&this->_hand_01E395F8 = baseAddr + 0x1e395f8;
			*(uintptr_t*)&this->_MainColorCode = baseAddr + 0x1f48238;
			*(uintptr_t*)&this->_SecondarColorCode = baseAddr + 0x1f48260;
			*(uintptr_t*)&this->_TitleColorCode = baseAddr + 0x1f48288;
			*(uintptr_t*)&this->_BadColorCode = baseAddr + 0x1f482b0;
			*(uintptr_t*)&this->_BadBrightColorCode = baseAddr + 0x1f482d8;
			*(uintptr_t*)&this->_GoodColorCode = baseAddr + 0x1f48300;
			*(uintptr_t*)&this->_GoodBrightColorCode = baseAddr + 0x1f48328;
			*(uintptr_t*)&this->_GreyedColorCode = baseAddr + 0x1f48350;
			*(uintptr_t*)&this->_GreyedBrightColorCode = baseAddr + 0x1f48378;
			*(uintptr_t*)&this->_SpecialColorCode = baseAddr + 0x1f483a0;
			*(uintptr_t*)&this->_NameTagSelectedColor = baseAddr + 0x2131aa0;
			*(uintptr_t*)&this->_NameTagUnselectedColor = baseAddr + 0x2131ab0;
			*(uintptr_t*)&this->_NameTagStealthHiddenColor = baseAddr + 0x2131ac0;
			*(uintptr_t*)&this->_NameTagStealthAlmostSeenColor = baseAddr + 0x2131ad0;
			*(uintptr_t*)&this->_NameTagStealthSeenColor = baseAddr + 0x2131ae0;
			*(uintptr_t*)&this->_DialogueBubbleTextColor = baseAddr + 0x2131c00;
			*(uintptr_t*)&this->_KenshiGUI = baseAddr + 0x2132750;
			*(uintptr_t*)&this->_LocaleInfoManagerPtr = baseAddr + 0x2127370;
			*(uintptr_t*)&this->_TemporaryLimbInterface = baseAddr + 0x2126cd0;
			this->_dropItemReturnAddress = baseAddr + 0x712c49;
		}
	}
	else if(platform == 0)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&this->_gBaseXpCombat = baseAddr + 0x2131dec;
			*(uintptr_t*)&this->_optionsAdvanced = baseAddr + 0x2130498;
			*(uintptr_t*)&this->_playerMorphCounter = baseAddr + 0x2124c90;
			*(uintptr_t*)&this->_KenshiFolder = baseAddr + 0x1f9ce48;
			*(uintptr_t*)&this->_UtilityT = baseAddr + 0x2132a80;
			*(uintptr_t*)&this->_CLASS_02133098 = baseAddr + 0x2131008;
			*(uintptr_t*)&this->_CLASS_021322B0 = baseAddr + 0x2130220;
			*(uintptr_t*)&this->_hand_01E395F8 = baseAddr + 0x1e375f8;
			*(uintptr_t*)&this->_MainColorCode = baseAddr + 0x1f46248;
			*(uintptr_t*)&this->_SecondarColorCode = baseAddr + 0x1f46270;
			*(uintptr_t*)&this->_TitleColorCode = baseAddr + 0x1f46298;
			*(uintptr_t*)&this->_BadColorCode = baseAddr + 0x1f462c0;
			*(uintptr_t*)&this->_BadBrightColorCode = baseAddr + 0x1f462e8;
			*(uintptr_t*)&this->_GoodColorCode = baseAddr + 0x1f46310;
			*(uintptr_t*)&this->_GoodBrightColorCode = baseAddr + 0x1f46338;
			*(uintptr_t*)&this->_GreyedColorCode = baseAddr + 0x1f46360;
			*(uintptr_t*)&this->_GreyedBrightColorCode = baseAddr + 0x1f46388;
			*(uintptr_t*)&this->_SpecialColorCode = baseAddr + 0x1f463b0;
			*(uintptr_t*)&this->_NameTagSelectedColor = baseAddr + 0x212fa10;
			*(uintptr_t*)&this->_NameTagUnselectedColor = baseAddr + 0x212fa20;
			*(uintptr_t*)&this->_NameTagStealthHiddenColor = baseAddr + 0x4212fa30;
			*(uintptr_t*)&this->_NameTagStealthAlmostSeenColor = baseAddr + 0x212fa40;
			*(uintptr_t*)&this->_NameTagStealthSeenColor = baseAddr + 0x212fa50;
			*(uintptr_t*)&this->_DialogueBubbleTextColor = baseAddr + 0x212fb70;
			*(uintptr_t*)&this->_KenshiGUI = baseAddr + 0x21306c0;
			*(uintptr_t*)&this->_LocaleInfoManagerPtr = baseAddr + 0x2125380;
			*(uintptr_t*)&this->_TemporaryLimbInterface = baseAddr + 0x2124ce0;
			this->_dropItemReturnAddress = baseAddr + 0x7125a9;
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
	, FUN_005A3B60(nullptr)
	, FUN_00340EB0(nullptr)
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
			*(uintptr_t*)&this->FUN_005A3B60 = baseAddr + 0x5a3b60;
			*(uintptr_t*)&this->FUN_00340EB0 = baseAddr + 0x340eb0;
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
			*(uintptr_t*)&this->FUN_005A3B60 = baseAddr + 0x5a3e70;
			*(uintptr_t*)&this->FUN_00340EB0 = baseAddr + 0x340a60;
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
		}
	}

	auto hOgreMain = GetModuleHandleA("OgreMain_x64.dll");
	*(uintptr_t*)&target_Ogre_Skeleton__refreshAnimationState = reinterpret_cast<uintptr_t>(GetProcAddress(hOgreMain, "?_refreshAnimationState@Skeleton@Ogre@@UEAAXPEAVAnimationStateSet@2@@Z"));
}
