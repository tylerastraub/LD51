#include "DeathSystem.h"
#include "EntityRegistry.h"
#include "HealthComponent.h"
#include "PlayerComponent.h"

void DeathSystem::update(float timescale) {
    auto ecs = EntityRegistry::getInstance();
    for(auto ent : _entities) {
        auto health = ecs->getComponent<HealthComponent>(ent);
        if(health.hitpoints <= 0) {
            if(ecs->hasComponent<PlayerComponent>(ent)) {
                
            }
            else {
                ecs->destroyEntity(ent);
            }
        }
    }
}