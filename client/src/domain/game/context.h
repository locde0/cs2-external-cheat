#pragma once
#include <vector>
#include "schemes.h"
#include "../../driver/driver.h"

namespace game {

	class Context {
	public:
		void update(driver::Driver& d);

		const LocalPlayer& local() const { return _local; }
		const std::vector<Entity>& entities() const { return _entities; }

	private:
		LocalPlayer _local;
		std::vector<Entity> _entities;
	};

}