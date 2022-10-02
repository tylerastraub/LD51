#ifndef WEAPON_COMPONENT_H
#define WEAPON_COMPONENT_H

struct WeaponComponent {
    int shotCooldown = 100;
    int timeSinceLastShot = 0;
};

#endif