//  ***************************************************************************
/// @file    servo_driver.h
/// @author  NeoProg
/// @brief   Servo driver
//  ***************************************************************************
#ifndef SERVO_DRIVER_H_
#define SERVO_DRIVER_H_

#include <stdint.h>


#define SUPPORT_SERVO_COUNT						(18)


extern int8_t ram_servo_angle_override[SUPPORT_SERVO_COUNT];	// Write only
extern int8_t ram_servo_angle[SUPPORT_SERVO_COUNT];				// Read only

extern void servo_driver_init(void); 
extern void servo_driver_move(uint32_t ch, float angle);
extern void servo_driver_process(void);


#endif // SERVO_DRIVER_H_