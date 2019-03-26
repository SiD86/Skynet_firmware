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
#define GAIT_SEQUENCE_HEIGHT_HIGH_LIMIT			(150)


typedef struct {
	point_3d_t  point_list[SUPPORT_LIMB_COUNT];
	path_type_t path_list[SUPPORT_LIMB_COUNT];
	uint32_t    smooth_point_count;
} sequence_iteration_t;

typedef struct {
	
    bool     is_sequence_looped;
	uint32_t main_sequence_begin;
	uint32_t finalize_sequence_begin;

    uint32_t total_iteration_count;
    sequence_iteration_t iteration_list[15];					// Sequence iterations list
    sequence_id_t available_sequences[SUPPORT_SEQUENCE_COUNT];	// Available sequence id`s list to which can go from this sequence
	
} sequence_info_t;


static sequence_info_t sequence_change_height = {
	
	.is_sequence_looped      = false,
	.main_sequence_begin     = 0,
	.finalize_sequence_begin = 5,
	.total_iteration_count   = 5,
	
	{
		{   // Move to new height
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 5
		},
		{	// Up 0, 2, 4 legs
			{{95, -55, 95}, {125, -85, 0}, {95, -55, -95}, {88, -85, 88}, {135, -55, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{	// Down 0, 2, 4 legs
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{	// Up 1, 3, 5 legs
			{{88, -85, 88}, {135, -55, 0}, {88, -85, -88}, {95, -55, 95}, {125, -85, 0}, {95, -55, -95}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{   // Down 0, 2, 4 legs
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
	},
	
	{ SEQUENCE_NONE }
};

static const sequence_info_t sequence_down = {  
    
    .is_sequence_looped      = false,
    .main_sequence_begin     = 0,
    .finalize_sequence_begin = 1,
    .total_iteration_count   = 1,
	
    {
        { 
			{{100, -35, 100}, {142, -35, 0}, {100, -35, -100}, {100, -35, 100}, {142, -35, 0}, {100, -35, -100}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 80
		},	
    },
	
    { SEQUENCE_UP, SEQUENCE_NONE }
};

static sequence_info_t sequence_up = {

	.is_sequence_looped      = false,
	.main_sequence_begin     = 0,
	.finalize_sequence_begin = 5,
    .total_iteration_count   = 5,
     
	{
		{   // Down all legs
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}}, 
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 15
		},
		{	// Up 0, 2, 4 legs
			{{95, -55, 95}, {125, -85, 0}, {95, -55, -95}, {88, -85, 88}, {135, -55, 0}, {88, -85, -88}}, 
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{	// Down 0, 2, 4 legs
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{	// Up 1, 3, 5 legs
			{{88, -85, 88}, {135, -55, 0}, {88, -85, -88}, {95, -55, 95}, {125, -85, 0}, {95, -55, -95}}, 
		    { PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
		{   // Down 0, 2, 4 legs
			{{88, -85, 88}, {125, -85, 0}, {88, -85, -88}, {88, -85, 88}, {125, -85, 0}, {88, -85, -88}}, 
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 30
		},
	},
     
    { SEQUENCE_DOWN, SEQUENCE_DIRECT_MOVEMENT, SEQUENCE_REVERSE_MOVEMENT, SEQUENCE_ROTATE_LEFT, SEQUENCE_ROTATE_RIGHT, SEQUENCE_NONE }
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
			{{110, -55, 110}, {125, -85,   0}, {110, -55, -110}, { 88, -85,  88}, {150, -55,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
	    },	
		{ 
			{{110, -85,  20}, {125, -85,   0}, {110, -85, -110}, { 88, -85,  88}, {110, -85, -45}, { 88, -85,  -88}},
		    { PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
	    },	
		{ 
			{{110, -85,  20}, {150, -55,   0}, {110, -85, -110}, {110, -55, 110}, {110, -85, -45}, {110, -55, -110}},
		    { PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},		
		{ 
			{{110, -85,  20}, {110, -85,  45}, {110, -85, -110}, {110, -85, 110}, {110, -85, -45}, {110, -88,  -20}},
		    { PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},	
				
		//
		// Main sequence
		//
		{ 
			{{130, -55, 110}, {110, -85, -45}, {130, -55,  -20}, {110, -85,  20}, {130, -55,  45}, {110, -85, -110}},
		    { PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR}, 50
		},	
		{ 
			{{110, -85,  20}, {130, -55,  45}, {110, -85, -110}, {130, -55, 110}, {110, -85, -45}, {130, -55,  -20}}, 
		    { PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS}, 50
		},	

		//
		// Finalize sequence
		//
		{ 
			{{110, -85,  20}, {150, -55,   0}, {110, -85, -110}, {110, -55, 110}, {110, -85, -45}, {110, -55, -110}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{ 
			{{110, -85,  20}, {125, -85,   0}, {110, -85, -110}, { 88, -85,  88}, {110, -85, -45}, { 88, -85,  -88}}, 
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{ 
			{{110, -55, 110}, {125, -85,   0}, {110, -55, -110}, { 88, -85,  88}, {150, -55,   0}, { 88, -85,  -88}}, 
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{ 
			{{ 88, -85,  88}, {125, -85,   0}, { 88, -85,  -88}, { 88, -85,  88}, {125, -85,   0}, { 88, -88,  -88}}, 
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
	},
     
    { SEQUENCE_DOWN, SEQUENCE_DIRECT_MOVEMENT, SEQUENCE_REVERSE_MOVEMENT, SEQUENCE_ROTATE_LEFT, SEQUENCE_ROTATE_RIGHT, SEQUENCE_NONE }
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
			{{110, -55, 110}, {125, -85,   0}, {110, -55, -110}, { 88, -85,  88}, {150, -55,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{{110, -85,  20}, {125, -85,   0}, {110, -85, -110}, { 88, -85,  88}, {110, -85, -45}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{{110, -85,  20}, {150, -55,   0}, {110, -85, -110}, {110, -55, 110}, {110, -85, -45}, {110, -55, -110}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{{110, -85,  20}, {110, -85,  45}, {110, -85, -110}, {110, -85, 110}, {110, -85, -45}, {110, -88,  -20}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		
		//
		// Main sequence
		//
		{
			{{110, -85, 110}, {130, -55, -45}, {110, -85, -20}, {130, -55,  20}, {110, -85,  45}, {130, -55, -110}},
			{ PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS}, 50
		},
		{
			{{130, -55,  20}, {110, -85, 45}, {130, -55, -110}, {110, -85, 110}, {130, -55, -45}, {110, -85, -20}},
			{ PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR, PATH_XZ_ELLIPTICAL_Y_SINUS, PATH_LINEAR}, 50
		},
		

		//
		// Finalize sequence
		//
		{
			{{110, -85,  20}, {150, -55, 0}, {110, -85, -110}, {110, -55, 110}, {110, -85, -45}, {110, -55, -110}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{{110, -85,  20}, {125, -85, 0}, {110, -85, -110}, { 88, -85,  88}, {110, -85, -45}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{{110, -55, 110}, {125, -85, 0}, {110, -55, -110}, { 88, -85,  88}, {150, -55,   0}, { 88, -85,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
		{
			{{ 88, -85,  88}, {125, -85, 0}, { 88, -85,  -88}, { 88, -85,  88}, {125, -85,   0}, { 88, -88,  -88}},
			{ PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, PATH_LINEAR}, 20
		},
	},
	
	{ SEQUENCE_DOWN, SEQUENCE_DIRECT_MOVEMENT, SEQUENCE_REVERSE_MOVEMENT, SEQUENCE_ROTATE_LEFT, SEQUENCE_ROTATE_RIGHT, SEQUENCE_NONE }
};

static sequence_info_t sequence_rotate_left = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 0,
	.finalize_sequence_begin = 2,
	.total_iteration_count   = 2,
	
	{
		{
			{{121, -55, 32}, {108, -85, 62}, {32, -55, -121}, {121, -85,  32}, {108, -55,  62}, {32, -85, -121}},
			{ PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR}, 50
		},
		{
			{{88, -85,  88}, {125, -55, 0}, {88, -85, -88}, {88, -55, 88}, {125, -85, 0}, {88, -55, -88}},
			{ PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS}, 50
		},
	},
	
	{ SEQUENCE_DOWN, SEQUENCE_DIRECT_MOVEMENT, SEQUENCE_REVERSE_MOVEMENT, SEQUENCE_ROTATE_LEFT, SEQUENCE_ROTATE_RIGHT, SEQUENCE_NONE }
};

static sequence_info_t sequence_rotate_right = {

	.is_sequence_looped      = true,
	.main_sequence_begin     = 0,
	.finalize_sequence_begin = 2,
	.total_iteration_count   = 2,
	
	{
		{
			{{32, -55, 121}, {108, -85, -62}, {121, -55, -32}, {32, -85, 121}, {108, -55, -62}, {121, -85, -32}},
			{ PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR}, 50
		},
		{
			{{88, -85,  88}, {125, -55, 0}, {88, -85, -88}, {88, -55, 88}, {125, -85, 0}, {88, -55, -88}},
			{ PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS, PATH_XZ_ARC_Y_LINEAR, PATH_XZ_ARC_Y_SINUS}, 50
		},
	},
	
	{ SEQUENCE_DOWN, SEQUENCE_DIRECT_MOVEMENT, SEQUENCE_REVERSE_MOVEMENT, SEQUENCE_ROTATE_LEFT, SEQUENCE_ROTATE_RIGHT, SEQUENCE_NONE }
};


#endif /* GAIT_SEQUENCES_H_ */