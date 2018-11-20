//  ***************************************************************************
/// @file    gaits_engine.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdlib.h>
#include "gaits_engine.h"
#include "limbs_driver.h"
#include "error_handling.h"
#include "systimer.h"
#include "tripod_gait_sequences.h"


typedef enum {
    STATE_NOINIT,       // Module not initialized
    STATE_PROCESS,      // Process step of current gait
    STATE_WAIT,         // Wait limbs movement complete
    STATE_NEXT_STEP,    // Select next step of current gait
    STATE_UPDATE_STATE, // Update current hexapod state
    STATE_CHANGE_GAIT   // Change current sequence (if needed)
} module_state_t;

typedef enum {
    HEXAPOD_STATE_DOWN,
    HEXAPOD_STATE_BASE,
    HEXAPOD_STATE_CUSTOM
} hexapod_state_t;



static module_state_t engine_state = STATE_NOINIT;
static hexapod_state_t hexapod_state = HEXAPOD_STATE_DOWN;

static sequence_t current_sequence = SEQUENCE_DOWN;
static sequence_info_t* current_sequence_info = &sequence_down;

static sequence_t next_sequence = SEQUENCE_DOWN;
static sequence_info_t* next_sequence_info = &sequence_down;


static bool read_configuration(void);


//  ***************************************************************************
/// @brief  Gaits engine initialize
/// @param  none
/// @return none
//  ***************************************************************************
void gaits_engine_init(void) {
    
    if (read_configuration() == false) {
        callback_set_config_error(ERROR_MODULE_GAITS_ENGINE);
        return;
    }

    current_sequence      = SEQUENCE_DOWN;
    next_sequence         = SEQUENCE_DOWN;
    current_sequence_info = &sequence_down;
    next_sequence_info    = &sequence_down;
    
    engine_state  = STATE_PROCESS;
    hexapod_state = HEXAPOD_STATE_DOWN;
}

//  ***************************************************************************
/// @brief  Gaits engine process
/// @param    none
/// @return none
//  ***************************************************************************
void gaits_engine_process(void) {
    
    if (callback_is_gaits_engine_error_set() == true) return; // Module disable
    
    
    static uint32_t current_sequence_step = 0;
    static uint32_t start_delay_time = 0;
    
    
    switch (engine_state) {
        
        case STATE_PROCESS:
            // Move limbs to new position
            for (uint32_t i = 0; i < 6; ++i) {
                limbs_driver_start_move(i, current_sequence_info->steps_list[current_sequence_step].coxa[i],
                                           current_sequence_info->steps_list[current_sequence_step].femur[i],
                                           current_sequence_info->steps_list[current_sequence_step].tibia[i],
                                           current_sequence_info->movement_iteration_count);
            }
            
            engine_state = STATE_WAIT;
            break;
        
        case STATE_WAIT:
            // Check limbs movement complete
            if (limbs_driver_is_move_complete() == true) {
                start_delay_time = get_time_ms();
                engine_state = STATE_NEXT_STEP;
            }
            break;
        
        case STATE_NEXT_STEP:
            // Wait time between steps
            if (get_time_ms() - start_delay_time < current_sequence_info->delay_between_steps) {
                break; 
            }
            
            // Go to next sequence step
            ++current_sequence_step;
            if (current_sequence_step >= current_sequence_info->steps_count) {
                current_sequence_step = current_sequence_info->begin_loop_step;
                
                engine_state = STATE_UPDATE_STATE;
            }
            break;
            
        case STATE_UPDATE_STATE:
            // Current sequence end. Change current hexapod state
            switch (current_sequence) {
                case SEQUENCE_UP:
                case SEQUENCE_ROTATE_LEFT:
                case SEQUENCE_ROTATE_RIGHT:
                case SEQUENCE_BASE:
                    hexapod_state = HEXAPOD_STATE_BASE;
                    break;
                    
                case SEQUENCE_DIRECT_MOVEMENT:
                case SEQUENCE_REVERSE_MOVEMENT:
                    hexapod_state = HEXAPOD_STATE_CUSTOM;
                    break;
                    
                case SEQUENCE_DOWN:
                    hexapod_state = HEXAPOD_STATE_DOWN;
                    break;
                    
                default:
                    callback_set_selfdiag_error(ERROR_MODULE_GAITS_ENGINE);
                    break;
            }
            
            // Check need change current sequence
            if (current_sequence != next_sequence) {
                engine_state = STATE_CHANGE_GAIT;
            }
            else {
                engine_state = STATE_PROCESS;
            }
            break;
            
        case STATE_CHANGE_GAIT:
            if (hexapod_state == HEXAPOD_STATE_DOWN) {
                
                if (next_sequence == SEQUENCE_UP) {
                    current_sequence = next_sequence;
                    current_sequence_info = next_sequence_info;
                    current_sequence_step = 0;
                }
                else {
                    // Selected sequence not available in this hexapod state
                    // Restore previous values
                    next_sequence = current_sequence;
                    next_sequence_info = current_sequence_info;
                }
            }
            else if (hexapod_state == HEXAPOD_STATE_CUSTOM) {
                current_sequence = SEQUENCE_BASE;
                current_sequence_info = &sequence_base;
                current_sequence_step = 0;
            }
            else if (hexapod_state == HEXAPOD_STATE_BASE) {
                
                if (next_sequence == SEQUENCE_BASE) {
                    // Selected sequence already completed
                    // Restore previous values
                    next_sequence = current_sequence;
                    next_sequence_info = current_sequence_info;
                }
                else {
                    current_sequence = next_sequence;
                    current_sequence_info = next_sequence_info;
                    current_sequence_step = 0;
                }
            }
            else {
                callback_set_selfdiag_error(ERROR_MODULE_GAITS_ENGINE);
            }
            engine_state = STATE_PROCESS;
            break;
            
        case STATE_NOINIT:
        default:
            callback_set_selfdiag_error(ERROR_MODULE_GAITS_ENGINE);
            return;
    }
}

//  ***************************************************************************
/// @brief  Select gait
/// @param  sequence: new sequence
/// @return none
//  ***************************************************************************
void gaits_engine_select_sequence(sequence_t sequence) {
    
    switch (sequence) {
        
        case SEQUENCE_NONE:
            return;
        
        case SEQUENCE_BASE:
            next_sequence_info = &sequence_base;
            break;
        
        case SEQUENCE_UP:    
            next_sequence_info = &sequence_up;
            break;

        case SEQUENCE_DOWN:                
            next_sequence_info = &sequence_down;
            break;

        case SEQUENCE_DIRECT_MOVEMENT:    
            next_sequence_info = &sequence_direct_movement;
            break;

        /*case SEQUENCE_REVERSE_MOVEMENT:    
            next_gait_info = &sequence_reverse_movement;
            break;*/

        case SEQUENCE_ROTATE_LEFT:        
            next_sequence_info = &sequence_rotate_left;
            break;

        case SEQUENCE_ROTATE_RIGHT:        
            next_sequence_info = &sequence_rotate_right;
            break;

        default:
            callback_set_selfdiag_error(ERROR_MODULE_GAITS_ENGINE);
            return;
    }
    next_sequence = sequence;
}



//  ***************************************************************************
/// @brief  Read configuration
/// @param  none
/// @return true - read success, false - fail
//  ***************************************************************************
static bool read_configuration(void) {
    
    return true;
}