#include <boost/functional/hash.hpp>

#include <kenshi/util/hand.h>
#include <kenshi/GameData.h>
#include <kenshi/GameDataManager.h>

#include <UtilityFunction.h>


std::size_t hash_value(const hand& h) {
	if (h.type == NULL_ITEM)
		return 0x9e3779c4;

	std::size_t seed = 0;
	boost::hash_combine(seed, h.container);
	boost::hash_combine(seed, h.index);
	boost::hash_combine(seed, h.serial);
	boost::hash_combine(seed, h.containerSerial);
	boost::hash_combine(seed, h.type);
	return seed;
}

/*
* KenshiLibからGameDataReferenceの定義が欠落しているので暫定的に追加する
*/

GameDataReference::GameDataReference(const GameDataReference& __that)
	: values(__that.values)
	, sid(__that.sid)
	, ptr(__that.ptr)
{

}

GameDataReference::GameDataReference(const std::string& _sid, TripleInt _values)
	: values(_values)
	, sid(_sid)
	, ptr(nullptr)
{

}

GameDataReference::GameDataReference()
	: values(TripleInt())
	, sid(std::string())
	, ptr(nullptr)
{

}

GameDataReference::~GameDataReference()
{

}

GameData* GameDataReference::getPtr(GameDataContainer* source) const
{
	if (this->ptr == nullptr)
	{
		auto newPtr = source->getData(this->sid);
		const_cast<GameDataReference*>(this)->ptr = newPtr;
		return newPtr;
	}
	return this->ptr;
}
