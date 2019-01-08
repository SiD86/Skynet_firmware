//  ***************************************************************************
/// @file    main.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdlib.h>
#include <stdbool.h>
#include "movement_driver.h"
#include "limbs_driver.h"
#include "servo_driver.h"
#include "wireless_modbus.h"
#include "monitoring.h"
#include "systimer.h"
#include "veeprom.h"
#include "modbus.h"
#include "scr.h"
#include "led.h"
#include "multimedia.h"
#include "error_handling.h"
#include "vc0706.h"


static void enter_to_emergency_loop(void);


void HardFault_Handler(void) {
	
	while (true);
}

int main(void) {
	
    // Initialize SAM system
    SystemInit();
	WDT->WDT_MR = WDT_MR_WDDIS;
    
	// Initialize FW
	systimer_init();
	//veeprom_init();
	//modbus_init();
	//wireless_init();
	//monitoring_init();
    //multimedia_init();
	//led_init();
	
	//limbs_driver_init();
	
	wireless_modbus_init();
	multimedia_init();
    
	multimedia_update_image();
    while (true) {
        multimedia_process();   
		wireless_modbus_process();
		scr_process();
    }        
	//movement_driver_init();
	
	/*uint32_t state = 0;
	uint32_t start_wait = 0;
	
	int32_t x[50] = {0};
	int32_t z[50] = {0};
	for (uint32_t i = 0; i < 50; ++i) {
		x[i] = 91 + (183.0 - 91.0) / 50.0 * i;
		z[i] = -91 + 91.0 / 50.0 * i;
	}
	uint32_t index = 0;*/
	
	while (1)  {
		
		/*switch (state) {
			
		case 0:
			limbs_driver_start_move(3, x[index], -50, z[index], 0);
			state = 1;
			start_wait = get_time_ms();
			break;
			
		case 1:
			if (get_time_ms() - start_wait > 10) {
				state = 0;
				++index;
				if (index == 49) {
					state = 2;
				}
			}
			break;
			
		case 2:
			limbs_driver_start_move(3, x[index], -50, z[index], 0);
			state = 3;
			start_wait = get_time_ms();
			break;
			
		case 3:
			if (get_time_ms() - start_wait > 10) {
				state = 2;
				--index;
				if (index == 0) {
					state = 0;
				}
			}
			break;
		}*/
  
		limbs_driver_process();
		servo_driver_process();
		modbus_process();
		scr_process();
	}
	
    while (1)  {
		
		//
		// CHECK SYSTEM STATUS
		//
		if (callback_is_fatal_error_set() == true) {
            led_enable(RED_LED);
			enter_to_emergency_loop();
		}
		if (callback_is_any_error_set() == true) {
            led_enable(YELLOW_LED);
        }
		
        
		//
		// NORMAL MODE PROCESS
		//
		//movement_driver_process();
        limbs_driver_process();
		servo_driver_process();
        
        wireless_modbus_process();
		modbus_process();
		scr_process();
        
        monitoring_process();
	}
}

static void enter_to_emergency_loop(void) {
    
	while (1)  {
		
		modbus_process();
		wireless_modbus_process();
		monitoring_process();
		scr_process();
	}
}