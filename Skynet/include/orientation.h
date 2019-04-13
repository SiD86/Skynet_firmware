//  ***************************************************************************
/// @file    orientation.h
/// @author  NeoProg
/// @brief   Orientation subsystem
//  ***************************************************************************
#ifndef ORIENTATION_H_
#define ORIENTATION_H_

#include <stdint.h>


typedef struct {
	uint32_t front_distance;
} orientation_t;


extern orientation_t current_orientation;


void orientation_init(void);
void orientation_process(void);


#endif /* ORIENTATION_H_ */