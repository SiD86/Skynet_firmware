//  ***************************************************************************
/// @file    adc.c
/// @author  NeoProg
///
/// ADC channels map
/// ADC_CHER_CH7;	// (A0)
/// ADC_CHER_CH6;	// (A1)
/// ADC_CHER_CH5;	// (A2)
/// ADC_CHER_CH4;	// (A3)
/// ADC_CHER_CH3;	// (A4)
/// ADC_CHER_CH2;	// (A5)
/// ADC_CHER_CH1;	// (A6)
/// ADC_CHER_CH0;	// (A7)
/// ADC_CHER_CH10;	// (A8)
/// ADC_CHER_CH11;	// (A9)
/// ADC_CHER_CH12;	// (A10)
/// ADC_CHER_CH13;	// (A11)
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#define SUPPORT_ADC_CHANNELS_COUNT				(12)

static uint16_t adc_data[SUPPORT_ADC_CHANNELS_COUNT] = { 0 };
	

//  ***************************************************************************
/// @brief  Initialize ADC
/// @note	none
/// @return none
//  ***************************************************************************
void adc_init(void) {

	// Enable ADC and PDC clock
	REG_PMC_PCER1 = PMC_PCER1_PID37 | PMC_PCER1_PID39;
	while ( (REG_PMC_PCSR1 & (PMC_PCER1_PID37 | PMC_PCER1_PID39)) == 0 );
	
	// Enable PIO
	REG_PIOA_PER = PIO_PER_P16 | PIO_PER_P24 | PIO_PER_P23 | PIO_PER_P22 | PIO_PER_P6 |
				   PIO_PER_P4  | PIO_PER_P3  | PIO_PER_P2;
	REG_PIOB_PER = PIO_PER_P17 | PIO_PER_P18 | PIO_PER_P19 | PIO_PER_P20;
	
	// Set pin as input
	REG_PIOA_ODR = PIO_ODR_P16 | PIO_ODR_P24 | PIO_ODR_P23 | PIO_ODR_P22 | PIO_ODR_P6 |
			       PIO_ODR_P4  | PIO_ODR_P3  | PIO_ODR_P2;
	REG_PIOB_ODR = PIO_PER_P17 | PIO_PER_P18 | PIO_PER_P19 | PIO_PER_P20;

	// Reset ADC
	REG_ADC_CR = ADC_CR_SWRST;
	REG_ADC_PTCR = ADC_PTCR_RXTDIS | ADC_PTCR_TXTDIS;

	// Setup ADC
	REG_ADC_MR = ADC_MR_PRESCAL(SystemCoreClock / (2 * 20000000) - 1) | ADC_MR_STARTUP_SUT768 | ADC_MR_SETTLING_AST17 | ADC_MR_TRGEN_DIS;
	
	// Disable all IRQ
	REG_ADC_IDR = 0xFFFFFFFF;
	
	// Disable all channels
	REG_ADC_IDR = 0xFFFFFFFF;
	
	// Enable all support channels
	REG_ADC_CHER = ADC_CHER_CH7;	// (A0)
	REG_ADC_CHER = ADC_CHER_CH6;	// (A1)
	REG_ADC_CHER = ADC_CHER_CH5;	// (A2)
	REG_ADC_CHER = ADC_CHER_CH4;	// (A3)
	REG_ADC_CHER = ADC_CHER_CH3;	// (A4)
	REG_ADC_CHER = ADC_CHER_CH2;	// (A5)
	REG_ADC_CHER = ADC_CHER_CH1;	// (A6)
	REG_ADC_CHER = ADC_CHER_CH0;	// (A7)
	REG_ADC_CHER = ADC_CHER_CH10;	// (A8)
	REG_ADC_CHER = ADC_CHER_CH11;	// (A9)
	REG_ADC_CHER = ADC_CHER_CH12;	// (A10)
	REG_ADC_CHER = ADC_CHER_CH13;	// (A11)
}

//  ***************************************************************************
/// @brief  Start ADC conversion with PDC
/// @note	none
/// @return none
//  ***************************************************************************
void adc_start_conversion(void) {

	// Initialize PDC channel for ADC
	REG_ADC_PTCR = ADC_PTCR_RXTDIS;
	REG_ADC_RPR  = (uint32_t)adc_data;
	REG_ADC_RCR  = SUPPORT_ADC_CHANNELS_COUNT;
	REG_ADC_PTCR = ADC_PTCR_RXTEN;

	// Start ADC conversion
	REG_ADC_CR = ADC_CR_START;
}

//  ***************************************************************************
/// @brief  Check conversion status
/// @note	none
/// @return true - conversion complete, false - conversion on progress
//  ***************************************************************************
bool adc_is_conversion_complete(void) {
	
	return (REG_ADC_RCR == 0);
}

//  ***************************************************************************
/// @brief  Get channel voltage
/// @param  ch: number
/// @return ADC channel voltage
//  ***************************************************************************
float adc_get_voltage(uint32_t ch) {
	
	if (ch >= SUPPORT_ADC_CHANNELS_COUNT) return 0;
	
	return (adc_data[ch] & 0x0FFF) * 3.3 / 4095.0;
}
