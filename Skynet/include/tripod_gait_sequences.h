//  ***************************************************************************
/// @file    tripod_gait_sequences.h
/// @author  NeoProg
/// @brief   Tripod gait sequences
//  ***************************************************************************
#ifndef TRIPOD_GAIT_SEQUENCES_H_
#define TRIPOD_GAIT_SEQUENCES_H_


#include "limbs_driver.h"


typedef struct {
    
    int32_t coxa[SUPPORT_LIMB_COUNT];
    int32_t femur[SUPPORT_LIMB_COUNT];
    int32_t tibia[SUPPORT_LIMB_COUNT];
    
} sequence_step_t;

typedef struct {
    
    uint32_t movement_iteration_count;     // Iteration count for limb movement complete
    
    uint32_t steps_count;                  // Sequence steps count
    uint32_t delay_between_steps;          // Delay between sequence steps, [ms]
    uint32_t begin_loop_step;              // Sequence step index witch begins loop
    sequence_step_t steps_list[20];        // Sequence steps list
    
} sequence_info_t;



static sequence_info_t sequence_down = { 50, 1, 400, 0, {
    
    {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
    {       45,      45,      45,      45,      45,      45 },
    {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
}
};

static sequence_info_t sequence_base = { 50, 4, 400, 3, {
    
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
    {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE}},
}
};

static sequence_info_t sequence_up = { 50, 6, 400, 5, {
    
    {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
    {       45,      45,      45,      45,      45,      45 },
    {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
    
    {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
    {       80,      80,      80,      80,      80,      80 },
    {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
    
    {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
    {       60, NO_MOVE,      60, NO_MOVE,      60, NO_MOVE },
    {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
    
    {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
    {       80, NO_MOVE,      80, NO_MOVE,      80, NO_MOVE },
    {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
    
    {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
    {  NO_MOVE,      60, NO_MOVE,      60, NO_MOVE,      60 },
    {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
    
    {{ NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE },
    {  NO_MOVE,      80, NO_MOVE,      80, NO_MOVE,      80 },
    {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }}
}
};

static sequence_info_t sequence_direct_movement = { 50, 8, 400, 4, {
    
    //
    // PREPARE SEQUENCE
    //
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
    
    //
    // PROCESS SEQUENCE
    //
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
    {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
}
};

static sequence_info_t sequence_rotate_left = { 50, 6, 400, 0, {
    
    //
    // PROCESS SEQUENCE
    //
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
    {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
}
};

static sequence_info_t sequence_rotate_right = { 50, 6, 400, 0, {
    
    //
    // PROCESS SEQUENCE
    //
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
    {  NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE }},
}
};


#endif /* TRIPOD_GAIT_SEQUENCES_H_ */