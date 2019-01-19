//  ***************************************************************************
/// @file    limbs_driver.h
/// @author  NeoProg
/// @brief   Hexapod limbs control
//  ***************************************************************************
#ifndef LIMB_H_
#define LIMB_H_

#include <stdbool.h>

#define SUPPORT_LIMB_COUNT				(6)
#define LIMB_NO_MOVE					(0xFFFF)


typedef enum {
	SPEED_FAST,
	SPEED_NORMAL,
	SPEED_LOW
} speed_t;

typedef struct {
	float x;
	float y;
	float z;
} point_3d_t;

typedef enum {
	PATH_LINEAR,
	PATH_ELLIPTICAL,
} path_type_t;


extern void limbs_driver_init(void);
extern void limbs_driver_start_move(const point_3d_t* point_list, const path_type_t* path_type_list);
extern void limbs_driver_stop_move(void);
extern void limbs_driver_process(void);
extern bool limbs_driver_is_move_complete(void);


#endif /* LIMB_H_ */