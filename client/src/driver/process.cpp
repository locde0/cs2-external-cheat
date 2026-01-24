#include "process.h"
#include <TlHelp32.h>
#include <cwchar>

namespace driver {

	DWORD findPid(const wchar_t* p_name) {
		DWORD pid = 0;

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (snapshot == INVALID_HANDLE_VALUE)
			return pid;

		PROCESSENTRY32W entry = {};
		entry.dwSize = sizeof(decltype(entry));

		if (Process32FirstW(snapshot, &entry) == TRUE) {
			if (_wcsicmp(p_name, entry.szExeFile) == 0)
				pid = entry.th32ProcessID;
			else {
				while (Process32NextW(snapshot, &entry) == TRUE) {
					if (_wcsicmp(p_name, entry.szExeFile) == 0) {
						pid = entry.th32ProcessID;
						break;
					}
				}
			}
		}

		CloseHandle(snapshot);
		return pid;
	}

	std::uintptr_t findModuleBase(DWORD pid, const wchar_t* m_name) {
		std::uintptr_t base = 0;

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
		if (snapshot == INVALID_HANDLE_VALUE)
			return base;

		MODULEENTRY32W entry = {};
		entry.dwSize = sizeof(decltype(entry));

		if (Module32FirstW(snapshot, &entry) == TRUE) {
			if (wcsstr(m_name, entry.szModule) != nullptr)
				base = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
			else {
				while (Module32NextW(snapshot, &entry) == TRUE) {
					if (wcsstr(m_name, entry.szModule) != nullptr) {
						base = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
						break;
					}
				}
			}
		}

		CloseHandle(snapshot);
		return base;
	}

}