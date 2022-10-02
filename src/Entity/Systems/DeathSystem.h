#ifndef DEATH_SYSTEM_H
#define DEATH_SYSTEM_H

#include "System.h"

class DeathSystem : public System {
public:
    DeathSystem() = default;
    ~DeathSystem() = default;

    void update(float timescale);

private:

};

#endif