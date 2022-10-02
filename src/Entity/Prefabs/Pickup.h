#ifndef PICKUP_H
#define PICKUP_H

#include "vec2.h"
#include "SpritesheetPropertiesComponent.h"
#include "PickupComponent.h"

#include <cstdint>

using Entity = std::uint16_t;

namespace prefab {
    class Pickup {
    public:
        Pickup() = default;
        ~Pickup() = default;

        static Entity create();
        static Entity create(strb::vec2 pos, PickupType pickupType);

    private:
        static SpritesheetPropertiesComponent createSpritesheetPropertiesComponent(Spritesheet* spritesheet);

    };
}

#endif