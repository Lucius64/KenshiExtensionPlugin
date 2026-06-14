#pragma once
#include "common.h"

namespace KEP
{
	class DECLSPEC VersionInfo
	{
	public:
		VersionInfo();
		VersionInfo(int _major, int _minor, int _micro);

		bool operator<(const VersionInfo& other) const;
		bool operator>(const VersionInfo& other) const;
		bool operator<=(const VersionInfo& other) const;
		bool operator>=(const VersionInfo& other) const;
		bool operator==(const VersionInfo& other) const;
		bool operator!=(const VersionInfo& other) const;

		std::string toString() const;

	private:
		int major;
		int minor;
		int micro;
	};

	DECLSPEC const VersionInfo& getKenshiLibVersion();
}
