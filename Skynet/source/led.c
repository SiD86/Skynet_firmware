//  ***************************************************************************
/// @file    led.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include "led.h"
#include "error_handling.h"
#include "systimer.h"


static uint32_t leds[] = {PIO_PC23, PIO_PC24, PIO_PC25};


/// ***************************************************************************
/// @brief  Led module initialization
/// @param  none
//  ***************************************************************************
void led_init(void) {
	
    // Initialization GPIO
	REG_PIOC_PER  = leds[GREEN_LED] | leds[YELLOW_LED] | leds[RED_LED];
	REG_PIOC_OER  = leds[GREEN_LED] | leds[YELLOW_LED] | leds[RED_LED];
	REG_PIOC_SODR = leds[GREEN_LED] | leds[YELLOW_LED] | leds[RED_LED];

    // Turn on/off for LED diagnostic
	REG_PIOC_CODR = leds[GREEN_LED] | leds[YELLOW_LED] | leds[RED_LED];
	delay_ms(500);
    REG_PIOC_SODR = leds[GREEN_LED] | leds[YELLOW_LED] | leds[RED_LED];
}

/// ***************************************************************************
/// @brief  Led enable
/// @param  none
//  ***************************************************************************
void led_enable(led_id id) {
    
    if (id >= sizeof(leds) / sizeof(leds[0])) {
        return;
    }
    
    // LED already enabled
    if ((REG_PIOC_ODSR & leds[id]) == 0) {
        return;
    }
    
    // Disable all LEDs and enable requested LED
	REG_PIOC_SODR = leds[GREEN_LED] | leds[YELLOW_LED] | leds[RED_LED];
    REG_PIOC_CODR = leds[id];
}