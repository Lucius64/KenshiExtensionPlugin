#pragma once
#include <kenshi/GameData.h>
#include <kep/translation.h>

namespace KEP
{
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

