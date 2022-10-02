#ifndef STATE_COMPONENT_H
#define STATE_COMPONENT_H

enum class EntityState {
    NOVAL = -1,
    IDLE,
    ACTIVE,
    RUNNING,
    JUMPING,
};

struct StateComponent {
    EntityState state = EntityState::NOVAL;
};

#endif