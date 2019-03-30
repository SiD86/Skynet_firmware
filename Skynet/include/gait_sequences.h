//  ***************************************************************************
/// @file    gait_sequences.h
/// @author  NeoProg
/// @brief   Gait sequences
//  ***************************************************************************
#ifndef GAIT_SEQUENCES_H_
#define GAIT_SEQUENCES_H_

#include "limbs_driver.h"
#include "movement_engine.h"


#define GAIT_SEQUENCE_HEIGHT_LOW_LIMIT			(85)
#define GAIT_SEQUENCE_HEIGHT_HIGH_LIMIT			(185)
#define GAIT_SEQUENCE_LIMB_UP_STEP_HEIGHT		(30)

typedef enum {
	LIMB_STATE_UP,
	LIMB_STATE_DOWN,
	LIMB_STATE_CUSTOM
} limb_state_t;

typedef struct {
	limb_state_t limb_state_list[SUPPORT_LIMB_COUNT];
	point_3d_t   point_list[SUPPORT_LIMB_COUNT];
	path_type_t  path_list[SUPPORT_LIMB_COUNT];
	uint32_t     smooth_point_count;
} sequence_iteration_t;

typedef struct {
	
    bool     is_sequence_looped;
	uint32_t main_sequence_begin;
	uint32_t finalize_sequence_begin;

    uint32_t total_iteration_count;
    sequence_iteration_t iteration_list[15];					// Sequence iterations list
	
} sequence_info_t;


static sequence_info_t sequence_update_height = {
	
	.is_sequence_looped      = false,
	.main_sequence_begin     = 0,
	.finalize_sequence_begin = 5,
	.total_iteration_count   = 5,
	
	{
		{   // Move to new height
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR }, 15
		},
		{	// Up 0, 2, 4 legs
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{95, -55, 95}, {125, -85, 0}, {95, -55, -95}, {88, -85, 88}, {135, -55, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR }, 30
		},
		{	// Down 0, 2, 4 legs
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR }, 30
		},
		{	// Up 1, 3, 5 legs
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{88, -85, 88}, {135, -55, 0}, {88, -85, -88}, {95, -55, 95}, {125, -85, 0}, {95, -55, -95}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR }, 30
		},
		{   // Down 0, 2, 4 legs
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR }, 30
		},
	}
};

static sequence_info_t sequence_down = {  
    
    .is_sequence_looped      = false,
    .main_sequence_begin     = 0,
    .finalize_sequence_begin = 1,
    .total_iteration_count   = 1,
	
    {
        { 
			{ LIMB_STATE_CUSTOM, LIMB_STATE_CUSTOM, LIMB_STATE_CUSTOM, LIMB_STATE_CUSTOM, LIMB_STATE_CUSTOM, LIMB_STATE_CUSTOM },
			{{100, -35, 100}, {142, -35, 0}, {100, -35, -100}, {100, -35, 100}, {142, -35, 0}, {100, -35, -100}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR }, 80
		},	
    }
};

static sequence_info_t sequence_up = {

	.is_sequence_looped      = false,
	.main_sequence_begin     = 0,
	.finalize_sequence_begin = 5,
    .total_iteration_count   = 5,
     
	{
		{   // Down all legs
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}}, 
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 15
		},
		{	// Up 0, 2, 4 legs
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{95, -55, 95}, {125, -85, 0}, {95, -55, -95}, {88, -85, 88}, {135, -55, 0}, {88, -85, -88}}, 
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{	// Down 0, 2, 4 legs
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{	// Up 1, 3, 5 legs
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{88, -85, 88}, {135, -55, 0}, {88, -85, -88}, {95, -55, 95}, {125, -85, 0}, {95, -55, -95}}, 
		    { PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{   // Down 0, 2, 4 legs
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}}, 
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
	}
};

static sequence_info_t sequence_direct_movement = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 4,
	.finalize_sequence_begin = 6,
    .total_iteration_count   = 10,
     
	{
		//
		// Prepare sequence
		//
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{110, -55, 110}, {125, -85,   0}, {110, -55, -110}, { 88, -85,  88}, {150, -55,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
	    },	
		{ 
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{110, -85,  20}, {125, -85,   0}, {110, -85, -110}, { 88, -85,  88}, {110, -85, -45}, { 88, -85,  -88}},
		    { PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
	    },	
		{ 
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{110, -85,  20}, {150, -55,   0}, {110, -85, -110}, {110, -55, 110}, {110, -85, -45}, {110, -55, -110}},
		    { PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},		
		{ 
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{110, -85,  20}, {110, -85,  45}, {110, -85, -110}, {110, -85, 110}, {110, -85, -45}, {110, -85,  -20}},
		    { PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},	
				
		//
		// Main sequence
		//
		{ 
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{130, -55, 110}, {110, -85, -45}, {130, -55,  -20}, {110, -85,  20}, {130, -55,  45}, {110, -85, -110}},
		    { PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR}, 50
		},	
		{ 
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{110, -85,  20}, {130, -55,  45}, {110, -85, -110}, {130, -55, 110}, {110, -85, -45}, {130, -55,  -20}}, 
		    { PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS}, 50
		},	

		//
		// Finalize sequence
		//
		{ 
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{110, -85,  20}, {150, -55,   0}, {110, -85, -110}, {110, -55, 110}, {110, -85, -45}, {110, -55, -110}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{ 
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{110, -85,  20}, {125, -85,   0}, {110, -85, -110}, { 88, -85,  88}, {110, -85, -45}, { 88, -85,  -88}}, 
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{ 
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{110, -55, 110}, {125, -85,   0}, {110, -55, -110}, { 88, -85,  88}, {150, -55,   0}, { 88, -85,  -88}}, 
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{ 
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{ 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}, { 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}}, 
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
	}
};

static sequence_info_t sequence_reverse_movement = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 4,
	.finalize_sequence_begin = 6,
	.total_iteration_count   = 10,
	
	{
		//
		// Prepare sequence
		//
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{110, -55, 110}, {125, -85,   0}, {110, -55, -110}, { 88, -85,  88}, {150, -55,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{110, -85,  20}, {125, -85,   0}, {110, -85, -110}, { 88, -85,  88}, {110, -85, -45}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{110, -85,  20}, {150, -55,   0}, {110, -85, -110}, {110, -55, 110}, {110, -85, -45}, {110, -55, -110}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{110, -85,  20}, {110, -85,  45}, {110, -85, -110}, {110, -85, 110}, {110, -85, -45}, {110, -85,  -20}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		
		//
		// Main sequence
		//
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{110, -85, 110}, {130, -55, -45}, {110, -85, -20}, {130, -55,  20}, {110, -85,  45}, {130, -55, -110}},
			{ PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS}, 50
		},
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{130, -55,  20}, {110, -85, 45}, {130, -55, -110}, {110, -85, 110}, {130, -55, -45}, {110, -85, -20}},
			{ PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR}, 50
		},
		

		//
		// Finalize sequence
		//
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{110, -85,  20}, {150, -55, 0}, {110, -85, -110}, {110, -55, 110}, {110, -85, -45}, {110, -55, -110}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{110, -85,  20}, {125, -85, 0}, {110, -85, -110}, { 88, -85,  88}, {110, -85, -45}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{110, -55, 110}, {125, -85, 0}, {110, -55, -110}, { 88, -85,  88}, {150, -55,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{ 88, -85,  88}, {125, -85, 0}, { 88, -85,  -88}, { 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
	}
};

static sequence_info_t sequence_rotate_left = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 0,
	.finalize_sequence_begin = 2,
	.total_iteration_count   = 2,
	
	{
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{121, -55, 32}, {108, -85, 62}, {32, -55, -121}, {121, -85,  32}, {108, -55,  62}, {32, -85, -121}},
			{ PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR}, 50
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{88, -85,  88}, {125, -55, 0}, {88, -85, -88}, {88, -55, 88}, {125, -85, 0}, {88, -55, -88}},
			{ PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS}, 50
		},
	}
};

static sequence_info_t sequence_rotate_right = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 0,
	.finalize_sequence_begin = 2,
	.total_iteration_count   = 2,
	
	{
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{32, -55, 121}, {108, -85, -62}, {121, -55, -32}, {32, -85, 121}, {108, -55, -62}, {121, -85, -32}},
			{ PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR}, 50
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{88, -85,  88}, {125, -55, 0}, {88, -85, -88}, {88, -55, 88}, {125, -85, 0}, {88, -55, -88}},
			{ PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS}, 50
		},
	}
};

static sequence_info_t sequence_direct_movement_short = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 4,
	.finalize_sequence_begin = 6,
	.total_iteration_count   = 10,
	
	{
		//
		// Prepare sequence
		//
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{110, -55, 110}, {125, -85,   0}, {110, -55, -110}, { 88, -85,  88}, {150, -55,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{110, -85,  60}, {125, -85,   0}, {110, -85, -110}, { 88, -85,  88}, {110, -85, -25}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{110, -85,  60}, {150, -55,   0}, {110, -85, -110}, {110, -55, 110}, {110, -85, -25}, {110, -55, -110}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{110, -85,  60}, {110, -85,  45}, {110, -85, -110}, {110, -85, 110}, {110, -85, -25}, {110, -85,  -60}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		
		//
		// Main sequence
		//
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{130, -55, 110}, {110, -85, -25}, {130, -55,  -60}, {110, -85,  60}, {130, -55,  25}, {110, -85, -110}},
			{ PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR}, 50
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{110, -85,  60}, {130, -55,  25}, {110, -85, -110}, {130, -55, 110}, {110, -85, -25}, {130, -55,  -60}},
			{ PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS}, 50
		},

		//
		// Finalize sequence
		//
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{110, -85,  60}, {150, -55,   0}, {110, -85, -110}, {110, -55, 110}, {110, -85, -25}, {110, -55, -110}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{110, -85,  60}, {125, -85,   0}, {110, -85, -110}, { 88, -85,  88}, {110, -85, -25}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{110, -55, 110}, {125, -85,   0}, {110, -55, -110}, { 88, -85,  88}, {150, -55,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{ 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}, { 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
	}
};

static sequence_info_t sequence_reverse_movement_short = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 4,
	.finalize_sequence_begin = 6,
	.total_iteration_count   = 10,
	
	{
		//
		// Prepare sequence
		//
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{110, -55, 110}, {125, -85,   0}, {110, -55, -110}, { 88, -85,  88}, {150, -55,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{110, -85,  60}, {125, -85,   0}, {110, -85, -110}, { 88, -85,  88}, {110, -85, -25}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{110, -85,  60}, {150, -55,   0}, {110, -85, -110}, {110, -55, 110}, {110, -85, -25}, {110, -55, -110}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{110, -85,  60}, {110, -85,  25}, {110, -85, -110}, {110, -85, 110}, {110, -85, -25}, {110, -85,  -60}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		
		//
		// Main sequence
		//
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{110, -85, 110}, {130, -55, -25}, {110, -85, -60}, {130, -55,  60}, {110, -85,  25}, {130, -55, -110}},
			{ PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS}, 50
		},
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{130, -55,  60}, {110, -85, 25}, {130, -55, -110}, {110, -85, 110}, {130, -55, -25}, {110, -85, -60}},
			{ PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR}, 50
		},
		

		//
		// Finalize sequence
		//
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{110, -85,  60}, {150, -55, 0}, {110, -85, -110}, {110, -55, 110}, {110, -85, -25}, {110, -55, -110}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{110, -85,  60}, {125, -85, 0}, {110, -85, -110}, { 88, -85,  88}, {110, -85, -25}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{110, -55, 110}, {125, -85, 0}, {110, -55, -110}, { 88, -85,  88}, {150, -55,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{ 88, -85,  88}, {125, -85, 0}, { 88, -85,  -88}, { 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
	}
};

static sequence_info_t sequence_rotate_left_short = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 0,
	.finalize_sequence_begin = 2,
	.total_iteration_count   = 2,
	
	{
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{108, -55, 62}, {120, -85, 32}, {62, -55, -108}, {108, -85, 62}, {120, -55, 32}, {62, -85, -108}},
			{ PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR}, 50
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{88, -85, 88}, {125, -55, 0}, {88, -85, -88}, {88, -55, 88}, {125, -85, 0}, {88, -55, -88}},
			{ PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS}, 50
		},
	}
};

static sequence_info_t sequence_rotate_right_short = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 0,
	.finalize_sequence_begin = 2,
	.total_iteration_count   = 2,
	
	{
		{
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{62, -55, 108}, {125, -85, -32}, {108, -55, -62}, {62, -85, 108}, {125, -55, -32}, {108, -85, -62}},
			{ PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR}, 50
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{88, -85,  88}, {125, -55, 0}, {88, -85, -88}, {88, -55, 88}, {125, -85, 0}, {88, -55, -88}},
			{ PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS}, 50
		},
	}
};

static sequence_info_t sequence_attack_left = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 1,
	.finalize_sequence_begin = 3,
	.total_iteration_count   = 4,
	
	{
		//
		// Prepare sequence
		//
		{
			{ LIMB_STATE_CUSTOM, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{   0,   0, 150}, {125, -85,   0}, { 88, -85,  -88}, { 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 50
		},
		
		//
		// Main sequence
		//
		{
			{ LIMB_STATE_CUSTOM, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{   0,  50, 250}, {125, -85,   0}, { 88, -85,  -88}, { 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 50
		},
		{
			{ LIMB_STATE_CUSTOM, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{   0,   0, 150}, {125, -85,   0}, { 88, -85,  -88}, { 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 50
		},

		//
		// Finalize sequence
		//
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{ 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}, { 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 50
		},
	}
};

static sequence_info_t sequence_attack_right = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 1,
	.finalize_sequence_begin = 3,
	.total_iteration_count   = 4,
	
	{
		//
		// Prepare sequence
		//
		{
			{ LIMB_STATE_CUSTOM, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{ {88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {0, 0, 150}, {125, -85, 0}, { 88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 50
		},
		
		//
		// Main sequence
		//
		{
			{ LIMB_STATE_CUSTOM, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{ 88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {0, 50, 250}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 50
		},
		{
			{ LIMB_STATE_CUSTOM, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{ {88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {0, 0, 150}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 50
		},

		//
		// Finalize sequence
		//
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{ {88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 50
		},
	}
};

static sequence_info_t sequence_dance = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 0,
	.finalize_sequence_begin = 12,
	.total_iteration_count   = 12,
	
	{
		{	// Up 0, 2, 4 legs
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{95, -55, 95}, {125, -85, 0}, {95, -55, -95}, {88, -85, 88}, {135, -55, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{	// Down 0, 2, 4 legs
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{	// Up 1, 3, 5 legs
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{88, -85, 88}, {135, -55, 0}, {88, -85, -88}, {95, -55, 95}, {125, -85, 0}, {95, -55, -95}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{   // Down 0, 2, 4 legs
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		
		{
			{ LIMB_STATE_CUSTOM, LIMB_STATE_DOWN, LIMB_STATE_CUSTOM, LIMB_STATE_DOWN, LIMB_STATE_CUSTOM, LIMB_STATE_DOWN },
			{{ 170,  50, 170}, {125, -85,   0}, { 170, 50, -170}, { 88, -85,  88}, {240, 50,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 70
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{ 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}, { 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 70
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_CUSTOM, LIMB_STATE_DOWN, LIMB_STATE_CUSTOM, LIMB_STATE_DOWN, LIMB_STATE_CUSTOM },
			{{ 88, -85,  88}, {240, 50,   0}, { 88, -85,  -88}, { 170, 50, 170}, {125, -85,   0}, {170, 50, -170}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 70
		},
		{
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{ 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}, { 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 70
		},
		
		{	// Up 0, 2, 4 legs
			{ LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN },
			{{95, -55, 95}, {125, -85, 0}, {95, -55, -95}, {88, -85, 88}, {135, -55, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{	// Down 0, 2, 4 legs
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{	// Up 1, 3, 5 legs
			{ LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP, LIMB_STATE_DOWN, LIMB_STATE_UP },
			{{88, -85, 88}, {135, -55, 0}, {88, -85, -88}, {95, -55, 95}, {125, -85, 0}, {95, -55, -95}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{   // Down 0, 2, 4 legs
			{ LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN, LIMB_STATE_DOWN },
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		}
	}
};

#endif /* GAIT_SEQUENCES_H_ */