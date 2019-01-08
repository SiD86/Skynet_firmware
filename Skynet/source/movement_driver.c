//  ***************************************************************************
/// @file    movement_driver.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdlib.h>
#include "movement_driver.h"
#include "limbs_driver.h"
#include "gait_sequences.h"
#include "error_handling.h"
#include "systimer.h"


typedef enum {
    STATE_NOINIT,           // Module not initialized
    STATE_MOVE,             // Process step of current gait
    STATE_WAIT,             // Wait limbs movement complete
    STATE_DELAY,            // Delay between iterations
    STATE_NEXT_ITERATION,   // Select next step of current gait
    STATE_CHANGE_SEQUENCE   // Change current sequence (if needed)
} driver_state_t;


static driver_state_t driver_state = STATE_NOINIT;

static sequence_t current_sequence = SEQUENCE_DOWN;
static const sequence_info_t* current_sequence_info = &sequence_down;

static sequence_t next_sequence = SEQUENCE_DOWN;
static const sequence_info_t* next_sequence_info = &sequence_down;


static bool read_configuration(void);


//  ***************************************************************************
/// @brief  Movement driver initialization
/// @param  none
/// @return none
//  ***************************************************************************
void movement_driver_init(void) {
    
    if (read_configuration() == false) {
        callback_set_config_error(ERROR_MODULE_GAITS_ENGINE);
        return;
    }

    current_sequence      = SEQUENCE_DOWN;
    next_sequence         = SEQUENCE_DOWN;
    current_sequence_info = &sequence_down;
    next_sequence_info    = &sequence_down;
    
    driver_state = STATE_MOVE;
}

//  ***************************************************************************
/// @brief  Movement driver process
/// @param  none
/// @return none
//  ***************************************************************************
void movement_driver_process(void) {
    
    if (callback_is_gaits_engine_error_set() == true) return; // Module disabled
    
    
    static uint32_t current_iteration = 0;
    static uint32_t start_delay_time = 0;
    
    switch (driver_state) {
        
        case STATE_MOVE:
            for (uint32_t i = 0; i < 6; ++i) {
                /*limbs_driver_start_move(i, current_sequence_info->iteration_list[current_iteration].coxa[i],
                                           current_sequence_info->iteration_list[current_iteration].femur[i],
                                           current_sequence_info->iteration_list[current_iteration].tibia[i],
                                           100);*/
            }
            driver_state = STATE_WAIT;
            break;
        
        case STATE_WAIT:
            if (limbs_driver_is_move_complete() == true) {
                start_delay_time = get_time_ms();
                driver_state = STATE_DELAY;
            }
            break;
            
        case STATE_DELAY:
            if (get_time_ms() - start_delay_time >= 100) {
                driver_state = STATE_NEXT_ITERATION;
            }
            break;
            
        case STATE_NEXT_ITERATION:
            ++current_iteration;
            if (current_iteration >= current_sequence_info->iteration_count) {
                current_iteration = current_sequence_info->begin_loop_iteration;
                driver_state = STATE_CHANGE_SEQUENCE;
            }
			else {
				driver_state = STATE_MOVE;
			}
            break;
            
        case STATE_CHANGE_SEQUENCE:
            if (current_sequence != next_sequence) {
                current_sequence = next_sequence;
                current_sequence_info = next_sequence_info;
                current_iteration = 0;
            }
            driver_state = STATE_MOVE;
            break;
            
        case STATE_NOINIT:
        default:
            callback_set_selfdiag_error(ERROR_MODULE_GAITS_ENGINE);
            return;
    }
}

//  ***************************************************************************
/// @brief  Select sequence
/// @param  sequence: new sequence
/// @return none
//  ***************************************************************************
void movement_driver_select_sequence(sequence_t sequence) {
    
    // Check possibility of go to selected sequence
    bool is_possibility = false;
    for (uint32_t i = 0; i < SUPPORT_SEQUENCE_COUNT; ++i) {
        if (current_sequence_info->available_sequences[i] == SEQUENCE_NONE) {
            return;
        }
        if (current_sequence_info->available_sequences[i] == sequence) {
            is_possibility = true;
            break;
        }
    }
    if (is_possibility == false) {
        return;
    }
    
    // Request switch current sequence
    switch (sequence) {
        
        case SEQUENCE_NONE:
            return;
        
        case SEQUENCE_BASE:
            next_sequence = SEQUENCE_BASE;
            next_sequence_info = &sequence_base;
            break;
        
        case SEQUENCE_UP:
            next_sequence = SEQUENCE_UP;
            next_sequence_info = &sequence_up;
            break;

        case SEQUENCE_DOWN:
            next_sequence = SEQUENCE_DOWN;
            next_sequence_info = &sequence_down;
            break;

        case SEQUENCE_DIRECT_MOVEMENT:
            next_sequence = SEQUENCE_DIRECT_MOVEMENT;
            next_sequence_info = &sequence_direct_movement;
            break;

        /*case SEQUENCE_REVERSE_MOVEMENT:    
            next_gait_info = &sequence_reverse_movement;
            break;*/

        case SEQUENCE_ROTATE_LEFT:
            next_sequence = SEQUENCE_ROTATE_LEFT;
            next_sequence_info = &sequence_rotate_left;
            break;

        case SEQUENCE_ROTATE_RIGHT:
            next_sequence = SEQUENCE_ROTATE_RIGHT;
            next_sequence_info = &sequence_rotate_right;
            break;

        default:
            callback_set_selfdiag_error(ERROR_MODULE_GAITS_ENGINE);
            return;
    }
    
}



//  ***************************************************************************
/// @brief  Read configuration
/// @param  none
/// @return true - read success, false - fail
//  ***************************************************************************
static bool read_configuration(void) {
    
    return true;
}