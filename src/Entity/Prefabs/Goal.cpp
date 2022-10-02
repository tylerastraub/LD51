#include "Goal.h"
#include "EntityRegistry.h"
#include "SpritesheetRegistry.h"
// Components
#include "ScriptComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "StateComponent.h"
#include "DirectionComponent.h"
#include "GoalComponent.h"

namespace {
    class GoalOnActivatedScript : public IScript {
    public:
        GoalOnActivatedScript() = default;
        ~GoalOnActivatedScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            auto& state = ecs->getComponent<StateComponent>(owner);
            state.state = EntityState::ACTIVE;

            auto& goal = ecs->getComponent<GoalComponent>(owner);
            goal.activated = true;

            audio->playAudio(owner, AudioSound::GOAL, 1.f);
        }

    private:

    };
}

namespace prefab {
    Entity Goal::create() {
        return create({0.f, 0.f});
    }

    Entity Goal::create(strb::vec2 pos) {
        auto ecs = EntityRegistry::getInstance();
        Entity ent = ecs->createEntity();

        ecs->addComponent<GoalComponent>(
            ent,
            GoalComponent{std::make_shared<GoalOnActivatedScript>()}
        );

        RenderComponent render;
        render.renderQuad = {0, 0, 16, 16};
        
        ecs->addComponent<RenderComponent>(ent, render);

        CollisionComponent collision;
        collision.collisionRect = {0, 0, 10, 10};
        collision.collisionRectOffset = {3, 3};

        ecs->addComponent<CollisionComponent>(ent, collision);
        ecs->addComponent<DirectionComponent>(ent, DirectionComponent{Direction::EAST});
        ecs->addComponent<StateComponent>(ent, StateComponent{EntityState::IDLE});
        ecs->addComponent<TransformComponent>(ent, TransformComponent{pos, pos});

        ecs->addComponent<SpritesheetPropertiesComponent>(
            ent,
            createSpritesheetPropertiesComponent(SpritesheetRegistry::getSpritesheet(SpritesheetID::FLAG))
            );

        return ent;
    }

    SpritesheetPropertiesComponent Goal::createSpritesheetPropertiesComponent(Spritesheet* spritesheet) {
        SpritesheetPropertiesComponent props;
        props.spritesheet = spritesheet;
        
        SpritesheetProperties idleEast = {
            0, // xTileIndex
            0, // yTileIndex
            false, // isAnimated
            false, // isLooped
            1, // numOfFrames
            1, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        props.addSpritesheetProperties(EntityState::IDLE, Direction::EAST, idleEast);
        
        SpritesheetProperties activeEast = {
            0, // xTileIndex
            1, // yTileIndex
            false, // isAnimated
            false, // isLooped
            1, // numOfFrames
            1, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        props.addSpritesheetProperties(EntityState::ACTIVE, Direction::EAST, activeEast);

        return props;
    }
}