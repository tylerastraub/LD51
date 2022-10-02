#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "vec2.h"
#include "SpritesheetPropertiesComponent.h"

#include <cstdint>

using Entity = std::uint16_t;

namespace prefab {
    class Projectile {
    public:
        Projectile() = default;
        ~Projectile() = default;

        static Entity create();
        static Entity create(strb::vec2 pos, Direction shotDir);

    private:
        static SpritesheetPropertiesComponent createSpritesheetPropertiesComponent(Spritesheet* spritesheet);

        static const int NUM_OF_ACTIVE_FRAMES = 4;
        static const int MS_BETWEEN_ACTIVE_FRAMES = 100;

    };
}

#endif