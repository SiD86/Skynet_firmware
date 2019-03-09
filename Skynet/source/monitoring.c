//  ***************************************************************************
/// @file    monitoring.c
/// @author  NeoProg
/// @brief   Monitoring subsystem
//  ***************************************************************************
#include "monitoring.h"

#include <sam.h>
#include <stdbool.h>
#include "adc.h"
#include "veeprom_map.h"
#include "veeprom.h"
#include "error_handling.h"

#define SUPPORT_ADC_CHANNEL_COUNT				(3)
#define WIRELESS_VOLTAGE_ADC_CH					(2)
#define PERIPHERY_VOLTAGE_ADC_CH				(1)
#define BATTERY_VOLTAGE_ADC_CH					(0)


typedef enum {
	STATE_NOINIT,
	STATE_WAIT_CONVERSION,
	STATE_CALCULATE
} state_t;

typedef struct {
	uint32_t up_resist;
	uint32_t down_resist;	
} voltage_divisor_info_t;


static state_t subsystem_state = STATE_NOINIT;
static voltage_divisor_info_t voltage_divisor[SUPPORT_ADC_CHANNEL_COUNT] = {0};

uint16_t wireless_voltage = 0;	// 0.1 V
uint16_t periphery_voltage = 0;	// 0.1 V
uint16_t battery_voltage = 0;	// 0.1 V


static bool read_configuration(void);
static uint16_t calculate_voltage(float adc_voltage, const voltage_divisor_info_t* divisor_info);


//  ***************************************************************************
/// @brief	Monitoring subsystem initialization
/// @return	none
//  ***************************************************************************
void monitoring_init(void) {
	
	if (read_configuration() == false) {
		callback_set_config_error(ERROR_MODULE_MONITORING);
		return;
	}
	
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
	
	
	static float adc_voltage[SUPPORT_ADC_CHANNEL_COUNT] = {0};
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
			wireless_voltage  = calculate_voltage(adc_voltage[0], &voltage_divisor[0]);
			periphery_voltage = calculate_voltage(adc_voltage[1], &voltage_divisor[1]);
			battery_voltage   = calculate_voltage(adc_voltage[2], &voltage_divisor[2]);
			
			subsystem_state = STATE_WAIT_CONVERSION;
			break;
		
		case STATE_NOINIT:
		default:
			callback_set_internal_error(ERROR_MODULE_MONITORING);
			break;
	}
}





//  ***************************************************************************
/// @brief  Read configuration
/// @param  none
/// @return true - read success, false - fail
//  ***************************************************************************
static bool read_configuration(void) {
	
	for (uint32_t i = 0; i < SUPPORT_ADC_CHANNEL_COUNT; ++i) {
		
		uint32_t base_address = VOLTAGE_DIVISOR_CONFIGURATION_BASE_EE_ADDRESS + i * VOLTAGE_DIVISOR_CONFIGURATION_SIZE;
		
		voltage_divisor[i].up_resist   = veeprom_read_32(base_address + VOLTAGE_DIVISOR_UP_RESIST_OFFSET);
		voltage_divisor[i].down_resist = veeprom_read_32(base_address + VOLTAGE_DIVISOR_DOWN_RESIST_OFFSET);
		if (voltage_divisor[i].up_resist == 0xFFFFFFFF || voltage_divisor[i].down_resist == 0xFFFFFFFF) {
			return false;
		}
	}
	
	return true;
}

//  ***************************************************************************
/// @brief	Calculate resistance divisor supply voltage
/// @param	adc_voltage: ADC voltage, [V]
/// @param	up_resistor: up resistor value, [Ohm]
/// @param	down_resistor: down resistor value, [Ohm]
/// @return	Resistance divisor supply voltage, [0.1 V]
//  ***************************************************************************
static uint16_t calculate_voltage(float adc_voltage, const voltage_divisor_info_t* divisor_info) {
	
	float divisor_factor = ((float)divisor_info->up_resist + (float)divisor_info->down_resist) / (float)divisor_info->down_resist;
	
	return (adc_voltage * divisor_factor) * 10.0f;
}