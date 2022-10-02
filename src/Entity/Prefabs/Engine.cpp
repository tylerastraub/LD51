#include "Engine.h"

#include "EntityRegistry.h"
#include "SpritesheetRegistry.h"
// Components
#include "PhysicsComponent.h"
#include "ScriptComponent.h"
#include "StateComponent.h"
#include "DirectionComponent.h"
#include "RenderComponent.h"
#include "SpritesheetPropertiesComponent.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "AnimationComponent.h"
#include "HealthComponent.h"
#include "EdgeCheckComponent.h"
#include "EnemyComponent.h"

namespace {
    class EngineScript : public IScript {
    public:
        EngineScript() = default;
        ~EngineScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            auto& physics = ecs->getComponent<PhysicsComponent>(owner);
            auto& dir = ecs->getComponent<DirectionComponent>(owner);
            auto& collision = ecs->getComponent<CollisionComponent>(owner);
            auto& edgeCheck = ecs->getComponent<EdgeCheckComponent>(owner);
            auto& state = ecs->getComponent<StateComponent>(owner);
            state.state = EntityState::RUNNING;

            if(collision.collidingRight) {
                dir.direction = Direction::WEST;
            }
            else if(collision.collidingLeft) {
                dir.direction = Direction::EAST;
            }
            else if(edgeCheck.onLeftEdge && dir.direction == Direction::WEST) {
                dir.direction = Direction::EAST;
            }
            else if(edgeCheck.onRightEdge && dir.direction == Direction::EAST) {
                dir.direction = Direction::WEST;
            }
            
            if(dir.direction == Direction::WEST) {
                float acceleration = (physics.touchingGround) ? physics.acceleration.x : physics.airAcceleration.x;
                // check if acceleration is just past max velocity
                if(physics.velocity.x - acceleration < physics.maxVelocity.x * -1 &&
                physics.velocity.x - acceleration > physics.maxVelocity.x * -1 - acceleration) {
                    acceleration = physics.maxVelocity.x - physics.velocity.x * -1;
                } 
                if(physics.velocity.x >= physics.maxVelocity.x * -1) physics.velocity.x -= acceleration;
            }
            else if(dir.direction == Direction::EAST) {
                float acceleration = (physics.touchingGround) ? physics.acceleration.x : physics.airAcceleration.x;
                // check if acceleration is just past max velocity
                if(physics.velocity.x + acceleration > physics.maxVelocity.x &&
                physics.velocity.x + acceleration < physics.maxVelocity.x + acceleration) {
                    acceleration = physics.maxVelocity.x - physics.velocity.x;
                }
                if(physics.velocity.x <= physics.maxVelocity.x) physics.velocity.x += acceleration;
            }
        }

    private:

    };
}

namespace prefab {
    Entity Engine::create() {
        return create({0.f, 0.f});
    }

    Entity Engine::create(strb::vec2 pos) {
        auto ecs = EntityRegistry::getInstance();
        Entity ent = ecs->createEntity();

        PhysicsComponent physics;
        physics.airAcceleration = {5.f, 0.f};
        physics.acceleration = {15.f, 0.f};
        physics.maxVelocity = {50.f, 300.f};
        physics.jumpPower = 0.f;
        physics.frictionCoefficient = 10.f;
        physics.airFrictionCoefficient = 5.f;

        ecs->addComponent<PhysicsComponent>(ent, physics);

        RenderComponent render;
        render.renderQuad = {0, 0, 16, 16};
        
        ecs->addComponent<RenderComponent>(ent, render);

        CollisionComponent collision;
        collision.collisionRect = {0, 0, 15, 15};
        collision.collisionRectOffset = {1, 1};

        ecs->addComponent<CollisionComponent>(ent, collision);

        ecs->addComponent<SpritesheetPropertiesComponent>(
            ent,
            createSpritesheetPropertiesComponent(SpritesheetRegistry::getSpritesheet(SpritesheetID::ENGINE_SPRITESHEET))
            );

        ecs->addComponent<AnimationComponent>(ent, AnimationComponent{});
        ecs->addComponent<DirectionComponent>(ent, DirectionComponent{Direction::EAST});
        ecs->addComponent<StateComponent>(ent, StateComponent{EntityState::RUNNING});
        ecs->addComponent<TransformComponent>(ent, {pos, pos});
        ecs->addComponent<ScriptComponent>(ent, ScriptComponent{std::make_shared<EngineScript>()});
        ecs->addComponent<HealthComponent>(ent, HealthComponent{3});
        ecs->addComponent<EdgeCheckComponent>(ent, EdgeCheckComponent{});
        ecs->addComponent<EnemyComponent>(ent, EnemyComponent{});

        return ent;
    }

    SpritesheetPropertiesComponent Engine::createSpritesheetPropertiesComponent(Spritesheet* spritesheet) {
        SpritesheetPropertiesComponent props;
        props.spritesheet = spritesheet;
        
        SpritesheetProperties runEast = {
            0, // xTileIndex
            0, // yTileIndex
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
            1, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_RUN_FRAMES, // numOfFrames
            MS_BETWEEN_RUN_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        props.addSpritesheetProperties(EntityState::RUNNING, Direction::WEST, runWest);

        return props;
    }
};