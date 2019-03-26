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
    STATE_NEXT_STAGE,
    STATE_CHANGE_SEQUENCE   // Change current sequence (if needed)
} driver_state_t;

typedef enum {
	SEQUENCE_STAGE_PREPARE,
	SEQUENCE_STAGE_MAIN,
	SEQUENCE_STAGE_FINALIZE
} sequence_stage_t;


static driver_state_t driver_state = STATE_NOINIT;

static sequence_id_t current_sequence = SEQUENCE_NONE;
static const sequence_info_t* current_sequence_info = NULL;

static sequence_id_t next_sequence = SEQUENCE_NONE;
static const sequence_info_t* next_sequence_info = NULL;


static bool read_configuration(void);


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
	
    movement_engine_select_sequence(SEQUENCE_DOWN);
    driver_state = STATE_IDLE;
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
						driver_state = STATE_IDLE;
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
            driver_state = STATE_MOVE;
			
			if (current_sequence == SEQUENCE_NONE) {
    			driver_state = STATE_IDLE;
			}        
            break;
            
        case STATE_NOINIT:
        default:
            callback_set_internal_error(ERROR_MODULE_MOVEMENT_ENGINE);
            return;
    }
}

//  ***************************************************************************
/// @brief  Increase hexapod height
/// @param  none
/// @return none
//  ***************************************************************************
void movement_engine_increase_height(void) {
	
	if (current_sequence != SEQUENCE_NONE) {
		return;
	}
	
	// Make sequence list
	sequence_info_t* sequence_list[] = {&sequence_change_height, 
							            // &sequence_down, 
							            &sequence_up, 
							            &sequence_direct_movement, 
							            &sequence_reverse_movement,
							            &sequence_rotate_left,
							            &sequence_rotate_right};
										
	// Check available increase height
	for (uint32_t sequence = 0; sequence < sizeof(sequence_list) / sizeof(sequence_list[0]); ++sequence) {
											
		for (uint32_t i = 0; i < sequence_list[sequence]->total_iteration_count; ++i) {
												
			for (uint32_t a = 0; a < SUPPORT_LIMB_COUNT; ++a) {
				
				uint32_t height = abs(sequence_list[sequence]->iteration_list[i].point_list[a].y - 20);
				if (height > GAIT_SEQUENCE_HEIGHT_HIGH_LIMIT) {
					return;
				}
			}
		}
	}
							
	// Change height in sequences
	for (uint32_t sequence = 0; sequence < sizeof(sequence_list) / sizeof(sequence_list[0]); ++sequence) {
		
		for (uint32_t i = 0; i < sequence_list[sequence]->total_iteration_count; ++i) {
			
			for (uint32_t a = 0; a < SUPPORT_LIMB_COUNT; ++a) {
				sequence_list[sequence]->iteration_list[i].point_list[a].y -= 20;
			}
		}
	}
	
	// Update height
	movement_engine_select_sequence(SEQUENCE_CHANGE_HEIGHT);
}

//  ***************************************************************************
/// @brief  Decrease hexapod height
/// @param  none
/// @return none
//  ***************************************************************************
void movement_engine_decrease_height(void) {
	
	if (current_sequence != SEQUENCE_NONE) {
		return;
	}
	
	// Make sequence list
	sequence_info_t* sequence_list[] = {&sequence_change_height,
										// &sequence_down,
										&sequence_up,
										&sequence_direct_movement,
										&sequence_reverse_movement,
										&sequence_rotate_left,
										&sequence_rotate_right};
	// Check available decrease height
	for (uint32_t sequence = 0; sequence < sizeof(sequence_list) / sizeof(sequence_list[0]); ++sequence) {
		
		for (uint32_t i = 0; i < sequence_list[sequence]->total_iteration_count; ++i) {
			
			for (uint32_t a = 0; a < SUPPORT_LIMB_COUNT; ++a) {
				
				uint32_t height = abs(sequence_list[sequence]->iteration_list[i].point_list[a].y + 20);
				if (height < GAIT_SEQUENCE_HEIGHT_LOW_LIMIT) {
					return;
				}
			}
		}
	}
	
	// Change height in sequences
	for (uint32_t sequence = 0; sequence < sizeof(sequence_list) / sizeof(sequence_list[0]); ++sequence) {
		
		for (uint32_t i = 0; i < sequence_list[sequence]->total_iteration_count; ++i) {
			
			for (uint32_t a = 0; a < SUPPORT_LIMB_COUNT; ++a) {
				sequence_list[sequence]->iteration_list[i].point_list[a].y += 20;
			}
		}
	}
	
	// Update height
	movement_engine_select_sequence(SEQUENCE_CHANGE_HEIGHT);
}

//  ***************************************************************************
/// @brief  Select sequence
/// @param  sequence: new sequence
/// @return none
//  ***************************************************************************
void movement_engine_select_sequence(sequence_id_t sequence) {
    
    // Check possibility of go to selected sequence
	if (current_sequence != SEQUENCE_NONE) {
		
		bool is_possibility = false;
		for (uint32_t i = 0; /* NONE */; ++i) {
			
    		if (current_sequence_info->available_sequences[i] == sequence) {
        		is_possibility = true;
        		break;
    		}
			
			if (current_sequence_info->available_sequences[i] == SEQUENCE_NONE) {
				break;
			}
		}
		if (is_possibility == false) {
    		return;
		}   
	}
    
    // Request switch current sequence
    switch (sequence) {
        
        case SEQUENCE_NONE:
			next_sequence = SEQUENCE_NONE;
            next_sequence_info = NULL;
            break;
			
		case SEQUENCE_CHANGE_HEIGHT:
			next_sequence = SEQUENCE_CHANGE_HEIGHT;
			next_sequence_info = &sequence_change_height;
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

        case SEQUENCE_REVERSE_MOVEMENT: 
			next_sequence = SEQUENCE_REVERSE_MOVEMENT;
            next_sequence_info = &sequence_reverse_movement;
            break;

        case SEQUENCE_ROTATE_LEFT:
            next_sequence = SEQUENCE_ROTATE_LEFT;
            next_sequence_info = &sequence_rotate_left;
            break;

        case SEQUENCE_ROTATE_RIGHT:
            next_sequence = SEQUENCE_ROTATE_RIGHT;
            next_sequence_info = &sequence_rotate_right;
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