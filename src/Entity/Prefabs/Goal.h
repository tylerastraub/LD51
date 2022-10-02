#ifndef GOAL_H
#define GOAL_H

#include "vec2.h"
#include "SpritesheetPropertiesComponent.h"

#include <cstdint>

using Entity = std::uint16_t;

namespace prefab {
    class Goal {
    public:
        Goal() = default;
        ~Goal() = default;

        static Entity create();
        static Entity create(strb::vec2 pos);

    private:
        static SpritesheetPropertiesComponent createSpritesheetPropertiesComponent(Spritesheet* spritesheet);

    };
}

#endif