//  ***************************************************************************
/// @file    src.c
/// @author  NeoProg
//  ***************************************************************************
#include "scr.h"

#include <sam.h>
#include "movement_driver.h"
#include "limbs_driver.h"
#include "servo_driver.h"
#include "veeprom.h"
#include "gui.h"
#include "led.h"
#include "monitoring.h"

#define SCR_CMD_CALCULATE_CHECKSUM				(0x01)

#define SCR_CMD_SELECT_GAIT_UP					(0x02)
#define SCR_CMD_SELECT_GAIT_BASE				(0x03)
#define SCR_CMD_SELECT_GAIT_DOWN				(0x04)
#define SCR_CMD_SELECT_GAIT_DIRECT_MOVEMENT		(0x05)
#define SCR_CMD_SELECT_GAIT_REVERSE_MOVEMENT	(0x06)
#define SCR_CMD_SELECT_GAIT_ROTATE_LEFT			(0x07)
#define SCR_CMD_SELECT_GAIT_ROTATE_RIGHT		(0x08)

#define SCR_CMD_RESET							(0xAA)


uint8_t scr = 0;
int16_t x = 0;
int16_t y = 0;
int16_t z = 0;

path_type_t path_type_list[6] = { PATH_LINEAR, PATH_LINEAR, PATH_LINEAR, 
                                  PATH_LINEAR, PATH_LINEAR, PATH_LINEAR };

point_3d_t point_list_1[6] = { { 0, 0, 0}, {0, 0, 0}, {0, 0, 0},
							   { 0, 0, 0}, {0, 0, 0}, {0, 0, 0} };

//  ***************************************************************************
/// @brief	Special command register process
/// @return	none
//  ***************************************************************************

// 20 10 - fast
// 35 5 - normal
// 45 5 - lower

void dummy_loop(void) {
	
	while (limbs_driver_is_move_complete() == false) {
		
		gui_process();
		led_process();
		
		servo_driver_process();
		limbs_driver_process();
		
		monitoring_process();
	}
}

void get_up(void) {
	
	limbs_driver_set_smooth_config(30);
	
	path_type_list[0] = PATH_LINEAR;
	point_list_1[0].x = +88;
	point_list_1[0].y = -85;
	point_list_1[0].z = +88;
	
	path_type_list[1] = PATH_LINEAR;
	point_list_1[1].x = +125;
	point_list_1[1].y = -85;
	point_list_1[1].z = 0;
	
	path_type_list[2] = PATH_LINEAR;
	point_list_1[2].x = +88;
	point_list_1[2].y = -85;
	point_list_1[2].z = -88;
	
	path_type_list[3] = PATH_LINEAR;
	point_list_1[3].x = +88;
	point_list_1[3].y = -85;
	point_list_1[3].z = +88;
	
	path_type_list[4] = PATH_LINEAR;
	point_list_1[4].x = +125;
	point_list_1[4].y = -85;
	point_list_1[4].z = 0;
	
	path_type_list[5] = PATH_LINEAR;
	point_list_1[5].x = +88;
	point_list_1[5].y = -85;
	point_list_1[5].z = -88;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
	
	//
	// Up 0, 2, 4 legs
	//
	path_type_list[0] = PATH_LINEAR;
	point_list_1[0].x = +95;
	point_list_1[0].y = -55;
	point_list_1[0].z = +95;
	
	path_type_list[1] = PATH_LINEAR;
	point_list_1[1].x = +125;
	point_list_1[1].y = -85;
	point_list_1[1].z = 0;
	
	path_type_list[2] = PATH_LINEAR;
	point_list_1[2].x = +95;
	point_list_1[2].y = -55;
	point_list_1[2].z = -95;
	
	path_type_list[3] = PATH_LINEAR;
	point_list_1[3].x = +88;
	point_list_1[3].y = -85;
	point_list_1[3].z = +88;
	
	path_type_list[4] = PATH_LINEAR;
	point_list_1[4].x = +135;
	point_list_1[4].y = -55;
	point_list_1[4].z = 0;
	
	path_type_list[5] = PATH_LINEAR;
	point_list_1[5].x = +88;
	point_list_1[5].y = -85;
	point_list_1[5].z = -88;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
	
	//
	// Down 0, 2, 4 legs
	//
	path_type_list[0] = PATH_LINEAR;
	point_list_1[0].x = +88;
	point_list_1[0].y = -85;
	point_list_1[0].z = +88;
	
	path_type_list[1] = PATH_LINEAR;
	point_list_1[1].x = +125;
	point_list_1[1].y = -85;
	point_list_1[1].z = 0;
	
	path_type_list[2] = PATH_LINEAR;
	point_list_1[2].x = +88;
	point_list_1[2].y = -85;
	point_list_1[2].z = -88;
	
	path_type_list[3] = PATH_LINEAR;
	point_list_1[3].x = +88;
	point_list_1[3].y = -85;
	point_list_1[3].z = +88;
	
	path_type_list[4] = PATH_LINEAR;
	point_list_1[4].x = +125;
	point_list_1[4].y = -85;
	point_list_1[4].z = 0;
	
	path_type_list[5] = PATH_LINEAR;
	point_list_1[5].x = +88;
	point_list_1[5].y = -85;
	point_list_1[5].z = -88;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
	
	//
	// Up 1, 3, 5 legs
	//
	path_type_list[0] = PATH_LINEAR;
	point_list_1[0].x = +88;
	point_list_1[0].y = -85;
	point_list_1[0].z = +88;
	
	path_type_list[1] = PATH_LINEAR;
	point_list_1[1].x = +135;
	point_list_1[1].y = -55;
	point_list_1[1].z = 0;
	
	path_type_list[2] = PATH_LINEAR;
	point_list_1[2].x = +88;
	point_list_1[2].y = -85;
	point_list_1[2].z = -88;
	
	path_type_list[3] = PATH_LINEAR;
	point_list_1[3].x = +95;
	point_list_1[3].y = -55;
	point_list_1[3].z = +95;
	
	path_type_list[4] = PATH_LINEAR;
	point_list_1[4].x = +125;
	point_list_1[4].y = -85;
	point_list_1[4].z = 0;
	
	path_type_list[5] = PATH_LINEAR;
	point_list_1[5].x = +95;
	point_list_1[5].y = -55;
	point_list_1[5].z = -95;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
	
	//
	// Up 1, 3, 5 legs
	//
	path_type_list[0] = PATH_LINEAR;
	point_list_1[0].x = +88;
	point_list_1[0].y = -85;
	point_list_1[0].z = +88;
	
	path_type_list[1] = PATH_LINEAR;
	point_list_1[1].x = +125;
	point_list_1[1].y = -85;
	point_list_1[1].z = 0;
	
	path_type_list[2] = PATH_LINEAR;
	point_list_1[2].x = +88;
	point_list_1[2].y = -85;
	point_list_1[2].z = -88;
	
	path_type_list[3] = PATH_LINEAR;
	point_list_1[3].x = +88;
	point_list_1[3].y = -85;
	point_list_1[3].z = +88;
	
	path_type_list[4] = PATH_LINEAR;
	point_list_1[4].x = +125;
	point_list_1[4].y = -85;
	point_list_1[4].z = 0;
	
	path_type_list[5] = PATH_LINEAR;
	point_list_1[5].x = +88;
	point_list_1[5].y = -85;
	point_list_1[5].z = -88;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
}

void get_down(void) {
	
	limbs_driver_set_smooth_config(80);
	
	path_type_list[0] = PATH_LINEAR;
	point_list_1[0].x = +110;
	point_list_1[0].y = 0;
	point_list_1[0].z = +110;
	
	path_type_list[1] = PATH_LINEAR;
	point_list_1[1].x = +150;
	point_list_1[1].y = 0;
	point_list_1[1].z = 0;
	
	path_type_list[2] = PATH_LINEAR;
	point_list_1[2].x = +110;
	point_list_1[2].y = 0;
	point_list_1[2].z = -110;
	
	path_type_list[3] = PATH_LINEAR;
	point_list_1[3].x = +110;
	point_list_1[3].y = 0;
	point_list_1[3].z = +110;
	
	path_type_list[4] = PATH_LINEAR;
	point_list_1[4].x = +150;
	point_list_1[4].y = 0;
	point_list_1[4].z = 0;
	
	path_type_list[5] = PATH_LINEAR;
	point_list_1[5].x = +110;
	point_list_1[5].y = 0;
	point_list_1[5].z = -110;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
}

void move_prepare(void) {
	
	limbs_driver_set_smooth_config(20);
	
	path_type_list[0] = PATH_LINEAR;
	point_list_1[0].x = 110;
	point_list_1[0].y = -55;
	point_list_1[0].z = 110;
	
	path_type_list[1] = PATH_LINEAR;
	point_list_1[1].x = +125;
	point_list_1[1].y = -85;
	point_list_1[1].z = 0;
	
	path_type_list[2] = PATH_LINEAR;
	point_list_1[2].x = 110;
	point_list_1[2].y = -55;
	point_list_1[2].z = -110;
	
	path_type_list[3] = PATH_LINEAR;
	point_list_1[3].x = +85;
	point_list_1[3].y = -85;
	point_list_1[3].z = +85;
	
	path_type_list[4] = PATH_LINEAR;
	point_list_1[4].x = 150;
	point_list_1[4].y = -55;
	point_list_1[4].z = 0;

	path_type_list[5] = PATH_LINEAR;
	point_list_1[5].x = +85;
	point_list_1[5].y = -85;
	point_list_1[5].z = -85;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
	
	path_type_list[0] = PATH_LINEAR;
	point_list_1[0].x = 110;
	point_list_1[0].y = -85;
	point_list_1[0].z = 20;
	
	path_type_list[1] = PATH_LINEAR;
	point_list_1[1].x = +125;
	point_list_1[1].y = -85;
	point_list_1[1].z = 0;
	
	path_type_list[2] = PATH_LINEAR;
	point_list_1[2].x = 110;
	point_list_1[2].y = -85;
	point_list_1[2].z = -110;
	
	path_type_list[3] = PATH_LINEAR;
	point_list_1[3].x = +85;
	point_list_1[3].y = -85;
	point_list_1[3].z = +85;
	
	path_type_list[4] = PATH_LINEAR;
	point_list_1[4].x = 110;
	point_list_1[4].y = -85;
	point_list_1[4].z = -45;
	
	path_type_list[5] = PATH_LINEAR;
	point_list_1[5].x = +85;
	point_list_1[5].y = -85;
	point_list_1[5].z = -85;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
	
	path_type_list[0] = PATH_LINEAR;
	point_list_1[0].x = 110;
	point_list_1[0].y = -85;
	point_list_1[0].z = 20;
	
	path_type_list[1] = PATH_LINEAR;
	point_list_1[1].x = 150;
	point_list_1[1].y = -55;
	point_list_1[1].z = 0;
	
	path_type_list[2] = PATH_LINEAR;
	point_list_1[2].x = 110;
	point_list_1[2].y = -85;
	point_list_1[2].z = -110;
	
	path_type_list[3] = PATH_LINEAR;
	point_list_1[3].x = 110;
	point_list_1[3].y = -55;
	point_list_1[3].z = 110;
	
	path_type_list[4] = PATH_LINEAR;
	point_list_1[4].x = 110;
	point_list_1[4].y = -85;
	point_list_1[4].z = -45;
	
	path_type_list[5] = PATH_LINEAR;
	point_list_1[5].x = 110;
	point_list_1[5].y = -55;
	point_list_1[5].z = -110;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
	
	path_type_list[0] = PATH_LINEAR;
	point_list_1[0].x = 110;
	point_list_1[0].y = -85;
	point_list_1[0].z = 20;
	
	path_type_list[1] = PATH_LINEAR;
	point_list_1[1].x = 110;
	point_list_1[1].y = -85;
	point_list_1[1].z = 45;
	
	path_type_list[2] = PATH_LINEAR;
	point_list_1[2].x = 110;
	point_list_1[2].y = -85;
	point_list_1[2].z = -110;
	
	path_type_list[3] = PATH_LINEAR;
	point_list_1[3].x = 110;
	point_list_1[3].y = -85;
	point_list_1[3].z = 110;
	
	path_type_list[4] = PATH_LINEAR;
	point_list_1[4].x = 110;
	point_list_1[4].y = -85;
	point_list_1[4].z = -45;
	
	path_type_list[5] = PATH_LINEAR;
	point_list_1[5].x = 110;
	point_list_1[5].y = -85;
	point_list_1[5].z = -20;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
}

void move_end(void) {
    
	limbs_driver_set_smooth_config(20);
	
	path_type_list[0] = PATH_LINEAR;
    point_list_1[0].x = 110;
    point_list_1[0].y = -85;
    point_list_1[0].z = 20;
    
	path_type_list[1] = PATH_LINEAR;
    point_list_1[1].x = 150;
    point_list_1[1].y = -55;
    point_list_1[1].z = 0;
    
	path_type_list[2] = PATH_LINEAR;
    point_list_1[2].x = 110;
    point_list_1[2].y = -85;
    point_list_1[2].z = -110;
    
	path_type_list[3] = PATH_LINEAR;
    point_list_1[3].x = 110;
    point_list_1[3].y = -55;
    point_list_1[3].z = 110;
    
	path_type_list[4] = PATH_LINEAR;
    point_list_1[4].x = 110;
    point_list_1[4].y = -85;
    point_list_1[4].z = -45;
    
	path_type_list[5] = PATH_LINEAR;
    point_list_1[5].x = 110;
    point_list_1[5].y = -55;
    point_list_1[5].z = -110;
	
    limbs_driver_start_move(point_list_1, path_type_list);
    dummy_loop();
	
    
	path_type_list[0] = PATH_LINEAR;
    point_list_1[0].x = 110;
    point_list_1[0].y = -85;
    point_list_1[0].z = 20;
    
	path_type_list[1] = PATH_LINEAR;
    point_list_1[1].x = +125;
    point_list_1[1].y = -85;
    point_list_1[1].z = 0;
    
	path_type_list[2] = PATH_LINEAR;
    point_list_1[2].x = 110;
    point_list_1[2].y = -85;
    point_list_1[2].z = -110;
    
	path_type_list[3] = PATH_LINEAR;
    point_list_1[3].x = +88;
    point_list_1[3].y = -85;
    point_list_1[3].z = +88;
    
	path_type_list[4] = PATH_LINEAR;
    point_list_1[4].x = 110;
    point_list_1[4].y = -85;
    point_list_1[4].z = -45;
    
	path_type_list[5] = PATH_LINEAR;
    point_list_1[5].x = +88;
    point_list_1[5].y = -85;
    point_list_1[5].z = -88;
	
    limbs_driver_start_move(point_list_1, path_type_list);
    dummy_loop();
	
    
	path_type_list[0] = PATH_LINEAR;
    point_list_1[0].x = 110;
    point_list_1[0].y = -55;
    point_list_1[0].z = 110;
    
	path_type_list[1] = PATH_LINEAR;
    point_list_1[1].x = +125;
    point_list_1[1].y = -85;
    point_list_1[1].z = 0;
    
	path_type_list[2] = PATH_LINEAR;
    point_list_1[2].x = 110;
    point_list_1[2].y = -55;
    point_list_1[2].z = -110;
    
	path_type_list[3] = PATH_LINEAR;
    point_list_1[3].x = +88;
    point_list_1[3].y = -85;
    point_list_1[3].z = +88;
    
	path_type_list[4] = PATH_LINEAR;
    point_list_1[4].x = 150;
    point_list_1[4].y = -55;
    point_list_1[4].z = 0;
    
	path_type_list[5] = PATH_LINEAR;
    point_list_1[5].x = +88;
    point_list_1[5].y = -85;
    point_list_1[5].z = -88;
	
    limbs_driver_start_move(point_list_1, path_type_list);
    dummy_loop();
	
    
	path_type_list[0] = PATH_LINEAR;
    point_list_1[0].x = +88;
    point_list_1[0].y = -85;
    point_list_1[0].z = +88;
    
	path_type_list[1] = PATH_LINEAR;
    point_list_1[1].x = +125;
    point_list_1[1].y = -85;
    point_list_1[1].z = 0;
    
	path_type_list[2] = PATH_LINEAR;
    point_list_1[2].x = +88;
    point_list_1[2].y = -85;
    point_list_1[2].z = -88;
    
	path_type_list[3] = PATH_LINEAR;
    point_list_1[3].x = +88;
    point_list_1[3].y = -85;
    point_list_1[3].z = +88;
    
	path_type_list[4] = PATH_LINEAR;
    point_list_1[4].x = +125;
    point_list_1[4].y = -85;
    point_list_1[4].z = 0;
    
	path_type_list[5] = PATH_LINEAR;
    point_list_1[5].x = +88;
    point_list_1[5].y = -85;
    point_list_1[5].z = -88;
	
    limbs_driver_start_move(point_list_1, path_type_list); 
    dummy_loop();
}

void direct() {
	
	limbs_driver_set_smooth_config(50);
	
	path_type_list[0] = PATH_XZ_ELLIPTICAL_Y_SINUS;
	point_list_1[0].x = 130;
	point_list_1[0].y = -55;
	point_list_1[0].z = 110;
	
	path_type_list[1] = PATH_LINEAR;
	point_list_1[1].x = 110;
	point_list_1[1].y = -85;
	point_list_1[1].z = -45;
	
	path_type_list[2] = PATH_XZ_ELLIPTICAL_Y_SINUS;
	point_list_1[2].x = 130;
	point_list_1[2].y = -55;
	point_list_1[2].z = -20;
	
	path_type_list[3] = PATH_LINEAR;
	point_list_1[3].x = 110;
	point_list_1[3].y = -85;
	point_list_1[3].z = 20;
	
	path_type_list[4] = PATH_XZ_ELLIPTICAL_Y_SINUS;
	point_list_1[4].x = 130;
	point_list_1[4].y = -55;
	point_list_1[4].z = 45;
	
	path_type_list[5] = PATH_LINEAR;
	point_list_1[5].x = 110;
	point_list_1[5].y = -85;
	point_list_1[5].z = -110;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
	
	
    path_type_list[0] = PATH_LINEAR;
    point_list_1[0].x = 110;
    point_list_1[0].y = -85;
    point_list_1[0].z = 20;
    
	path_type_list[1] = PATH_XZ_ELLIPTICAL_Y_SINUS;
    point_list_1[1].x = 130;
    point_list_1[1].y = -55;
    point_list_1[1].z = 45;
    
	path_type_list[2] = PATH_LINEAR;
    point_list_1[2].x = 110;
    point_list_1[2].y = -85;
    point_list_1[2].z = -110;
    
	path_type_list[3] = PATH_XZ_ELLIPTICAL_Y_SINUS;
    point_list_1[3].x = 130;
    point_list_1[3].y = -55;
    point_list_1[3].z = 110;
    
	path_type_list[4] = PATH_LINEAR;
    point_list_1[4].x = 110;
    point_list_1[4].y = -85;
    point_list_1[4].z = -45;
    
	path_type_list[5] = PATH_XZ_ELLIPTICAL_Y_SINUS;
    point_list_1[5].x = 130;
    point_list_1[5].y = -55;
    point_list_1[5].z = -20;
	
    limbs_driver_start_move(point_list_1, path_type_list);
    dummy_loop();
}

void reverse() {
	
	limbs_driver_set_smooth_config(50);
    
	path_type_list[0] = PATH_LINEAR;
    point_list_1[0].x = 110;
    point_list_1[0].y = -85;
    point_list_1[0].z = 65;
    
	path_type_list[1] = PATH_LINEAR;
    point_list_1[1].x = 150;
    point_list_1[1].y = -55;
    point_list_1[1].z = 0;
    
	path_type_list[2] = PATH_LINEAR;
    point_list_1[2].x = 110;
    point_list_1[2].y = -85;
    point_list_1[2].z = -65;
    
	path_type_list[3] = PATH_LINEAR;
    point_list_1[3].x = 150;
    point_list_1[3].y = -55;
    point_list_1[3].z = 65;
    
	path_type_list[4] = PATH_LINEAR;
    point_list_1[4].x = 110;
    point_list_1[4].y = -85;
    point_list_1[4].z = 0;
    
	path_type_list[5] = PATH_LINEAR;
    point_list_1[5].x = 150;
    point_list_1[5].y = -55;
    point_list_1[5].z = -65;
	
    limbs_driver_start_move(point_list_1, path_type_list);
    dummy_loop();
    
    
	path_type_list[0] = PATH_LINEAR;
    point_list_1[0].x = 110;
    point_list_1[0].y = -85;
    point_list_1[0].z = 110;
    
	path_type_list[1] = PATH_LINEAR;
    point_list_1[1].x = 110;
    point_list_1[1].y = -85;
    point_list_1[1].z = -45;
    
	path_type_list[2] = PATH_LINEAR;
    point_list_1[2].x = 110;
    point_list_1[2].y = -85;
    point_list_1[2].z = -20;
    
	path_type_list[3] = PATH_LINEAR;
    point_list_1[3].x = 110;
    point_list_1[3].y = -85;
    point_list_1[3].z = 20;
    
	path_type_list[4] = PATH_LINEAR;
    point_list_1[4].x = 110;
    point_list_1[4].y = -85;
    point_list_1[4].z = 45;
    
	path_type_list[5] = PATH_LINEAR;
    point_list_1[5].x = 110;
    point_list_1[5].y = -85;
    point_list_1[5].z = -110;
	
    limbs_driver_start_move(point_list_1, path_type_list);
    dummy_loop();
    
    
	path_type_list[0] = PATH_LINEAR;
    point_list_1[0].x = 150;
    point_list_1[0].y = -55;
    point_list_1[0].z = 65;
    
	path_type_list[1] = PATH_LINEAR;
    point_list_1[1].x = 110;
    point_list_1[1].y = -85;
    point_list_1[1].z = 0;
    
	path_type_list[2] = PATH_LINEAR;
    point_list_1[2].x = 150;
    point_list_1[2].y = -55;
    point_list_1[2].z = -65;
    
	path_type_list[3] = PATH_LINEAR;
    point_list_1[3].x = 110;
    point_list_1[3].y = -85;
    point_list_1[3].z = 65;
    
	path_type_list[4] = PATH_LINEAR;
    point_list_1[4].x = 150;
    point_list_1[4].y = -55;
    point_list_1[4].z = 0;
    
	path_type_list[5] = PATH_LINEAR;
    point_list_1[5].x = 110;
    point_list_1[5].y = -85;
    point_list_1[5].z = -65;
	
    limbs_driver_start_move(point_list_1, path_type_list);
    dummy_loop();
	
    
	path_type_list[0] = PATH_LINEAR;
    point_list_1[0].x = 110;
    point_list_1[0].y = -85;
    point_list_1[0].z = 20;
    
	path_type_list[1] = PATH_LINEAR;
    point_list_1[1].x = 110;
    point_list_1[1].y = -85;
    point_list_1[1].z = 45;
    
	path_type_list[2] = PATH_LINEAR;
    point_list_1[2].x = 110;
    point_list_1[2].y = -85;
    point_list_1[2].z = -110;
    
	path_type_list[3] = PATH_LINEAR;
    point_list_1[3].x = 110;
    point_list_1[3].y = -85;
    point_list_1[3].z = 110;
    
	path_type_list[4] = PATH_LINEAR;
    point_list_1[4].x = 110;
    point_list_1[4].y = -85;
    point_list_1[4].z = -45;
    
	path_type_list[5] = PATH_LINEAR;
    point_list_1[5].x = 110;
    point_list_1[5].y = -85;
    point_list_1[5].z = -20;
	
    limbs_driver_start_move(point_list_1, path_type_list);
    dummy_loop();
}

void rotate_left() {
	
	limbs_driver_set_smooth_config(50);
	
	path_type_list[0] = PATH_XZ_ARC_Y_SINUS;
	point_list_1[0].x = +121;
	point_list_1[0].y = -55;
	point_list_1[0].z = +32;
	
	path_type_list[1] = PATH_XZ_ARC_Y_LINEAR;
	point_list_1[1].x = +108;
	point_list_1[1].y = -85;
	point_list_1[1].z = +62;
	
	path_type_list[2] = PATH_XZ_ARC_Y_SINUS;
	point_list_1[2].x = +32;
	point_list_1[2].y = -55;
	point_list_1[2].z = -121;
	
	path_type_list[3] = PATH_XZ_ARC_Y_LINEAR;
	point_list_1[3].x = +121;
	point_list_1[3].y = -85;
	point_list_1[3].z = +32;
	
	path_type_list[4] = PATH_XZ_ARC_Y_SINUS;
	point_list_1[4].x = +108;
	point_list_1[4].y = -55;
	point_list_1[4].z = +62;
	
	path_type_list[5] = PATH_XZ_ARC_Y_LINEAR;
	point_list_1[5].x = +32;
	point_list_1[5].y = -85;
	point_list_1[5].z = -121;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
	
	
	path_type_list[0] = PATH_XZ_ARC_Y_LINEAR;
	point_list_1[0].x = +88;
	point_list_1[0].y = -85;
	point_list_1[0].z = +88;
	
	path_type_list[1] = PATH_XZ_ARC_Y_SINUS;
	point_list_1[1].x = +125;
	point_list_1[1].y = -55;
	point_list_1[1].z = +0;
	
	path_type_list[2] = PATH_XZ_ARC_Y_LINEAR;
	point_list_1[2].x = +88;
	point_list_1[2].y = -85;
	point_list_1[2].z = -88;
	
	path_type_list[3] = PATH_XZ_ARC_Y_SINUS;
	point_list_1[3].x = +88;
	point_list_1[3].y = -55;
	point_list_1[3].z = +88;
	
	path_type_list[4] = PATH_XZ_ARC_Y_LINEAR;
	point_list_1[4].x = +125;
	point_list_1[4].y = -85;
	point_list_1[4].z = +0;
	
	path_type_list[5] = PATH_XZ_ARC_Y_SINUS;
	point_list_1[5].x = +88;
	point_list_1[5].y = -55;
	point_list_1[5].z = -88;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
}

void rotate_right() {
	
	limbs_driver_set_smooth_config(50);
	
	path_type_list[0] = PATH_XZ_ARC_Y_SINUS;
	point_list_1[0].x = +32;
	point_list_1[0].y = -55;
	point_list_1[0].z = +121;
	
	path_type_list[1] = PATH_XZ_ARC_Y_LINEAR;
	point_list_1[1].x = +108;
	point_list_1[1].y = -85;
	point_list_1[1].z = -62;
	
	path_type_list[2] = PATH_XZ_ARC_Y_SINUS;
	point_list_1[2].x = +121;
	point_list_1[2].y = -55;
	point_list_1[2].z = -32;
	
	path_type_list[3] = PATH_XZ_ARC_Y_LINEAR;
	point_list_1[3].x = +32;
	point_list_1[3].y = -85;
	point_list_1[3].z = +121;
	
	path_type_list[4] = PATH_XZ_ARC_Y_SINUS;
	point_list_1[4].x = +108;
	point_list_1[4].y = -55;
	point_list_1[4].z = -62;
	
	path_type_list[5] = PATH_XZ_ARC_Y_LINEAR;
	point_list_1[5].x = +121;
	point_list_1[5].y = -85;
	point_list_1[5].z = -32;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
	
	
	path_type_list[0] = PATH_XZ_ARC_Y_LINEAR;
	point_list_1[0].x = +88;
	point_list_1[0].y = -85;
	point_list_1[0].z = +88;
	
	path_type_list[1] = PATH_XZ_ARC_Y_SINUS;
	point_list_1[1].x = +125;
	point_list_1[1].y = -55;
	point_list_1[1].z = +0;
	
	path_type_list[2] = PATH_XZ_ARC_Y_LINEAR;
	point_list_1[2].x = +88;
	point_list_1[2].y = -85;
	point_list_1[2].z = -88;
	
	path_type_list[3] = PATH_XZ_ARC_Y_SINUS;
	point_list_1[3].x = +88;
	point_list_1[3].y = -55;
	point_list_1[3].z = +88;
	
	path_type_list[4] = PATH_XZ_ARC_Y_LINEAR;
	point_list_1[4].x = +125;
	point_list_1[4].y = -85;
	point_list_1[4].z = +0;
	
	path_type_list[5] = PATH_XZ_ARC_Y_SINUS;
	point_list_1[5].x = +88;
	point_list_1[5].y = -55;
	point_list_1[5].z = -88;
	
	limbs_driver_start_move(point_list_1, path_type_list);
	dummy_loop();
}

void scr_process(void) {
	
	switch (scr) {
		
		case 0x01:
			get_up();
			break;
			
		case 0x02:
			get_down();
			break;
		
		case 0x04:
			move_prepare();
			for (uint32_t i = 0; i < 5; ++i) {
				direct();
			}
			move_end();
			break;
			
		case 0x05:
			move_prepare();
			for (uint32_t i = 0; i < 5; ++i) {
    			reverse();
			}
			move_end();
			break;
            
       case 0x06:
            for (uint32_t i = 0; i < 5; ++i) {
                move_prepare();
                move_end();
            }
            break;
			
		case 0x10:
			point_list_1[4].x = x;
			point_list_1[4].y = y;
			point_list_1[4].z = z;
            path_type_list[4] = PATH_LINEAR;
			limbs_driver_start_move(point_list_1, path_type_list);
			break;
			
		case 0x11:
			point_list_1[4].x = x;
			point_list_1[4].y = y;
			point_list_1[4].z = z;
			path_type_list[4] = PATH_XZ_ARC_Y_LINEAR;
			limbs_driver_start_move(point_list_1, path_type_list);
			break;
		
		case 0x12:
			point_list_1[4].x = x;
			point_list_1[4].y = y;
			point_list_1[4].z = z;
            path_type_list[4] = PATH_XZ_ARC_Y_SINUS;
			limbs_driver_start_move(point_list_1, path_type_list);
			break;
			
		case 0x13:
			rotate_left();
			break;
			
		case 0x14:
			rotate_right();
			break;
			
        
			
		/*case SCR_CMD_CALCULATE_CHECKSUM:
			veeprom_update_checksum();
			break;
			
		case SCR_CMD_SELECT_GAIT_UP:
			movement_driver_select_sequence(SEQUENCE_UP);
			break;
		
		case SCR_CMD_SELECT_GAIT_BASE:
			movement_driver_select_sequence(SEQUENCE_BASE);
			break;	
	
		case SCR_CMD_SELECT_GAIT_DOWN:
			movement_driver_select_sequence(SEQUENCE_DOWN);
			break;
			
		case SCR_CMD_SELECT_GAIT_DIRECT_MOVEMENT:
			movement_driver_select_sequence(SEQUENCE_DIRECT_MOVEMENT);
			break;
			
		case SCR_CMD_SELECT_GAIT_REVERSE_MOVEMENT:
			movement_driver_select_sequence(SEQUENCE_REVERSE_MOVEMENT);
			break;
		
		case SCR_CMD_SELECT_GAIT_ROTATE_LEFT:
			movement_driver_select_sequence(SEQUENCE_ROTATE_LEFT);
			break;
			
		case SCR_CMD_SELECT_GAIT_ROTATE_RIGHT:
			movement_driver_select_sequence(SEQUENCE_ROTATE_RIGHT);
			break;*/
			
		case SCR_CMD_RESET:
			REG_RSTC_CR = 0xA5000005;
			break;
	}
	
	scr = 0x00;
}