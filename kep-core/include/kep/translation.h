#pragma once
#include <string>
#include <boost/locale.hpp>
#include <boost/locale/format.hpp>

#include "common.h"

namespace KEP
{
	class TranslationUtility
	{
	public:
		static void init(unsigned int platform, const std::string& version, uintptr_t baseAddr);
		static DECLSPEC const std::string& getLanguage();
		static DECLSPEC std::string gettext_main(const char* id);
		static DECLSPEC std::string gettext_main(const boost::locale::message& msg);
		static DECLSPEC std::string format_main(const boost::locale::format& fmt);
		static DECLSPEC std::string dgettext(const char* domain, const char* id);
		static DECLSPEC std::string gettext(const char* id);
		static DECLSPEC std::string str(const std::string& domain, const boost::locale::message& msg);
		static DECLSPEC std::string format(const boost::locale::format& fmt);
		static DECLSPEC const boost::locale::collator<char>& getCollator();
		static DECLSPEC void getMessagesPath(const std::string& domain, const std::string& language, std::string& filepath);
	private:

	};

}
