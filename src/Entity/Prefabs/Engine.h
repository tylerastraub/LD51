#ifndef ENGINE_H
#define ENGINE_H

#include "vec2.h"
#include "SpritesheetPropertiesComponent.h"

#include <cstdint>

using Entity = std::uint16_t;

namespace prefab {
    class Engine {
    public:
        Engine() = default;
        ~Engine() = default;

        static Entity create();
        static Entity create(strb::vec2 pos);

    private:
        static SpritesheetPropertiesComponent createSpritesheetPropertiesComponent(Spritesheet* spritesheet);

        static const int NUM_OF_RUN_FRAMES = 2;
        static const int MS_BETWEEN_RUN_FRAMES = 100;

    };
}

#endif