//  ***************************************************************************
/// @file    limbs_driver.h
/// @author  NeoProg
/// @brief   Hexapod limbs control
//  ***************************************************************************
#ifndef LIMB_H_
#define LIMB_H_

#include <stdbool.h>

#define SUPPORT_LIMB_COUNT				(6)
#define NO_MOVE							(0xFFFF)

typedef enum {
	
	LIMB_FRONT_LEFT_LEG,
	LIMB_CENTER_LEFT_LEG,
	LIMB_REAR_LEFT_LEG,
	
	LIMB_FRONT_RIGHT_LEG,
	LIMB_CENTER_RIGHT_LEG,
	LIMB_REAR_RIGHT_LEG,
	
} limb_t;


void limbs_driver_init(void);
void limbs_driver_start_move(limb_t limb, int32_t coxa, int32_t femur, int32_t tibia, uint32_t iteration_count);
void limbs_driver_stop_move(limb_t limb);
bool limbs_driver_is_move_complete(void);


#endif /* LIMB_H_ */