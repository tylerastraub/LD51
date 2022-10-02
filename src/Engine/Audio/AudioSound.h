#ifndef AUDIO_SOUND_H
#define AUDIO_SOUND_H

enum class AudioSound {
    NOVAL = -1,
    ACTION,
    JUMP,
    WALLJUMP,
    CHARACTER_BLIP,
    ITEM_PICKUP,
    CHECKPOINT_ACTIVATED,
    CHECKPOINT_RESPAWN,
    DEAD,
    CLOCK_TICK,
    SHOOT,
    GOAL,
};

#endif