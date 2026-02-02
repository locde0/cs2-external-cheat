#include "driver.h"

namespace driver {

	bool Driver::init() {
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

	bool Driver::readBuf(const uintptr_t addr, void* buf, size_t size) {
		kmd::request r;
		r.target = reinterpret_cast<PVOID>(addr);
		r.buffer = buf;
		r.size = size;

		return DeviceIoControl(
			_device.get(),
			kmd::ioctl::read,
			&r, sizeof(r), &r, sizeof(r),
			nullptr, nullptr
		);
	}

}