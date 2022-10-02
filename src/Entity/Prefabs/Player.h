#ifndef PLAYER_H
#define PLAYER_H

#include "vec2.h"
#include "SpritesheetPropertiesComponent.h"

#include <cstdint>

using Entity = std::uint16_t;

namespace prefab {
    class Player {
    public:
        Player() = default;
        ~Player() = default;

        static Entity create();
        static Entity create(strb::vec2 pos);

    private:
        static SpritesheetPropertiesComponent createSpritesheetPropertiesComponent(Spritesheet* spritesheet);

        static const int NUM_OF_IDLE_FRAMES = 6;
        static const int MS_BETWEEN_IDLE_FRAMES = 250;
        static const int NUM_OF_RUN_FRAMES = 6;
        static const int MS_BETWEEN_RUN_FRAMES = 100;

    };
}

#endif