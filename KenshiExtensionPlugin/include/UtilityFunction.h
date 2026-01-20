#pragma once
#include <sstream>

namespace KEP
{
	inline float lerp(float t, float a, float b)
	{
		return (b - a) * t + a;
	}

	inline float linear(float t, float zero, float one)
	{
		return (t - zero) / (one - zero);
	}

	inline std::string getHexString(size_t number, int width)
	{
		std::stringstream ss;
		ss.setf(std::stringstream::hex);
		ss << std::hex;
		ss.width(width);
		ss.fill('0');
		ss << number;
		return ss.str();
	}

	inline std::string getHexString(void* ptr)
	{
		return getHexString(reinterpret_cast<uintptr_t>(ptr), 0x10);
	}

	inline int getLevel(int rarity)
	{
		switch (rarity) {
		case 0:
			return 5;
		case 1:
			return 40;
		case 3:
			return 60;
		case 4:
			return 80;
		case 5:
			return 95;
		}
		return 20;
	}
}

class hand;
std::size_t hash_value(const hand& h);
