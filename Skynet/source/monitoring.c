//  ***************************************************************************
/// @file    monitoring.c
/// @author  NeoProg
/// @brief   Monitoring subsystem
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#include "monitoring.h"
#include "adc.h"
#include "dac.h"
#include "error_handling.h"

#define ADC_CHANNEL_COUNT						(5)
#define WIRELESS_VOLTAGE_ADC_CH					(4)
#define PERIPHERY_VOLTAGE_ADC_CH				(3)
#define BATTERY_BANK0_VOLTAGE_ADC_CH			(2)
#define BATTERY_BANK1_VOLTAGE_ADC_CH			(1)
#define POWER_BOARD_TEMPERATURE_ADC_CH			(0)

#define FAN_DAC_CH								(0)
#define FAN_LEVEL0_TEMPERATURE					(4000)	// 40.00 *C
#define FAN_LEVEL1_TEMPERATURE					(5000)	// 50.00 *C
#define FAN_LEVEL2_TEMPERATURE					(6000)	// 60.00 *C
#define FAN_LEVEL3_TEMPERATURE					(7000)	// 70.00 *C
#define FAN_LEVEL0_DAC_VALUE					(1023)
#define FAN_LEVEL1_DAC_VALUE					(2046)
#define FAN_LEVEL2_DAC_VALUE					(3069)
#define FAN_LEVEL3_DAC_VALUE					(4095)


typedef enum {
	NOINIT,
	WAIT_ADC_CONVERSION,
	CALCULATE,
	FAN_PROCESS
} state_t;


static state_t subsystem_state = NOINIT;

static uint32_t wireless_voltage = 0;			// 0.01 V
static uint32_t periphery_voltage = 0;			// 0.01 V
static uint32_t battery_bank0_voltage = 0;		// 0.01 V
static uint32_t battery_bank1_voltage = 0;		// 0.01 V
static uint32_t power_board_temperature = 0;	// 0.01 *C


static uint32_t calculate_voltage(float adc_voltage, float up_resistor, float down_resistor);
static uint32_t calculate_temperature(float adc_voltage, float resistor);


//  ***************************************************************************
/// @brief	Monitoring subsystem initialization
/// @return	none
//  ***************************************************************************
void monitoring_init(void) {
	
	dac_init();
	dac_set_output_value(FAN_DAC_CH, 0);
	
	adc_init();
	adc_start_conversion();
	
	subsystem_state = WAIT_ADC_CONVERSION;
}

//  ***************************************************************************
/// @brief	Monitoring subsystem process
/// @note	Call from main loop
//  ***************************************************************************
void monitoring_process(void) {
	
	if (callback_is_monitoring_error_set() == true) return; // Module disabled
	
	
	static float adc_voltage[5] = {0};
	switch (subsystem_state) {
		
		case WAIT_ADC_CONVERSION:
			if (adc_is_conversion_complete() == true) {
				
				adc_voltage[0] = adc_get_voltage(WIRELESS_VOLTAGE_ADC_CH);
				adc_voltage[1] = adc_get_voltage(PERIPHERY_VOLTAGE_ADC_CH);
				adc_voltage[2] = adc_get_voltage(BATTERY_BANK0_VOLTAGE_ADC_CH);
				adc_voltage[3] = adc_get_voltage(BATTERY_BANK1_VOLTAGE_ADC_CH);
				adc_voltage[4] = adc_get_voltage(POWER_BOARD_TEMPERATURE_ADC_CH);
				
				adc_start_conversion();
				subsystem_state = CALCULATE;
			}
			break;
		
		case CALCULATE:
			wireless_voltage		= calculate_voltage(adc_voltage[0], 1500, 1300);
			periphery_voltage		= calculate_voltage(adc_voltage[1], 1500, 1300);
			battery_bank0_voltage	= calculate_voltage(adc_voltage[2], 1000000, 1000000);
			battery_bank1_voltage	= calculate_voltage(adc_voltage[3], 1000000, 1000000);
			power_board_temperature = calculate_temperature(adc_voltage[4], 1200000);
			
			subsystem_state = FAN_PROCESS;
			break;
			
		case FAN_PROCESS:
			if (power_board_temperature >= FAN_LEVEL3_TEMPERATURE) {
				dac_set_output_value(FAN_DAC_CH, FAN_LEVEL3_DAC_VALUE);
			}
			else if (power_board_temperature >= FAN_LEVEL2_TEMPERATURE) {
				dac_set_output_value(FAN_DAC_CH, FAN_LEVEL2_DAC_VALUE);
			}
			else if (power_board_temperature >= FAN_LEVEL1_TEMPERATURE) {
				dac_set_output_value(FAN_DAC_CH, FAN_LEVEL1_DAC_VALUE);
			}
			else if (power_board_temperature >= FAN_LEVEL0_TEMPERATURE) {
				dac_set_output_value(FAN_DAC_CH, FAN_LEVEL0_DAC_VALUE);
			}
			else {
				dac_set_output_value(FAN_DAC_CH, 0);
			}
			
			subsystem_state = WAIT_ADC_CONVERSION;
			break;
		
		case NOINIT:
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
/// @return	Resistance divisor supply voltage, [0.01 V]
//  ***************************************************************************
static uint32_t calculate_voltage(float adc_voltage, float up_resistor, float down_resistor) {
	
	float divisor_factor = (up_resistor + down_resistor) / down_resistor;
	
	return (adc_voltage * divisor_factor) * 100;
}

//  ***************************************************************************
/// @brief	Calculate temperature
/// @param	adc_voltage: ADC voltage, [V]
/// @param	resistor: resistor value, [Ohm]
/// @return	Temperature, [0.01 *C]
//  ***************************************************************************
static uint32_t calculate_temperature(float adc_voltage, float resistor) {
	
	return 0;
}