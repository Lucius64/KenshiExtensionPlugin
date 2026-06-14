#pragma once
#include <kenshi/GameData.h>
#include <kep/translation.h>

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

	inline int getLevel(int rarity)
	{
		switch (rarity) {
		case 0:
			return 5;
		case 2:
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

	namespace tools
	{
		struct LessGameData
		{
			bool operator()(const GameData* a, const GameData* b) const
			{
				int result = KEP::TranslationUtility::getCollator().compare(boost::locale::collator_base::secondary, a->name, b->name);
				if (result != 0)
					return result < 0;
				return a->stringID < b->stringID;
			}
		};

		const std::string& getModulePath();
	}
}

