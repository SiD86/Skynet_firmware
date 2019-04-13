//  ***************************************************************************
/// @file    orientation.c
/// @author  NeoProg
//  ***************************************************************************
#include "orientation.h"

#include <sam.h>
#include "systimer.h"
#include "hc-sr04.h"
#include "error_handling.h"


orientation_t current_orientation;


//  ***************************************************************************
/// @brief  Initialization orientation subsystem
/// @return none
//  ***************************************************************************
void orientation_init(void) {
	
	hcsr04_init();
}

//  ***************************************************************************
/// @brief  Process orientation subsystem
/// @return none
//  ***************************************************************************
void orientation_process(void) {
	
	hcsr04_process();
	current_orientation.front_distance = hcsr04_get_distance();
}