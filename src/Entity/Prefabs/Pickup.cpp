#include "Pickup.h"
#include "EntityRegistry.h"
#include "SpritesheetRegistry.h"
// Components
#include "PhysicsComponent.h"
#include "ScriptComponent.h"
#include "RenderComponent.h"
#include "SpritesheetPropertiesComponent.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "PlayerComponent.h"
#include "InputComponent.h"
#include "BootsComponent.h"
#include "WalljumpComponent.h"
#include "WeaponComponent.h"

#include <cmath>
#include <algorithm>

namespace PickupScript {
    class PickupIdleScript : public IScript {
    public:
        PickupIdleScript() = default;
        ~PickupIdleScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            auto& physics = ecs->getComponent<PhysicsComponent>(owner);

            // after extended time this gets out of sync so we reset timer after every cycle
            float halfPi = 1.5707963267948966192313216916398f;
            physics.velocity.y += std::cos(_timer * halfPi) / 4.f;
            _timer += timescale;
            if(_timer >= 4) {
                resetIdleTimer();
                physics.velocity.y = 0;
            }
        }

        void resetIdleTimer() {
            _timer = 0;
        }

    private:
        float _timer = 0;    

    };

    class WeaponPickupScript : public IScript {
    public:
        WeaponPickupScript() = default;
        ~WeaponPickupScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            Entity player = ecs->getAllOf<PlayerComponent>().front();
            ecs->addComponent<WeaponComponent>(player, WeaponComponent{});
        }
    private:

    };

    class JumpPickupScript : public IScript {
    public:
        JumpPickupScript() = default;
        ~JumpPickupScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            Entity player = ecs->getAllOf<PlayerComponent>().front();
            auto& input = ecs->getComponent<InputComponent>(player);
            if(std::find(input.allowedInputs.begin(), input.allowedInputs.end(), InputEvent::JUMP) == input.allowedInputs.end()) {
                input.allowedInputs.push_back(InputEvent::JUMP);
            }
        }
    private:

    };

    class BootsPickupScript : public IScript {
    public:
        BootsPickupScript() = default;
        ~BootsPickupScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            Entity player = ecs->getAllOf<PlayerComponent>().front();
            ecs->addComponent<BootsComponent>(player, BootsComponent{});
        }
    private:

    };

    class WalljumpPickupScript : public IScript {
    public:
        WalljumpPickupScript() = default;
        ~WalljumpPickupScript() = default;

        void update(Entity owner, float timescale, Audio* audio) override {
            auto ecs = EntityRegistry::getInstance();
            Entity player = ecs->getAllOf<PlayerComponent>().front();
            ecs->addComponent<WalljumpComponent>(player, WalljumpComponent{});
        }
    private:

    };
}

namespace prefab
{
    Entity Pickup::create() {
        return Pickup::create({0.f, 0.f}, PickupType::NOVAL);
    }

    Entity Pickup::create(strb::vec2 pos, PickupType pickupType) {
        auto ecs = EntityRegistry::getInstance();
        Entity ent = ecs->createEntity();

        PickupComponent pickupComp;
        SpritesheetID spritesheetId;
        pickupComp.pickupType = pickupType;
        switch(pickupType) {
            case PickupType::WEAPON:
                pickupComp.onPickupScript = std::make_shared<PickupScript::WeaponPickupScript>();
                pickupComp.onPickupMessage = "New upgrade: BLASTER! Press 'Z' to shoot.";
                spritesheetId = SpritesheetID::WEAPON_PICKUP;
                break;
            case PickupType::JUMP:
                pickupComp.onPickupScript = std::make_shared<PickupScript::JumpPickupScript>();
                pickupComp.onPickupMessage = "New upgrade: JUMP! Press 'UP' to jump.";
                spritesheetId = SpritesheetID::JUMP_PICKUP;
                break;
            case PickupType::BOOTS:
                pickupComp.onPickupScript = std::make_shared<PickupScript::BootsPickupScript>();
                pickupComp.onPickupMessage = "New upgrade: BOOTS! You can now walk on spikes. However, be careful - these boots aren't quite sturdy enough to protect you if you are falling!";
                spritesheetId = SpritesheetID::BOOTS_PICKUP;
                break;
            case PickupType::WALLJUMP:
                pickupComp.onPickupScript = std::make_shared<PickupScript::WalljumpPickupScript>();
                pickupComp.onPickupMessage = "New upgrade: WALLJUMP! Hold the movement key in the direction of the wall and press 'UP' to jump off it. You can walljump multiple times on the same wall!";
                spritesheetId = SpritesheetID::WALLJUMP_PICKUP;
                break;
            default:
                break;
        }
        ecs->addComponent<PickupComponent>(ent, pickupComp);
        ecs->addComponent<SpritesheetPropertiesComponent>(
            ent,
            createSpritesheetPropertiesComponent(SpritesheetRegistry::getSpritesheet(spritesheetId))
            );
        
        PhysicsComponent physics;
        physics.airAcceleration = {0.f, 0.f};
        physics.acceleration = {0.f, 0.f};
        physics.maxVelocity = {100.f, 100.f};
        physics.jumpPower = 0.f;
        physics.frictionCoefficient = 0.f;
        physics.airFrictionCoefficient = 0.f;
        physics.gravity = 0.f;
        physics.touchingGround = false;

        ecs->addComponent<PhysicsComponent>(ent, physics);

        RenderComponent render;
        render.renderQuad = {0, 0, 16, 16};
        
        ecs->addComponent<RenderComponent>(ent, render);

        CollisionComponent collision;
        collision.collisionRect = {0, 0, 14, 14};
        collision.collisionRectOffset = {1, 1};

        ecs->addComponent<CollisionComponent>(ent, collision);

        ecs->addComponent<TransformComponent>(ent, {pos, pos});

        ecs->addComponent<ScriptComponent>(ent, ScriptComponent{std::make_shared<PickupScript::PickupIdleScript>()});

        return ent;
    }

    SpritesheetPropertiesComponent Pickup::createSpritesheetPropertiesComponent(Spritesheet* spritesheet) {
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

        return props;
    }
}
