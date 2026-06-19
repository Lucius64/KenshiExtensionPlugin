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
	, _TemporaryLimbInterface(nullptr)
	, _InventorySlotSkinSize(nullptr)
	, _ScreenSize(nullptr)
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
			*(uintptr_t*)&this->_TemporaryLimbInterface        = baseAddr + 0x02126ce0;
			*(uintptr_t*)&this->_InventorySlotSkinSize         = baseAddr + 0x02127450;
			this->_dropItemReturnAddress                       = baseAddr + 0x007131b9;
		}
	}
}

KEP::ExternalFunctions::ExternalFunctions()
	: FUN_00354500(nullptr)
	, FUN_00505D50(nullptr)
	, FUN_00591720(nullptr)
	, FUN_005E7D60(nullptr)
	, FUN_009A7AA0(nullptr)
	, FUN_009A93A0(nullptr)
	, FUN_009AFCA0(nullptr)
	, FUN_009C4000(nullptr)
	, FUN_009C3510(nullptr)
	, FUN_009429B0(nullptr)
	, FUN_008F4200(nullptr)
	, FUN_00844C90(nullptr)
	, FUN_008DBF80(nullptr)
	, FUN_008DC0D0(nullptr)
	, FUN_000D5FD0(nullptr)
	, FUN_00830B90(nullptr)
	, FUN_002715E0(nullptr)
	, FUN_00340EB0(nullptr)
	, FUN_00343720(nullptr)
	, FUN_001A4FB0(nullptr)
	, FUN_00954340(nullptr)
	, FUN_00155350(nullptr)
	, FUN_0014F530(nullptr)
	, FUN_000D8620(nullptr)
	, FUN_000D6360(nullptr)
	, FUN_000D8310(nullptr)
	, FUN_004B0B60(nullptr)
	, FUN_002C9840(nullptr)
	, FUN_0082E2E0(nullptr)
	, FUN_002CA350(nullptr)
	, FUN_0079EAB0(nullptr)
{

}

void KEP::ExternalFunctions::init(unsigned int platform, const std::string& version, uintptr_t baseAddr)
{

	if (platform == 1)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&this->FUN_00354500 = baseAddr + 0x354500;
			*(uintptr_t*)&this->FUN_00505D50 = baseAddr + 0x505d50;
			*(uintptr_t*)&this->FUN_00591720 = baseAddr + 0x591720;
			*(uintptr_t*)&this->FUN_005E7D60 = baseAddr + 0x5e7d60;
			*(uintptr_t*)&this->FUN_009A7AA0 = baseAddr + 0x9a7aa0;
			*(uintptr_t*)&this->FUN_009A93A0 = baseAddr + 0x9a93a0;
			*(uintptr_t*)&this->FUN_009AFCA0 = baseAddr + 0x9afca0;
			*(uintptr_t*)&this->FUN_009C4000 = baseAddr + 0x9c4000;
			*(uintptr_t*)&this->FUN_009C3510 = baseAddr + 0x9c3510;
			*(uintptr_t*)&this->FUN_009429B0 = baseAddr + 0x9429b0;
			*(uintptr_t*)&this->FUN_008F4200 = baseAddr + 0x8f4200;
			*(uintptr_t*)&this->FUN_00844C90 = baseAddr + 0x844c90;
			*(uintptr_t*)&this->FUN_008DBF80 = baseAddr + 0x8dbf80;
			*(uintptr_t*)&this->FUN_008DC0D0 = baseAddr + 0x8dc0d0;
			*(uintptr_t*)&this->FUN_000D5FD0 = baseAddr + 0xd5fd0;
			*(uintptr_t*)&this->FUN_00830B90 = baseAddr + 0x830b90;
			*(uintptr_t*)&this->FUN_002715E0 = baseAddr + 0x2715e0;
			*(uintptr_t*)&this->FUN_00340EB0 = baseAddr + 0x340eb0;
			*(uintptr_t*)&this->FUN_00343720 = baseAddr + 0x343720;
			*(uintptr_t*)&this->FUN_001A4FB0 = baseAddr + 0x1a4fb0;
			*(uintptr_t*)&this->FUN_00954340 = baseAddr + 0x954340;
			*(uintptr_t*)&this->FUN_00155350 = baseAddr + 0x155350;
			*(uintptr_t*)&this->FUN_0014F530 = baseAddr + 0x14f530;
			*(uintptr_t*)&this->FUN_000D8620 = baseAddr + 0xd8620;
			*(uintptr_t*)&this->FUN_000D6360 = baseAddr + 0xd6360;
			*(uintptr_t*)&this->FUN_000D8310 = baseAddr + 0xd8310;
			*(uintptr_t*)&this->FUN_004B0B60 = baseAddr + 0x4b0b60;
			*(uintptr_t*)&this->FUN_002C9840 = baseAddr + 0x2c9840;
			*(uintptr_t*)&this->FUN_0082E2E0 = baseAddr + 0x82e2e0;
			*(uintptr_t*)&this->FUN_002CA350 = baseAddr + 0x2ca350;
			*(uintptr_t*)&this->FUN_0079EAB0 = baseAddr + 0x79eab0;
		}
		else if (version == "1.0.68")
		{
			*(uintptr_t*)&this->FUN_00354500 = baseAddr + 0x00354560;
			*(uintptr_t*)&this->FUN_00505D50 = baseAddr + 0x005067e0;
			*(uintptr_t*)&this->FUN_00591720 = baseAddr + 0x005921b0;
			*(uintptr_t*)&this->FUN_005E7D60 = baseAddr + 0x005e87f0;
			*(uintptr_t*)&this->FUN_009A7AA0 = baseAddr + 0x009a8970;
			*(uintptr_t*)&this->FUN_009A93A0 = baseAddr + 0x009aa270;
			*(uintptr_t*)&this->FUN_009AFCA0 = baseAddr + 0x009b0b70;
			*(uintptr_t*)&this->FUN_009C4000 = baseAddr + 0x009c4ed0;
			*(uintptr_t*)&this->FUN_009C3510 = baseAddr + 0x009c43e0;
			*(uintptr_t*)&this->FUN_009429B0 = baseAddr + 0x00943880;
			*(uintptr_t*)&this->FUN_008F4200 = baseAddr + 0x008f50d0;
			*(uintptr_t*)&this->FUN_00844C90 = baseAddr + 0x00845b60;
			*(uintptr_t*)&this->FUN_008DBF80 = baseAddr + 0x008dce50;
			*(uintptr_t*)&this->FUN_008DC0D0 = baseAddr + 0x008dcfa0;
			*(uintptr_t*)&this->FUN_000D5FD0 = baseAddr + 0x000d6010;
			*(uintptr_t*)&this->FUN_00830B90 = baseAddr + 0x00831a60;
			*(uintptr_t*)&this->FUN_002715E0 = baseAddr + 0x00271620;
			*(uintptr_t*)&this->FUN_00340EB0 = baseAddr + 0x00340f10;
			*(uintptr_t*)&this->FUN_00343720 = baseAddr + 0x00343780;
			*(uintptr_t*)&this->FUN_001A4FB0 = baseAddr + 0x001a4ff0;
			*(uintptr_t*)&this->FUN_00954340 = baseAddr + 0x00955210;
			*(uintptr_t*)&this->FUN_00155350 = baseAddr + 0x00155220;
			*(uintptr_t*)&this->FUN_0014F530 = baseAddr + 0x0014f570;
			*(uintptr_t*)&this->FUN_000D8620 = baseAddr + 0x000d8660;
			*(uintptr_t*)&this->FUN_000D6360 = baseAddr + 0x000d63a0;
			*(uintptr_t*)&this->FUN_000D8310 = baseAddr + 0x000d8350;
			*(uintptr_t*)&this->FUN_004B0B60 = baseAddr + 0x004b15f0;
			*(uintptr_t*)&this->FUN_002C9840 = baseAddr + 0x002c9880;
			*(uintptr_t*)&this->FUN_0082E2E0 = baseAddr + 0x0082f1b0;
			*(uintptr_t*)&this->FUN_002CA350 = baseAddr + 0x002ca390;
			*(uintptr_t*)&this->FUN_0079EAB0 = baseAddr + 0x0079f650;
		}
	}
	else if (platform == 0)
	{
		if (version == "1.0.65")
		{
			*(uintptr_t*)&this->FUN_00354500 = baseAddr + 0x3540b0;
			*(uintptr_t*)&this->FUN_00505D50 = baseAddr + 0x506060;
			*(uintptr_t*)&this->FUN_00591720 = baseAddr + 0x591a30;
			*(uintptr_t*)&this->FUN_005E7D60 = baseAddr + 0x5e8070;
			*(uintptr_t*)&this->FUN_009A7AA0 = baseAddr + 0x9a71c0;
			*(uintptr_t*)&this->FUN_009A93A0 = baseAddr + 0x9a8ac0;
			*(uintptr_t*)&this->FUN_009AFCA0 = baseAddr + 0x9af3c0;
			*(uintptr_t*)&this->FUN_009C4000 = baseAddr + 0x9c3720;
			*(uintptr_t*)&this->FUN_009C3510 = baseAddr + 0x9c2c30;
			*(uintptr_t*)&this->FUN_009429B0 = baseAddr + 0x9420d0;
			*(uintptr_t*)&this->FUN_008F4200 = baseAddr + 0x8f3920;
			*(uintptr_t*)&this->FUN_00844C90 = baseAddr + 0x8445d0;
			*(uintptr_t*)&this->FUN_008DBF80 = baseAddr + 0x8db6a0;
			*(uintptr_t*)&this->FUN_008DC0D0 = baseAddr + 0x8db7f0;
			*(uintptr_t*)&this->FUN_000D5FD0 = baseAddr + 0xd5fd0;
			*(uintptr_t*)&this->FUN_00830B90 = baseAddr + 0x8304d0;
			*(uintptr_t*)&this->FUN_002715E0 = baseAddr + 0x271170;
			*(uintptr_t*)&this->FUN_00340EB0 = baseAddr + 0x340a60;
			*(uintptr_t*)&this->FUN_00343720 = baseAddr + 0x3432d0;
			*(uintptr_t*)&this->FUN_001A4FB0 = baseAddr + 0x1a4b40;
			*(uintptr_t*)&this->FUN_00954340 = baseAddr + 0x953a60;
			*(uintptr_t*)&this->FUN_00155350 = baseAddr + 0x155270;
			*(uintptr_t*)&this->FUN_0014F530 = baseAddr + 0x14f450;
			*(uintptr_t*)&this->FUN_000D8620 = baseAddr + 0xd8620;
			*(uintptr_t*)&this->FUN_000D6360 = baseAddr + 0xd6360;
			*(uintptr_t*)&this->FUN_000D8310 = baseAddr + 0xd8310;
			*(uintptr_t*)&this->FUN_004B0B60 = baseAddr + 0x4b0e70;
			*(uintptr_t*)&this->FUN_002C9840 = baseAddr + 0x2c93d0;
			*(uintptr_t*)&this->FUN_0082E2E0 = baseAddr + 0x82dc20;
			*(uintptr_t*)&this->FUN_002CA350 = baseAddr + 0x2c9ee0;
			*(uintptr_t*)&this->FUN_0079EAB0 = baseAddr + 0x79e3f0;
		}
		else if (version == "1.0.68")
		{
			*(uintptr_t*)&this->FUN_00354500 = baseAddr + 0x00353f00;
			*(uintptr_t*)&this->FUN_00505D50 = baseAddr + 0x00506180;
			*(uintptr_t*)&this->FUN_00591720 = baseAddr + 0x00591b50;
			*(uintptr_t*)&this->FUN_005E7D60 = baseAddr + 0x005e8190;
			*(uintptr_t*)&this->FUN_009A7AA0 = baseAddr + 0x009a80b0;
			*(uintptr_t*)&this->FUN_009A93A0 = baseAddr + 0x009a99b0;
			*(uintptr_t*)&this->FUN_009AFCA0 = baseAddr + 0x009b02b0;
			*(uintptr_t*)&this->FUN_009C4000 = baseAddr + 0x009c4610;
			*(uintptr_t*)&this->FUN_009C3510 = baseAddr + 0x009c3b20;
			*(uintptr_t*)&this->FUN_009429B0 = baseAddr + 0x00942fc0;
			*(uintptr_t*)&this->FUN_008F4200 = baseAddr + 0x008f4810;
			*(uintptr_t*)&this->FUN_00844C90 = baseAddr + 0x008454c0;
			*(uintptr_t*)&this->FUN_008DBF80 = baseAddr + 0x008dc590;
			*(uintptr_t*)&this->FUN_008DC0D0 = baseAddr + 0x008dc6e0;
			*(uintptr_t*)&this->FUN_000D5FD0 = baseAddr + 0x000d5ff0;
			*(uintptr_t*)&this->FUN_00830B90 = baseAddr + 0x008313c0;
			*(uintptr_t*)&this->FUN_002715E0 = baseAddr + 0x00270fc0;
			*(uintptr_t*)&this->FUN_00340EB0 = baseAddr + 0x003408b0;
			*(uintptr_t*)&this->FUN_00343720 = baseAddr + 0x00343120;
			*(uintptr_t*)&this->FUN_001A4FB0 = baseAddr + 0x001a4990;
			*(uintptr_t*)&this->FUN_00954340 = baseAddr + 0x00954950;
			*(uintptr_t*)&this->FUN_00155350 = baseAddr + 0x00155120;
			*(uintptr_t*)&this->FUN_0014F530 = baseAddr + 0x0014f470;
			*(uintptr_t*)&this->FUN_000D8620 = baseAddr + 0x000d8640;
			*(uintptr_t*)&this->FUN_000D6360 = baseAddr + 0x000d6380;
			*(uintptr_t*)&this->FUN_000D8310 = baseAddr + 0x000d8330;
			*(uintptr_t*)&this->FUN_004B0B60 = baseAddr + 0x004b0f90;
			*(uintptr_t*)&this->FUN_002C9840 = baseAddr + 0x002c9220;
			*(uintptr_t*)&this->FUN_0082E2E0 = baseAddr + 0x0082eb10;
			*(uintptr_t*)&this->FUN_002CA350 = baseAddr + 0x002c9d30;
			*(uintptr_t*)&this->FUN_0079EAB0 = baseAddr + 0x007b2380;
		}
	}
}
