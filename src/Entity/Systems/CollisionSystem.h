#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "System.h"
#include "Level.h"
#include "PickupComponent.h"

namespace {
    struct TileCoords {
        int x;
        int y;
        int dist;

        TileCoords(int x, int y, int dist) : x(x), y(y), dist(dist) {}

        bool operator<(const TileCoords& rhs) const
        {
            return dist < rhs.dist;
        }
    };
}

class CollisionSystem : public System {
public:
    CollisionSystem() = default;
    ~CollisionSystem() = default;

    void checkForLevelCollisionsOnXAxis(Level* level, float timescale);
    void checkForLevelCollisionsOnYAxis(Level* level, float timescale);
    bool checkForPlayerAndItemCollisions(Entity player, float timescale, std::string& itemMessage, PickupType& pickupType);
    bool checkForPlayerAndCheckpointCollisions(Entity player, float timescale, Entity& checkpointResult);
    void checkForProjectileAndEnemyCollisions(float timescale);
    void checkForPlayerAndEnemyCollisions(Entity player, float timescale);
    bool checkForPlayerAndGoalCollisions(Entity player, float timescale, Entity& goalResult);
    void checkIfOnEdge(Level* level);

private:

};

#endif