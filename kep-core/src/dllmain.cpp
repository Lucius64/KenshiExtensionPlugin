#include "pch.h"
#include <boost/filesystem.hpp>

#include <Debug.h>
#include <kep.h>

namespace fs = boost::filesystem;

namespace
{
	std::string kepDirectory;
}

const std::string& KEP::getPluginPath()
{
	return kepDirectory;
}

__declspec(dllexport) void startPlugin()
{
	DebugLog("kep-core 1.0.2");

	fs::path modulePath = kepDirectory;
	if (!fs::exists(modulePath / "LICENSE") || !fs::exists(modulePath / "NOTICE.md"))
	{
		ErrorLog("License file not found.");
		return;
	}

	KEP::coreOptions.init(kepDirectory + "\\kep_core_settings.json");
	if (!KEP::coreOptions.getEnablePlugin())
	{
		DebugLog("This plugin has been disabled in the settings. path: " + kepDirectory + "\\kep_core_settings.json");
		return;
	}

	KEP::initialize();
	KEP::ConfigManager::getSingleton().addItem(&KEP::coreOptions);
	DebugLog("Installed");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DWORD bufSize = _MAX_PATH;
		std::vector<TCHAR> buf(bufSize + 1, 0);
		if (GetModuleFileNameW(hModule, buf.data(), bufSize) != 0)
		{
			kepDirectory = fs::path(buf.data()).branch_path().string();
		}
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			bufSize = MAXSHORT;
			buf.resize(static_cast<size_t>(bufSize) + 1);
			if (GetModuleFileNameW(hModule, buf.data(), bufSize) != 0)
			{
				kepDirectory = fs::path(buf.data()).branch_path().string();
			}
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
