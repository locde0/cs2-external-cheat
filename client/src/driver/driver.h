#pragma once
#include <string>
#include "process.h"
#include "../core/scoped/scoped.h"
#include "../core/win.h"
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

		bool readBuf(const uintptr_t, void*, size_t);

		template <typename T>
		T read(const uintptr_t);

		template <typename T>
		void write(const uintptr_t, const T&);

	private:
		bool exists();
		bool create();
		bool start();

		std::wstring _name;
		std::wstring _path;
		uintptr_t _b_addr = 0;
		DWORD _pid = 0;
		core::scoped::ScopedHandle _device;
	};

	template <typename T>
	T Driver::read(const uintptr_t addr) {
		T temp = {};
		readBuf(addr, &temp, sizeof(T));
		return temp;
	}

	template <typename T>
	void Driver::write(const uintptr_t addr, const T& value) {
		kmd::request r;
		r.target = reinterpret_cast<PVOID>(addr);
		r.buffer = (PVOID)(&value);
		r.size = sizeof(T);

		DeviceIoControl(
			_device.get(),
			kmd::ioctl::write,
			&r, sizeof(r), &r, sizeof(r),
			nullptr, nullptr
		);
	}

}
