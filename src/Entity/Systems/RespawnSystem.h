#ifndef RESPAWN_SYSTEM_H
#define RESPAWN_SYSTEM_H

#include "System.h"

class RespawnSystem : public System {
public:
    RespawnSystem() = default;
    ~RespawnSystem() = default;

    void onEntityDelete(Entity entity) override;

private:

};

#endif