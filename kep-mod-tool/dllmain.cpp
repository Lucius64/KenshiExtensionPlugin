#include "pch.h"
#include <boost/filesystem.hpp>

#include <Debug.h>
#include <kep/utility.h>
#include <kep/version_info.h>
#include <DevToolsPanel.h>
#include <utility.h>

namespace fs = boost::filesystem;

namespace
{
	std::string myDirectory;
}

__declspec(dllexport) void startPlugin()
{
	DebugLog("kep-mod-tool 0.1.0");

	auto& libVersion = KEP::getKenshiLibVersion();
	if (libVersion < KEP::VersionInfo(0, 3, 0))
	{
		ErrorLog("Please install KenshiLib version 0.3.0 or later.");
		return;
	}

	fs::path modulePath = myDirectory;
	if (!fs::exists(modulePath / "LICENSE") || !fs::exists(modulePath / "NOTICE.md"))
	{
		ErrorLog("License file not found.");
		return;
	}

	KEP::tools::initHook();
}

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DWORD bufSize = _MAX_PATH;
		std::vector<TCHAR> buf(bufSize + 1, 0);
		if (GetModuleFileNameW(hModule, buf.data(), bufSize) != 0)
			myDirectory = fs::path(buf.data()).branch_path().string();

		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			bufSize = MAXSHORT;
			buf.resize(static_cast<size_t>(bufSize) + 1);
			if (GetModuleFileNameW(hModule, buf.data(), bufSize) != 0)
				myDirectory = fs::path(buf.data()).branch_path().string();
		}
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

const std::string& KEP::tools::getModulePath()
{
	return myDirectory;
}
