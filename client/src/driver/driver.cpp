#include "driver.h"

namespace driver {

	Driver::Driver(const std::wstring name, const std::wstring path)
		: _name(name), _path(path)
	{}

	bool Driver::init() {
		if (!exists()) create();
		start();

		_device.reset(CreateFileW(
			KMD_DEVICE_NAME,
			GENERIC_READ,
			0,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		));

		return _device.valid();
	}

	bool Driver::attach(const wchar_t* p_name, const wchar_t* m_name) {
		_pid = driver::findPid(p_name);
		if (!_pid) return false;

		if (m_name != nullptr)
			_b_addr = driver::findModuleBase(_pid, m_name);

		kmd::request req{};
		req.process_id = reinterpret_cast<HANDLE>(static_cast<uintptr_t>(_pid));

		return DeviceIoControl(
			_device.get(),
			kmd::ioctl::attach,
			&req, sizeof(req), &req, sizeof(req),
			nullptr, nullptr
		);
	}

	bool Driver::exists() {
		core::scoped::ScopedScHandle scm(OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT));
		if (!scm) return false;

		core::scoped::ScopedScHandle svc(OpenServiceW(scm.get(), _name.c_str(), SERVICE_QUERY_STATUS));

		return svc;
	}

	bool Driver::create() {
		core::scoped::ScopedScHandle scm(OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE));
		if (!scm) return false;

		core::scoped::ScopedScHandle svc(CreateServiceW(
			scm.get(),
			_name.c_str(),
			_name.c_str(),
			SERVICE_START | DELETE | SERVICE_STOP,
			SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_IGNORE,
			_path.c_str(),
			nullptr, nullptr, nullptr, nullptr, nullptr
		));

		if (!svc && GetLastError() != ERROR_SERVICE_EXISTS)
			return false;

		return true;
	}
	
	bool Driver::start() {
		core::scoped::ScopedScHandle scm(OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT));
		if (!scm) return false;

		core::scoped::ScopedScHandle svc(OpenServiceW(scm.get(), _name.c_str(), SERVICE_START));
		if (!svc) return false;

		return StartServiceW(svc.get(), 0, nullptr) || GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;
	}

}