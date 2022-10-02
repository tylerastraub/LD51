#ifndef GOAL_COMPONENT_H
#define GOAL_COMPONENT_H

#include "ScriptComponent.h"

struct GoalComponent {
    std::shared_ptr<IScript> onActivatedScript = nullptr;
    bool activated = false;
};

#endif