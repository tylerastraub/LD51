#include "Player.h"

#include "EntityRegistry.h"
#include "SpritesheetRegistry.h"
// Components
#include "PlayerComponent.h"
#include "PhysicsComponent.h"
#include "ScriptComponent.h"
#include "StateComponent.h"
#include "InputComponent.h"
#include "DirectionComponent.h"
#include "RenderComponent.h"
#include "SpritesheetPropertiesComponent.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "AnimationComponent.h"
#include "HealthComponent.h"
#include "WeaponComponent.h"

namespace {
    class PlayerScript : public IScript {
    public:
        PlayerScript() = default;
        ~PlayerScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            auto& state = ecs->getComponent<StateComponent>(owner);
            auto& physics = ecs->getComponent<PhysicsComponent>(owner);
            if(physics.touchingGround) {
                state.state = EntityState::IDLE;
            }
            else {
                state.state = EntityState::JUMPING;
            }

            auto& weapon = ecs->getComponent<WeaponComponent>(owner);
            weapon.timeSinceLastShot += timescale * 1000.f;
        }

    private:

    };
}

namespace prefab {
    Entity Player::create() {
        return create({0.f, 0.f});
    }

    Entity Player::create(strb::vec2 pos) {
        auto ecs = EntityRegistry::getInstance();
        Entity ent = ecs->createEntity();

        ecs->addComponent<PlayerComponent>(ent, PlayerComponent{});

        InputComponent input = {{
            InputEvent::LEFT,
            InputEvent::RIGHT
        }};
        ecs->addComponent<InputComponent>(ent, input);

        PhysicsComponent physics;
        physics.airAcceleration = {25.f, 0.f};
        physics.acceleration = {95.f, 0.f};
        physics.maxVelocity = {125.f, 300.f};
        physics.jumpPower = 260.f;
        physics.frictionCoefficient = 30.f;
        physics.airFrictionCoefficient = 5.f;

        ecs->addComponent<PhysicsComponent>(ent, physics);

        RenderComponent render;
        render.renderQuad = {0, 0, 24, 24};
        
        ecs->addComponent<RenderComponent>(ent, render);

        CollisionComponent collision;
        collision.collisionRect = {0, 0, 8, 20};
        collision.collisionRectOffset = {8, 4};

        ecs->addComponent<CollisionComponent>(ent, collision);

        ecs->addComponent<SpritesheetPropertiesComponent>(
            ent,
            createSpritesheetPropertiesComponent(SpritesheetRegistry::getSpritesheet(SpritesheetID::PLAYER_SPRITESHEET))
            );

        ecs->addComponent<AnimationComponent>(ent, AnimationComponent{});
        ecs->addComponent<DirectionComponent>(ent, DirectionComponent{Direction::EAST});
        ecs->addComponent<StateComponent>(ent, StateComponent{EntityState::IDLE});
        ecs->addComponent<TransformComponent>(ent, {pos, pos});
        ecs->addComponent<ScriptComponent>(ent, ScriptComponent{std::make_shared<PlayerScript>()});
        ecs->addComponent<HealthComponent>(ent, HealthComponent{1});

        return ent;
    }

    SpritesheetPropertiesComponent Player::createSpritesheetPropertiesComponent(Spritesheet* spritesheet) {
        SpritesheetPropertiesComponent props;
        props.spritesheet = spritesheet;
        
        SpritesheetProperties idleEast = {
            0, // xTileIndex
            0, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_IDLE_FRAMES, // numOfFrames
            MS_BETWEEN_IDLE_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        props.addSpritesheetProperties(EntityState::IDLE, Direction::EAST, idleEast);
        
        SpritesheetProperties idleWest = {
            0, // xTileIndex
            1, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_IDLE_FRAMES, // numOfFrames
            MS_BETWEEN_IDLE_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        props.addSpritesheetProperties(EntityState::IDLE, Direction::WEST, idleWest);
        
        SpritesheetProperties runEast = {
            0, // xTileIndex
            2, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_RUN_FRAMES, // numOfFrames
            MS_BETWEEN_RUN_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        props.addSpritesheetProperties(EntityState::RUNNING, Direction::EAST, runEast);
        
        SpritesheetProperties runWest = {
            0, // xTileIndex
            3, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_RUN_FRAMES, // numOfFrames
            MS_BETWEEN_RUN_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        props.addSpritesheetProperties(EntityState::RUNNING, Direction::WEST, runWest);
        
        SpritesheetProperties jumpEast = {
            0, // xTileIndex
            4, // yTileIndex
            false, // isAnimated
            false, // isLooped
            1, // numOfFrames
            1, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        props.addSpritesheetProperties(EntityState::JUMPING, Direction::EAST, jumpEast);
        
        SpritesheetProperties jumpWest = {
            0, // xTileIndex
            5, // yTileIndex
            false, // isAnimated
            false, // isLooped
            1, // numOfFrames
            1, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        props.addSpritesheetProperties(EntityState::JUMPING, Direction::WEST, jumpWest);

        return props;
    }
};