//  ***************************************************************************
/// @file    gait_sequences.h
/// @author  NeoProg
/// @brief   Gait sequences
//  ***************************************************************************
#ifndef GAIT_SEQUENCES_H_
#define GAIT_SEQUENCES_H_


#include "limbs_driver.h"
#include "movement_driver.h"


typedef struct {
	int32_t coxa[SUPPORT_LIMB_COUNT];
	int32_t femur[SUPPORT_LIMB_COUNT];
	int32_t tibia[SUPPORT_LIMB_COUNT];
} sequence_iteration_t;

typedef struct {
    uint32_t iteration_count;                   // Sequence iterations count
    uint32_t begin_loop_iteration;              // Sequence iterations index witch begins loop
    sequence_iteration_t iteration_list[20];    // Sequence iterations list
    sequence_t available_sequences[SUPPORT_SEQUENCE_COUNT]; // Available sequence id`s list to which can go from this sequence
} sequence_info_t;


static const sequence_info_t sequence_down = {  
    
    .iteration_count = 1, .begin_loop_iteration = 0,
    {
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
        {       45,      45,      45,      45,      45,      45 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }}
    },
    { SEQUENCE_UP, SEQUENCE_NONE }
};

static const sequence_info_t sequence_base = {

    .iteration_count = 4, .begin_loop_iteration = 3,
    {
        {{       0, NO_MOVE,       0, NO_MOVE,       0, NO_MOVE },    // Up 1, 3, 5 legs and return to start position
        {       60, NO_MOVE,      60, NO_MOVE,      60, NO_MOVE },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },    // Return 1, 3, 5 legs to ground
        {       80, NO_MOVE,      80, NO_MOVE,      80, NO_MOVE },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE,       0, NO_MOVE,       0, NO_MOVE,       0 },    // Return 2, 4, 6 legs to start position
        {  NO_MOVE,      60, NO_MOVE,      60, NO_MOVE,      60 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },    // Return 2, 4, 6 legs to ground
        {  NO_MOVE,      80, NO_MOVE,      80, NO_MOVE,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE}}
    },
    { SEQUENCE_DOWN, SEQUENCE_DIRECT_MOVEMENT, SEQUENCE_REVERSE_MOVEMENT, 
      SEQUENCE_ROTATE_LEFT, SEQUENCE_ROTATE_RIGHT, SEQUENCE_NONE }
};

static const sequence_info_t sequence_up = {

    .iteration_count = 7, .begin_loop_iteration = 6,
    {
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
        {       45,      45,      45,      45,      45,      45 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
        {       80,      80,      80,      80,      80,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{       0, NO_MOVE,       0, NO_MOVE,       0, NO_MOVE },
        {       60, NO_MOVE,      60, NO_MOVE,      60, NO_MOVE },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
        {       80, NO_MOVE,      80, NO_MOVE,      80, NO_MOVE },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE,       0, NO_MOVE,       0, NO_MOVE,       0 },
        {  NO_MOVE,      60, NO_MOVE,      60, NO_MOVE,      60 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
        {  NO_MOVE,      80, NO_MOVE,      80, NO_MOVE,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE}},
			
		{{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
		{  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
		{  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE}}
    },
    { SEQUENCE_BASE, SEQUENCE_DOWN, SEQUENCE_DIRECT_MOVEMENT, SEQUENCE_REVERSE_MOVEMENT, 
      SEQUENCE_ROTATE_LEFT, SEQUENCE_ROTATE_RIGHT, SEQUENCE_NONE }
};

static const sequence_info_t sequence_direct_movement = {

    .iteration_count = 8, .begin_loop_iteration = 4,
    {
        // PREPARE SEQUENCE
        {{     -35,       0,      55,       0,      10,       0 },
        {       60,      80,      60,      80,      60,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},

        {{     -35,       0,      55,       0,      10,       0 },
        {       80,      80,      80,      80,      80,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{     -35,     -10,      55,     -55,      10,      35 },
        {       80,      60,      80,      60,      80,      60 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{     -35,     -10,      55,     -55,      10,      35 },
        {       80,      80,      80,      80,      80,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        // LOOPED SEQUENCE
        {{     -35,     -10,      55,     -55,      10,      35 },
        {       80,      60,      80,      60,      80,      60 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{     -55,      10,      35,     -35,     -10,      55 },
        {       80,      80,      80,      80,      80,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},

        {{     -55,      10,      35,     -35,     -10,      55 },
        {       60,      80,      60,      80,      60,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{     -35,     -10,      55,     -55,      10,      35 },
        {       80,      80,      80,      80,      80,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }}
    },
    { SEQUENCE_BASE, SEQUENCE_NONE }
};

static const sequence_info_t sequence_rotate_left = {
    
    .iteration_count = 6, .begin_loop_iteration = 0,
    {
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },    // Up 2, 5 legs
        {  NO_MOVE,      60, NO_MOVE, NO_MOVE,      60, NO_MOVE },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{      30, NO_MOVE,      30,     -30, NO_MOVE,     -30 },    // Rotate hull
        {       80,      80,      80,      80,      80,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{       0, NO_MOVE,       0, NO_MOVE, NO_MOVE, NO_MOVE },    // Up 1, 2 legs and return to start position
        {       60, NO_MOVE,      60, NO_MOVE, NO_MOVE, NO_MOVE },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },    // Return 1, 2 legs to ground
        {       80, NO_MOVE,      80, NO_MOVE, NO_MOVE, NO_MOVE },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE, NO_MOVE, NO_MOVE,       0, NO_MOVE,       0 },    // Up 4, 6 legs and return to start position
        {  NO_MOVE, NO_MOVE, NO_MOVE,      60, NO_MOVE,      60 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },    // Return 4, 6 legs to ground
        {  NO_MOVE, NO_MOVE, NO_MOVE,      80, NO_MOVE,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }}
    },
    { SEQUENCE_BASE, SEQUENCE_DOWN, SEQUENCE_DIRECT_MOVEMENT, SEQUENCE_REVERSE_MOVEMENT, 
      SEQUENCE_ROTATE_RIGHT, SEQUENCE_NONE }
};

static const sequence_info_t sequence_rotate_right = {

    .iteration_count = 6, .begin_loop_iteration = 0,
    {
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },    // Up 2, 5 legs
        {  NO_MOVE,      60, NO_MOVE, NO_MOVE,      60, NO_MOVE },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{     -30, NO_MOVE,     -30,      30, NO_MOVE,      30 },    // Rotate hull
        {       80,      80,      80,      80,      80,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{       0, NO_MOVE,       0, NO_MOVE, NO_MOVE, NO_MOVE },    // Up 1, 2 legs and return to start position
        {       60, NO_MOVE,      60, NO_MOVE, NO_MOVE, NO_MOVE },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },    // Return 1, 2 legs to ground
        {       80, NO_MOVE,      80, NO_MOVE, NO_MOVE, NO_MOVE },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE, NO_MOVE, NO_MOVE,       0, NO_MOVE,       0 },    // Up 4, 6 legs and return to start position
        {  NO_MOVE, NO_MOVE, NO_MOVE,      60, NO_MOVE,      60 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
        
        {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },    // Return 4, 6 legs to ground
        {  NO_MOVE, NO_MOVE, NO_MOVE,      80, NO_MOVE,      80 },
        {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }}
    },
    { SEQUENCE_BASE, SEQUENCE_DOWN, SEQUENCE_DIRECT_MOVEMENT, SEQUENCE_REVERSE_MOVEMENT, 
      SEQUENCE_ROTATE_LEFT, SEQUENCE_NONE }
};


#endif /* GAIT_SEQUENCES_H_ */