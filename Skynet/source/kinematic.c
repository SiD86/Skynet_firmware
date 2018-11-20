//  ***************************************************************************
/// @file    kinematic.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#include <fastmath.h>
#include "kinematic.h"
#define RAD_TO_DEG(rad)				((rad) * 180 / M_PI)
#define DEG_TO_RAD(deg)				((deg) * M_PI / 180)



//  ***************************************************************************
/// @brief  Translate coordinates of destination point to relative limb begin
/// @note	Move to (X*, Y*, Z*) coordinate system
/// @return none
//  ***************************************************************************
void kinematic_prepare(int32_t* x, int32_t* y, int32_t* z, const kinematic_config_t* config) {
	
	float coxa_zero_rotate_rad = DEG_TO_RAD(config->link_zero_rotate[LINK_COXA]);

	// Move to (X*, Y*, Z*) coordinate system - offset
	float x1 = *x - config->coxa_begin_x;
	float y1 = *y - config->coxa_begin_y;
	float z1 = *z - config->coxa_begin_z;

	// Move to (X*, Y*, Z*) coordinate system - rotate
	*x = x1 * cos(coxa_zero_rotate_rad) + z1 * sin(coxa_zero_rotate_rad);
	*y = y1;
	*z = -x1 * sin(coxa_zero_rotate_rad) + z1 * cos(coxa_zero_rotate_rad);
}

//  ***************************************************************************
/// @brief  Calculate angles
/// @param  x, y, z: coordinates of destination point
/// @param  config: kinematic configuration @ref kinematic_input_t
/// @param  link_angles: calculated angles
/// @retval	link_angles
/// @return true - calculation success, false - no
//  ***************************************************************************
bool kinematic_calculate_angles(int32_t x, int32_t y, int32_t z, const kinematic_config_t* config, int32_t* link_angles) {

	int32_t femur_zero_rotate_deg = config->link_zero_rotate[LINK_FEMUR];
	int32_t tibia_zero_rotate_deg = config->link_zero_rotate[LINK_TIBIA];
	uint32_t coxa_length = config->link_length[LINK_COXA];
	uint32_t femur_length = config->link_length[LINK_FEMUR];
	uint32_t tibia_length = config->link_length[LINK_TIBIA];


	//
	// Calculate COXA angle
	//
	float coxa_angle_rad = atan2(z, x);
	link_angles[LINK_COXA] = RAD_TO_DEG(coxa_angle_rad);


	//
	// Prepare for calculation FEMUR and TIBIA angles
	//
	// Move to (X*, Y*) coordinate system (rotate on axis Y)
	x = x * cos(coxa_angle_rad) + z * sin(coxa_angle_rad);

	// Move to (X**, Y**) coordinate system (remove coxa from calculations)
	x = x - coxa_length;
	
	// Calculate angle between axis X and destination point
	float fi = atan2(y, x);

	// Calculate distance to destination point
	float d = sqrt(x * x + y * y);
	if (d > femur_length + tibia_length) {
		return false; // Point not attainable
	}
	
	
	//
	// Calculate triangle angles
	//
	float a = tibia_length;
	float b = femur_length;
	float c = d;

	// Calculate angles of triangle
	float alpha = acos( (b * b + c * c - a * a) / (2 * b * c) );
	float gamma = acos( (a * a + b * b - c * c) / (2 * a * b) );


	//
	// Calculate FEMUR and TIBIA angle
	//
	link_angles[LINK_FEMUR] = femur_zero_rotate_deg - RAD_TO_DEG(alpha) - RAD_TO_DEG(fi);
	link_angles[LINK_TIBIA] = RAD_TO_DEG(gamma) - tibia_zero_rotate_deg;



	// Check angles
	if (link_angles[LINK_COXA]  < config->link_min_angle[LINK_COXA] || link_angles[LINK_COXA]  > config->link_max_angle[LINK_COXA] ||
	    link_angles[LINK_FEMUR] < config->link_min_angle[LINK_FEMUR] || link_angles[LINK_FEMUR] > config->link_max_angle[LINK_FEMUR] ||
	    link_angles[LINK_TIBIA] < config->link_min_angle[LINK_TIBIA] || link_angles[LINK_TIBIA] > config->link_max_angle[LINK_TIBIA]) {

		return false;
	}

	return true;
}
