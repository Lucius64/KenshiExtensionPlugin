#pragma once
#include <extern/StorageBuilding.h>

// KenshiLibでは未定義
class ProductionBuilding : public StorageBuilding
{
public:
	int _0x468;
	float resourceMult;
	InventorySection* outputInventory;
	lektor<StorageBuilding::ConsumptionItem> inputItems;

	virtual ~ProductionBuilding();
};

// KenshiLibでは未定義
class FurnaceBuilding : public ProductionBuilding
{
public:
	bool _0x480;

	virtual ~FurnaceBuilding();
};