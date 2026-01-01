#pragma once
#include<extern/UseableStuff.h>

class InventorySection;

// KenshiLibでは未定義
class StorageBuilding : public UseableStuff
{
public:
	class ConsumptionItem
	{
	public:
		float amount;
		float _0x4;
		int numStack;
		GameData* data;
		InventorySection* section;

		ConsumptionItem() : amount(0.0f), _0x4(1.0f), numStack(1), data(nullptr), section(nullptr) {}
	};

	itemType itemtypeLimit;
	bool flags;
	ConsumptionItem* outputItems;
	lektor<ConsumptionItem*> storageItems;

	virtual ~StorageBuilding();
};