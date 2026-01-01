#pragma once
#include <kenshi/RootObject.h>

// KenshiLibでは未定義
class TemporaryLimbInterface : public RootObject
{
public:
	Inventory* inventory;
	Character* owner;
	bool flag;

	virtual ~TemporaryLimbInterface();
};