//  ***************************************************************************
/// @file    kinematic.h
/// @author  NeoProg
/// @brief   Inverse kinematic interface
//  ***************************************************************************
#ifndef KINEMATIC_H_
#define KINEMATIC_H_

#include <stdbool.h>


typedef enum {
	LINK_COXA  = 0,
	LINK_FEMUR = 1,
	LINK_TIBIA = 2	
} kinematic_link_t;

typedef struct {
	
	int32_t coxa_begin_x;
	int32_t coxa_begin_y;
	int32_t coxa_begin_z;
	
	uint32_t link_length[3];
	int32_t  link_zero_rotate[3];
	int32_t  link_min_angle[3];
	int32_t  link_max_angle[3];
	
} kinematic_config_t;


extern void kinematic_prepare(int32_t* x, int32_t* y, int32_t* z, const kinematic_config_t* config);
extern bool kinematic_calculate_angles(int32_t x, int32_t y, int32_t z, const kinematic_config_t* config, int32_t* link_angles);

#endif /* KINEMATIC_H_ */