//  ***************************************************************************
/// @file    monitoring.c
/// @author  NeoProg
/// @brief   Monitoring subsystem
//  ***************************************************************************
#include "monitoring.h"

#include <sam.h>
#include <stdbool.h>
#include "adc.h"
#include "error_handling.h"

#define ADC_CHANNEL_COUNT						(3)
#define WIRELESS_VOLTAGE_ADC_CH					(4)
#define PERIPHERY_VOLTAGE_ADC_CH				(3)
#define BATTERY_VOLTAGE_ADC_CH					(2)


typedef enum {
	STATE_NOINIT,
	STATE_WAIT_CONVERSION,
	STATE_CALCULATE
} state_t;


static state_t subsystem_state = STATE_NOINIT;

uint32_t wireless_voltage = 0;	// 0.1 V
uint32_t periphery_voltage = 0;	// 0.1 V
uint32_t battery_voltage = 0;	// 0.1 V


static uint32_t calculate_voltage(float adc_voltage, float up_resistor, float down_resistor);


//  ***************************************************************************
/// @brief	Monitoring subsystem initialization
/// @return	none
//  ***************************************************************************
void monitoring_init(void) {
	
	adc_init();
	adc_start_conversion();
	
	subsystem_state = STATE_WAIT_CONVERSION;
}

//  ***************************************************************************
/// @brief	Monitoring subsystem process
/// @note	Call from main loop
//  ***************************************************************************
void monitoring_process(void) {
	
	if (callback_is_monitoring_error_set() == true) return; // Module disabled
	
	
	static float adc_voltage[ADC_CHANNEL_COUNT] = {0};
	switch (subsystem_state) {
		
		case STATE_WAIT_CONVERSION:
			if (adc_is_conversion_complete() == true) {
				
				adc_voltage[0] = adc_get_voltage(WIRELESS_VOLTAGE_ADC_CH);
				adc_voltage[1] = adc_get_voltage(PERIPHERY_VOLTAGE_ADC_CH);
				adc_voltage[2] = adc_get_voltage(BATTERY_VOLTAGE_ADC_CH);
				
				adc_start_conversion();
				subsystem_state = STATE_CALCULATE;
			}
			break;
		
		case STATE_CALCULATE:
			wireless_voltage  = calculate_voltage(adc_voltage[0], 1500, 1300);
			periphery_voltage = calculate_voltage(adc_voltage[1], 1500, 1300);
			battery_voltage   = calculate_voltage(adc_voltage[2], 1000000, 1000000);
			
			subsystem_state = STATE_WAIT_CONVERSION;
			break;
		
		case STATE_NOINIT:
		default:
			callback_set_internal_error(ERROR_MODULE_MONITORING);
			break;
	}
}





//  ***************************************************************************
/// @brief	Calculate resistance divisor supply voltage
/// @param	adc_voltage: ADC voltage, [V]
/// @param	up_resistor: up resistor value, [Ohm]
/// @param	down_resistor: down resistor value, [Ohm]
/// @return	Resistance divisor supply voltage, [0.1 V]
//  ***************************************************************************
static uint32_t calculate_voltage(float adc_voltage, float up_resistor, float down_resistor) {
	
	float divisor_factor = (up_resistor + down_resistor) / down_resistor;
	
	return (adc_voltage * divisor_factor) * 10;
}