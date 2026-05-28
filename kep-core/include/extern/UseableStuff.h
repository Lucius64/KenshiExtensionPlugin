#pragma once
class UseableStuff;

class DoorLock
{
public:
	int level;
	float hardness;
	UseableStuff* stuff;
	float minTime;
	float maxTime;
	float pickkingTime;
	float waitingTime;
	bool locked;
	bool broken;

private:

};
