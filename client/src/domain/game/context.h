#pragma once
#include <vector>
#include "schemes.h"
#include "offsets/offsets.h"
#include "../../driver/driver.h"

namespace game {

	class Context {
	public:
		explicit Context(driver::Driver&);

		void update();

		const LocalPlayer& local() const { return _local; }
		const std::vector<Entity>& entities() const { return _entities; }

	private:
		driver::Driver& _driver;
		offsets::Offsets& _off;

		LocalPlayer _local{};
		std::vector<Entity> _entities{};
	};

}