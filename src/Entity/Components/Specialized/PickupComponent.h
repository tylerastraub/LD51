#ifndef PICKUP_COMPONENT_H
#define PICKUP_COMPONENT_H

#include "ScriptComponent.h"

enum class PickupType {
    NOVAL = -1,
    WEAPON,
    JUMP,
    BOOTS,
    WALLJUMP,
};

struct PickupComponent {
    std::shared_ptr<IScript> onPickupScript = nullptr;
    std::string onPickupMessage = "";
    PickupType pickupType = PickupType::NOVAL;
};

#endif