#include "Checkpoint.h"
#include "EntityRegistry.h"
#include "SpritesheetRegistry.h"
// Components
#include "ScriptComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "StateComponent.h"
#include "DirectionComponent.h"
#include "AnimationComponent.h"
#include "CheckpointComponent.h"

namespace {
    class CheckpointOnActivatedScript : public IScript {
    public:
        CheckpointOnActivatedScript() = default;
        ~CheckpointOnActivatedScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            auto& state = ecs->getComponent<StateComponent>(owner);
            state.state = EntityState::ACTIVE;
            audio->playAudio(owner, AudioSound::CHECKPOINT_ACTIVATED, 1.f);
        }

    private:

    };
}

namespace prefab {
    Entity Checkpoint::create() {
        return create({0.f, 0.f});
    }

    Entity Checkpoint::create(strb::vec2 pos) {
        auto ecs = EntityRegistry::getInstance();
        Entity ent = ecs->createEntity();

        ecs->addComponent<CheckpointComponent>(
            ent,
            CheckpointComponent{std::make_shared<CheckpointOnActivatedScript>()}
        );

        RenderComponent render;
        render.renderQuad = {0, 0, 16, 16};
        
        ecs->addComponent<RenderComponent>(ent, render);

        CollisionComponent collision;
        collision.collisionRect = {0, 0, 10, 10};
        collision.collisionRectOffset = {3, 3};

        ecs->addComponent<CollisionComponent>(ent, collision);
        ecs->addComponent<AnimationComponent>(ent, AnimationComponent{});
        ecs->addComponent<DirectionComponent>(ent, DirectionComponent{Direction::EAST});
        ecs->addComponent<StateComponent>(ent, StateComponent{EntityState::IDLE});
        ecs->addComponent<TransformComponent>(ent, TransformComponent{pos, pos});

        ecs->addComponent<SpritesheetPropertiesComponent>(
            ent,
            createSpritesheetPropertiesComponent(SpritesheetRegistry::getSpritesheet(SpritesheetID::CHECKPOINT))
            );

        return ent;
    }

    SpritesheetPropertiesComponent Checkpoint::createSpritesheetPropertiesComponent(Spritesheet* spritesheet) {
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
            true, // isAnimated
            true, // isLooped
            NUM_OF_ACTIVE_FRAMES, // numOfFrames
            MS_BETWEEN_ACTIVE_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        props.addSpritesheetProperties(EntityState::ACTIVE, Direction::EAST, activeEast);

        return props;
    }
}