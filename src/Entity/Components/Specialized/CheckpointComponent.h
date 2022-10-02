#ifndef CHECKPOINT_COMPONENT_H
#define CHECKPOINT_COMPONENT_H

#include "ScriptComponent.h"

struct CheckpointComponent {
    std::shared_ptr<IScript> onActivatedScript = nullptr;
    bool isActive = false;
};

#endif