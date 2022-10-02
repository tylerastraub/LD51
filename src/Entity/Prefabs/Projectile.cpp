#include "Projectile.h"
#include "EntityRegistry.h"
#include "SpritesheetRegistry.h"
// Components
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "ProjectileComponent.h"
#include "CollisionComponent.h"
#include "AnimationComponent.h"

namespace prefab {
    Entity Projectile::create() {
        return create({0.f, 0.f}, Direction::EAST);
    }

    Entity Projectile::create(strb::vec2 pos, Direction shotDir) {
        auto ecs = EntityRegistry::getInstance();
        Entity ent = ecs->createEntity();

        RenderComponent render;
        render.renderQuad = {(int) pos.x, (int) pos.y, 8, 8};
        
        ecs->addComponent<RenderComponent>(ent, render);

        CollisionComponent collision;
        collision.collisionRect = {(int) pos.x, (int) pos.y, 4, 4};
        collision.collisionRectOffset = {4, 4};
        
        ecs->addComponent<CollisionComponent>(ent, collision);
        
        PhysicsComponent physics;
        physics.airAcceleration = {0.f, 0.f};
        physics.acceleration = {0.f, 0.f};
        physics.maxVelocity = {280.f, 280.f};
        physics.jumpPower = 0.f;
        physics.frictionCoefficient = 0.f;
        physics.airFrictionCoefficient = 0.f;
        physics.gravity = 0.f;
        physics.touchingGround = false;
        float coefficient = (shotDir == Direction::WEST) ? -1.f : 1.f;
        physics.velocity.x = 280.f * coefficient;

        ecs->addComponent<PhysicsComponent>(ent, physics);

        ecs->addComponent<AnimationComponent>(ent, AnimationComponent{});
        ecs->addComponent<TransformComponent>(ent, TransformComponent{pos, pos});
        ecs->addComponent<ProjectileComponent>(ent, ProjectileComponent{});
        ecs->addComponent<ProjectileComponent>(ent, ProjectileComponent{});

        ecs->addComponent<SpritesheetPropertiesComponent>(
            ent,
            createSpritesheetPropertiesComponent(SpritesheetRegistry::getSpritesheet(SpritesheetID::PROJECTILE))
            );
        
        return ent;
    }
    
    SpritesheetPropertiesComponent Projectile::createSpritesheetPropertiesComponent(Spritesheet* spritesheet) {
        SpritesheetPropertiesComponent props;
        props.spritesheet = spritesheet;
        
        SpritesheetProperties active = {
            0, // xTileIndex
            0, // yTileIndex
            true, // isAnimated
            true, // isLooped
            NUM_OF_ACTIVE_FRAMES, // numOfFrames
            MS_BETWEEN_ACTIVE_FRAMES, // msBetweenFrames
            SDL_FLIP_NONE, // flip
            0.0, // angle
            {-1, -1} // center
        };
        props.setPrimarySpritesheetProperties(active);

        return props;
    }
}