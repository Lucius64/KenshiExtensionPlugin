#pragma once
#include <kenshi/Item.h>

class BlueprintItem : public Item
{
public:
	int money;
	virtual ~BlueprintItem();
};