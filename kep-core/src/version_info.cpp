#include "pch.h"
#include <ogre/OgreStringConverter.h>
#include <kep/version_info.h>

KEP::VersionInfo::VersionInfo()
	: major(0)
	, minor(0)
	, micro(0)
{
}

KEP::VersionInfo::VersionInfo(int _major, int _minor, int _micro)
	: major(_major)
	, minor(_minor)
	, micro(_micro)
{
}

bool KEP::VersionInfo::operator<(const VersionInfo& other) const
{
	if (major != other.major)
		return major < other.major;
	else if (minor != other.minor)
		return minor < other.minor;
	else
		return micro < other.micro;
}

bool KEP::VersionInfo::operator>(const VersionInfo& other) const
{
	return !(*this < other);
}

bool KEP::VersionInfo::operator<=(const VersionInfo& other) const
{
	if (major != other.major)
		return major <= other.major;
	else if (minor != other.minor)
		return minor <= other.minor;
	else
		return micro <= other.micro;
}

bool KEP::VersionInfo::operator>=(const VersionInfo& other) const
{
	return !(*this <= other);
}

bool KEP::VersionInfo::operator==(const VersionInfo& other) const
{
	return *this <= other && *this >= other;
}

bool KEP::VersionInfo::operator!=(const VersionInfo& other) const
{
	return !(*this == other);
}

std::string KEP::VersionInfo::toString() const
{
	return Ogre::StringConverter::toString(major) + "." + Ogre::StringConverter::toString(minor) + "." + Ogre::StringConverter::toString(micro);
}
