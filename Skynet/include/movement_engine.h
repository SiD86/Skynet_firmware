//  ***************************************************************************
/// @file    movement_engine.h
/// @author  NeoProg
/// @brief   Hexapod movement engine
//  ***************************************************************************
#ifndef MOVEMENT_ENGINE_H_
#define MOVEMENT_ENGINE_H_

#include <stdint.h>


typedef enum {
    SEQUENCE_NONE,
    
    SEQUENCE_UPDATE_HEIGHT,
    SEQUENCE_UP,
    SEQUENCE_DOWN,
    
    SEQUENCE_DIRECT_MOVEMENT,
    SEQUENCE_REVERSE_MOVEMENT,
    SEQUENCE_ROTATE_LEFT,
    SEQUENCE_ROTATE_RIGHT,
    
    SEQUENCE_DIRECT_MOVEMENT_SHORT,
    SEQUENCE_REVERSE_MOVEMENT_SHORT,
    SEQUENCE_ROTATE_LEFT_SHORT,
    SEQUENCE_ROTATE_RIGHT_SHORT,
    
    SEQUENCE_ATTACK_LEFT,
    SEQUENCE_ATTACK_RIGHT,
    SEQUENCE_DANCE,
    
    SUPPORT_SEQUENCE_COUNT
} sequence_id_t;


extern void movement_engine_init(void);
extern void movement_engine_process(void);
extern void movement_engine_increase_height(void);
extern void movement_engine_decrease_height(void);
extern void movement_engine_select_sequence(sequence_id_t sequence);


#endif /* MOVEMENT_ENGINE_H_ */