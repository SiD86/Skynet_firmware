//  ***************************************************************************
/// @file    main.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#include "gaits_engine.h"
#include "limbs_driver.h"
#include "servo_driver.h"
#include "wireless.h"
#include "monitoring.h"
#include "systimer.h"
#include "veeprom.h"
#include "modbus.h"
#include "scr.h"
#include "error_handling.h"
#include "uart.h"
#include "dac.h"
#define MAX_MAIN_LOOP_ITERATION_TIME				(40) // ms


static void enter_to_emergency_loop(void);


int main(void) {
	
    // Initialize the SAM system
    SystemInit();
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	REG_PIOB_PER = PIO_PB27;
	REG_PIOB_OER = PIO_PB27;
	REG_PIOB_CODR = PIO_PB27;
	
	// Initialize FW
	systimer_init();
	veeprom_init();
	modbus_init();
	wireless_init();
	monitoring_init();
	
	servo_driver_init();
	limbs_driver_init();
	gaits_engine_init();
	
    while (1)  {
		
		uint32_t begin_loop_time = get_time_ms();
		
		//
		// CHECK SYSTEM STATUS
		//
		if (callback_is_fatal_error_set() == true) {
			enter_to_emergency_loop();
		}
		
		
		//
		// NORMAL MODE PROCESS
		//
		gaits_engine_process();
		servo_driver_process();
		modbus_process();
		scr_process();

		// Check loop time
		if (get_time_ms() - begin_loop_time >= MAX_MAIN_LOOP_ITERATION_TIME) {
			callback_set_sync_error(ERROR_MODULE_NO);
		}
	}
}

static void enter_to_emergency_loop(void) {
	
	while (1)  {
		
		modbus_process();
		wireless_process();
		monitoring_process();
		scr_process();
	}
}