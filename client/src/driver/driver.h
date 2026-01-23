#pragma once
#include <string>
#include "process.h"
#include "../core/scoped.h"
#include <ioctls.h>

namespace driver {
	class Driver {
	public:
		explicit Driver(const std::wstring, const std::wstring);

		Driver(const Driver&) = delete;
		Driver& operator=(const Driver&) = delete;

		bool init();
		bool attach(const wchar_t*, const wchar_t* m_name = nullptr);

		DWORD pid() const { return _pid; }
		uintptr_t b_addr() const { return _b_addr; }

		template <typename T>
		T read(const std::uintptr_t);

		template <typename T>
		void write(const std::uintptr_t, const T&);

	private:
		bool exists();
		bool create();
		bool start();

		std::wstring _name;
		std::wstring _path;
		uintptr_t _b_addr = 0;
		DWORD _pid = 0;
		core::ScopedHandle _device;
	};

	template <typename T>
	T Driver::read(const std::uintptr_t addr) {
		T temp = {};

		kmd::request r;
		r.target = addr;
		r.buffer = &temp;
		r.size = sizeof(T);

		DeviceIoControl(
			_device.get(),
			kmd::ioctl::read,
			&r, sizeof(r), &r, sizeof(r),
			nullptr, nullptr
		);

		return temp;
	}

	template <typename T>
	void Driver::write(const std::uintptr_t addr, const T& value) {
		kmd::request r;
		r.target = addr;
		r.buffer = &value;
		r.size = sizeof(T);

		DeviceIoControl(
			_device.get(),
			kmd::ioctl::write,
			&r, sizeof(r), &r, sizeof(r),
			nullptr, nullptr
		);
	}

}
