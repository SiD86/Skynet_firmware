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
#include "error_handling.h"

 
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
	veeprom_init();
	modbus_init();
	wireless_modbus_init();
	//monitoring_init();
	led_init();
    
    servo_driver_init();
    limbs_driver_init();
	//movement_driver_init();
    
    while (1)  {
		
		//
		// CHECK SYSTEM STATUS
		//
		if (callback_is_emergency_mode_active() == true) {
            led_enable(RED_LED);
			enter_to_emergency_loop();
		}
		if (callback_is_any_error_set() == true) {
            led_enable(YELLOW_LED);
        }
		else {
			led_enable(GREEN_LED);;
		}
        
		//
		// NORMAL MODE PROCESS
		//
        servo_driver_process();
        limbs_driver_process();
        //movement_driver_process();
		
        
        wireless_modbus_process();
		modbus_process();
		scr_process();
        
       // monitoring_process();
	}
}

static void enter_to_emergency_loop(void) {
    
	while (1)  {
		
		modbus_process();
		wireless_modbus_process();
		//monitoring_process();
		scr_process();
	}
}