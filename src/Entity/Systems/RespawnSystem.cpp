#include "RespawnSystem.h"
#include "EntityRegistry.h"

void RespawnSystem::onEntityDelete(Entity entity) {
    auto ecs = EntityRegistry::getInstance();
}