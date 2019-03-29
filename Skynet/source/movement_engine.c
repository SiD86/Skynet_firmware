//  ***************************************************************************
/// @file    movement_engine.c
/// @author  NeoProg
//  ***************************************************************************
#include "movement_engine.h"

#include <sam.h>
#include <stdlib.h>
#include "limbs_driver.h"
#include "gait_sequences.h"
#include "error_handling.h"
#include "systimer.h"


typedef enum {
    STATE_NOINIT,           // Module not initialized
    STATE_IDLE,
    STATE_MOVE,             // Process step of current gait
    STATE_WAIT,             // Wait limbs movement complete
    STATE_NEXT_ITERATION,   // Select next step of current gait
    STATE_CHANGE_SEQUENCE   // Change current sequence (if needed)
} driver_state_t;

typedef enum {
	SEQUENCE_STAGE_PREPARE,
	SEQUENCE_STAGE_MAIN,
	SEQUENCE_STAGE_FINALIZE
} sequence_stage_t;

typedef enum {
	HEXAPOD_STATE_DOWN,
	HEXAPOD_STATE_UP
} hexapod_state_t;


static driver_state_t driver_state = STATE_NOINIT;
static hexapod_state_t hexapod_state = HEXAPOD_STATE_DOWN;
static int32_t hexapod_height = GAIT_SEQUENCE_HEIGHT_LOW_LIMIT;

static sequence_id_t current_sequence = SEQUENCE_NONE;
static const sequence_info_t* current_sequence_info = NULL;

static sequence_id_t next_sequence = SEQUENCE_NONE;
static const sequence_info_t* next_sequence_info = NULL;


static bool read_configuration(void);
static void update_sequences_y_coordinate(void);


//  ***************************************************************************
/// @brief  Movement driver initialization
/// @param  none
/// @return none
//  ***************************************************************************
void movement_engine_init(void) {
    
    if (read_configuration() == false) {
        callback_set_config_error(ERROR_MODULE_MOVEMENT_ENGINE);
        return;
    }
	
	update_sequences_y_coordinate();
	
	// Force select DOWN sequence. It automatically executed in movement_engine_process() function
	current_sequence      = SEQUENCE_NONE;
	current_sequence_info = NULL;
    next_sequence         = SEQUENCE_DOWN;
    next_sequence_info    = &sequence_down;
	
    driver_state          = STATE_IDLE;
}

//  ***************************************************************************
/// @brief  Movement driver process
/// @param  none
/// @return none
//  ***************************************************************************
void movement_engine_process(void) {
    
    if (callback_is_movement_engine_error_set() == true) return; // Module disabled
    

    static sequence_stage_t sequence_stage = SEQUENCE_STAGE_PREPARE;
    static uint32_t current_iteration = 0;

    switch (driver_state) {
        
        case STATE_IDLE:
			if (current_sequence != next_sequence) {
                driver_state = STATE_CHANGE_SEQUENCE;
            }
			break;
        
        case STATE_MOVE:
			limbs_driver_set_smooth_config(current_sequence_info->iteration_list[current_iteration].smooth_point_count);
            for (uint32_t i = 0; i < SUPPORT_LIMB_COUNT; ++i) {
                limbs_driver_start_move(current_sequence_info->iteration_list[current_iteration].point_list, 
									    current_sequence_info->iteration_list[current_iteration].path_list);
            }
            driver_state = STATE_WAIT;
            break;
        
        case STATE_WAIT:
            if (limbs_driver_is_move_complete() == true) {
                driver_state = STATE_NEXT_ITERATION;
            }
            break;
            
        case STATE_NEXT_ITERATION:
			
			++current_iteration;
            driver_state = STATE_MOVE;
			
			if (sequence_stage == SEQUENCE_STAGE_PREPARE && current_iteration >= current_sequence_info->main_sequence_begin) {
                sequence_stage = SEQUENCE_STAGE_MAIN;
			}
            if (sequence_stage == SEQUENCE_STAGE_MAIN && current_iteration >= current_sequence_info->finalize_sequence_begin) {
				
				if (current_sequence != next_sequence) { 
					
					// Need change current sequence - go to finalize sequence if it available
					current_iteration = current_sequence_info->finalize_sequence_begin;
					sequence_stage = SEQUENCE_STAGE_FINALIZE;
				}
				else {
                    
					if (current_sequence_info->is_sequence_looped == true) {
						current_iteration = current_sequence_info->main_sequence_begin;
					}
					else {
						// Current sequence completed and new sequence not selected
						hexapod_state = (current_sequence == SEQUENCE_DOWN) ? HEXAPOD_STATE_DOWN : HEXAPOD_STATE_UP;
						movement_engine_select_sequence(SEQUENCE_NONE);
						driver_state = STATE_CHANGE_SEQUENCE;
					}
				}              
            }
            if (sequence_stage == SEQUENCE_STAGE_FINALIZE && current_iteration >= current_sequence_info->total_iteration_count) {
				driver_state = STATE_CHANGE_SEQUENCE;
            }           
            break;
            
        case STATE_CHANGE_SEQUENCE:
			current_sequence      = next_sequence;
            current_sequence_info = next_sequence_info;
            current_iteration     = 0;
            sequence_stage        = SEQUENCE_STAGE_PREPARE;
            driver_state          = STATE_MOVE;
			
			if (current_sequence == SEQUENCE_NONE) {
    			driver_state = STATE_IDLE;
			}        
            break;
            
        case STATE_NOINIT:
        default:
            callback_set_internal_error(ERROR_MODULE_MOVEMENT_ENGINE);
            break;
    }
	
	if (hexapod_state == HEXAPOD_STATE_DOWN && hexapod_height != GAIT_SEQUENCE_HEIGHT_LOW_LIMIT) {
		hexapod_height = GAIT_SEQUENCE_HEIGHT_LOW_LIMIT;
		update_sequences_y_coordinate();
	}
}

//  ***************************************************************************
/// @brief  Set hexapod height
/// @param  height: new height
/// @return none
//  ***************************************************************************
void movement_engine_set_height(uint32_t height) {
	
	if (height < GAIT_SEQUENCE_HEIGHT_LOW_LIMIT || height > GAIT_SEQUENCE_HEIGHT_HIGH_LIMIT) {
		return;
	}
	
	hexapod_height = height;
}

//  ***************************************************************************
/// @brief  Select sequence
/// @param  sequence: new sequence
/// @return none
//  ***************************************************************************
void movement_engine_select_sequence(sequence_id_t sequence) {
    
    // Request switch current sequence
    switch (sequence) {
        
        case SEQUENCE_NONE:
			next_sequence = SEQUENCE_NONE;
            next_sequence_info = NULL;
            break;
			
		case SEQUENCE_UPDATE_HEIGHT:
			if (hexapod_state == HEXAPOD_STATE_UP) {
				next_sequence = SEQUENCE_UPDATE_HEIGHT;
				next_sequence_info = &sequence_update_height;
				
				update_sequences_y_coordinate();
			}
			break;
        
        case SEQUENCE_UP:
			if (hexapod_state == HEXAPOD_STATE_DOWN) {
				next_sequence = SEQUENCE_UP;
				next_sequence_info = &sequence_up;
			}
            break;

        case SEQUENCE_DOWN:
			if (hexapod_state == HEXAPOD_STATE_UP) {
				next_sequence = SEQUENCE_DOWN;
				next_sequence_info = &sequence_down;
			}
            break;

        case SEQUENCE_DIRECT_MOVEMENT:
			if (hexapod_state == HEXAPOD_STATE_UP) {
				next_sequence = SEQUENCE_DIRECT_MOVEMENT;
				next_sequence_info = &sequence_direct_movement;
			}
            break;

        case SEQUENCE_REVERSE_MOVEMENT: 
			if (hexapod_state == HEXAPOD_STATE_UP) {
				next_sequence = SEQUENCE_REVERSE_MOVEMENT;
				next_sequence_info = &sequence_reverse_movement;
			}
            break;

        case SEQUENCE_ROTATE_LEFT:
			if (hexapod_state == HEXAPOD_STATE_UP) {
				next_sequence = SEQUENCE_ROTATE_LEFT;
				next_sequence_info = &sequence_rotate_left;
			}
            break;

        case SEQUENCE_ROTATE_RIGHT:
			if (hexapod_state == HEXAPOD_STATE_UP) {
				next_sequence = SEQUENCE_ROTATE_RIGHT;
				next_sequence_info = &sequence_rotate_right;
			}
            break;
		
		case SEQUENCE_DIRECT_MOVEMENT_SHORT:
			if (hexapod_state == HEXAPOD_STATE_UP) {
				next_sequence = SEQUENCE_DIRECT_MOVEMENT_SHORT;
				next_sequence_info = &sequence_direct_movement_short;
			}
			break;

		case SEQUENCE_REVERSE_MOVEMENT_SHORT:
			if (hexapod_state == HEXAPOD_STATE_UP) {
				next_sequence = SEQUENCE_REVERSE_MOVEMENT_SHORT;
				next_sequence_info = &sequence_reverse_movement_short;
			}
			break;
			
		case SEQUENCE_ATTACK_LEFT:
			if (hexapod_state == HEXAPOD_STATE_UP) {
				next_sequence = SEQUENCE_ATTACK_LEFT;
				next_sequence_info = &sequence_attack_left;
			}
			break;
			
		case SEQUENCE_DENCE:
			if (hexapod_state == HEXAPOD_STATE_UP) {
				next_sequence = SEQUENCE_DENCE;
				next_sequence_info = &sequence_dence;
			}
			break;

        default:
            callback_set_internal_error(ERROR_MODULE_MOVEMENT_ENGINE);
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

//  ***************************************************************************
/// @brief  Update Y coordinate in all sequences
/// @param  none
/// @return none
//  ***************************************************************************
static void update_sequences_y_coordinate(void) {
	
	static sequence_info_t* sequence_list[] = {
		&sequence_update_height,
		&sequence_down,
		&sequence_up,
		&sequence_direct_movement,
		&sequence_reverse_movement,
		&sequence_rotate_left,
		&sequence_rotate_right,
		&sequence_direct_movement_short,
		&sequence_reverse_movement_short
	};
	
	// Change height in sequences
	for (uint32_t sequence = 0; sequence < sizeof(sequence_list) / sizeof(sequence_list[0]); ++sequence) {
					
		for (uint32_t i = 0; i < sequence_list[sequence]->total_iteration_count; ++i) {
						
			sequence_iteration_t* iteration = &sequence_list[sequence]->iteration_list[i];
			for (uint32_t a = 0; a < SUPPORT_LIMB_COUNT; ++a) {
							
				if (iteration->limb_state_list[a] == LIMB_STATE_DOWN) {
					iteration->point_list[a].y = -hexapod_height;
				}
				if (iteration->limb_state_list[a] == LIMB_STATE_UP) {
					iteration->point_list[a].y = -(hexapod_height - GAIT_SEQUENCE_LIMB_UP_STEP_HEIGHT);
				}
				/*if (iteration->limb_state_list[a] == LIMB_STATE_CUSTOM) {
					continue;
				}*/
			}
		}
	}
}