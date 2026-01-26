#pragma once
#include "../driver/driver.h"
#include "game/context.h"
#include "features/esp/esp.h"
#include "../core/types.h"

namespace render { 
	class DrawList; 
}

namespace domain {

    class Facade {
    public:
        Facade(driver::Driver&);

        void update();
        void build(render::DrawList&, const core::Extent&);

    private:
        game::Context _ctx;
        features::Esp _esp;
    };

}