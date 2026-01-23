#include "driver.h"

namespace {
	DWORD getPid(const wchar_t* p_name) {
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

	std::uintptr_t getBase(DWORD pid, const wchar_t* m_name) {
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

namespace driver {
	Driver::Driver(const std::wstring name, const std::wstring path)
		: _name(name), _path(path)
	{}

	Driver::~Driver() {
		if (_device != INVALID_HANDLE_VALUE)
			CloseHandle(_device);
	}

	bool Driver::init() {
		if (!exists()) create();
		start();

		_device = CreateFileW(
			KMD_DEVICE_NAME,
			GENERIC_READ,
			0,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);

		return _device != INVALID_HANDLE_VALUE;
	}

	bool Driver::attach(const wchar_t* p_name, const wchar_t* m_name) {
		_pid = getPid(p_name);
		if (!_pid) return false;
		if (m_name != nullptr)
			_b_addr = getBase(_pid, m_name);

		kmd::request req{};
		req.process_id = reinterpret_cast<HANDLE>(_pid);

		return DeviceIoControl(
			_device,
			kmd::ioctl::attach,
			&req,
			sizeof(req),
			&req,
			sizeof(req),
			nullptr,
			nullptr
		);
	}

	bool Driver::exists() {
		SC_HANDLE scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
		if (!scm) return false;

		SC_HANDLE svc = OpenServiceW(scm, _name.c_str(), SERVICE_QUERY_STATUS);
		bool exists = (svc != nullptr);

		if (svc) CloseServiceHandle(svc);
		CloseServiceHandle(scm);
		return exists;
	}

	bool Driver::create() {
		SC_HANDLE scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
	    if (!scm) return false;
	
	    SC_HANDLE svc = CreateServiceW(
	        scm,
	        _name.c_str(),
	        _name.c_str(),
	        SERVICE_START | DELETE | SERVICE_STOP,
	        SERVICE_KERNEL_DRIVER,
	        SERVICE_DEMAND_START,
	        SERVICE_ERROR_IGNORE,
	        _path.c_str(),
	        nullptr, nullptr, nullptr, nullptr, nullptr
	    );
	
	    if (!svc && GetLastError() != ERROR_SERVICE_EXISTS) {
	        CloseServiceHandle(scm);
	        return false;
	    }
	
	    if (svc) CloseServiceHandle(svc);
	    CloseServiceHandle(scm);
	    return true;
	}
	
	bool Driver::start() {
		SC_HANDLE scm = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
		if (!scm) return false;
		
		SC_HANDLE svc = OpenServiceW(scm, _name.c_str(), SERVICE_START);
		if (!svc) {
		    CloseServiceHandle(scm);
		    return false;
		}
		
		bool st = StartServiceW(svc, 0, nullptr) || GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;
		
		CloseServiceHandle(svc);
		CloseServiceHandle(scm);
		return st;
	}

}