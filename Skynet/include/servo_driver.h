//  ***************************************************************************
/// @file    servo_driver.h
/// @author  NeoProg
/// @brief   Servo driver
//  ***************************************************************************
#ifndef SERVO_DRIVER_H_
#define SERVO_DRIVER_H_

#include <stdbool.h>


#define SUPPORT_SERVO_COUNT						(18)


extern int8_t   ram_servo_dest_angle_override[SUPPORT_SERVO_COUNT];		// Read only
extern uint8_t  ram_servo_current_angle[SUPPORT_SERVO_COUNT];			// Write only


extern void servo_driver_init(void); 
extern void servo_driver_start_move(uint32_t ch, int32_t angle, uint32_t iteration_count);
extern void servo_driver_stop_move(uint32_t ch);
extern bool servo_driver_is_move_complete(void);
extern void servo_driver_process(void);


#endif // SERVO_DRIVER_H_