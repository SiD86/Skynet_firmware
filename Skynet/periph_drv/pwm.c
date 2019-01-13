//  ***************************************************************************
/// @file    pwm.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include "pwm.h"

#define PWM_FREQUENCY_HZ                (100)
#define PWM_PERIOD_TICKS                (SystemCoreClock / 2 / PWM_FREQUENCY_HZ)
#define PWM_PERIOD_US                   (1000000 / PWM_FREQUENCY_HZ)
#define PULSE_WIDTH_TO_TICKS(_width)    (PWM_PERIOD_TICKS / PWM_PERIOD_US * _width)

#define PWM_CH0_PIN                     (PIO_PC13)
#define PWM_CH1_PIN                     (PIO_PB21)
#define PWM_CH2_PIN                     (PIO_PB14)
#define PWM_CH3_PIN                     (PIO_PC12)
#define PWM_CH4_PIN                     (PIO_PC14)
#define PWM_CH5_PIN                     (PIO_PC16)
#define PWM_CH6_PIN                     (PIO_PC18)
#define PWM_CH7_PIN                     (PIO_PA20)
#define PWM_CH8_PIN                     (PIO_PC9)

#define PWM_CH9_PIN                     (PIO_PD3)
#define PWM_CH10_PIN                    (PIO_PD1)
#define PWM_CH11_PIN                    (PIO_PD2)
#define PWM_CH12_PIN                    (PIO_PD6)
#define PWM_CH13_PIN                    (PIO_PA7)
#define PWM_CH14_PIN                    (PIO_PC1)
#define PWM_CH15_PIN                    (PIO_PC3)
#define PWM_CH16_PIN                    (PIO_PC5)
#define PWM_CH17_PIN                    (PIO_PC7)

#define PWM_ALL_PINS_PORTA              (PWM_CH7_PIN | PWM_CH13_PIN)
#define PWM_ALL_PINS_PORTB				(PWM_CH1_PIN | PWM_CH2_PIN)
#define PWM_ALL_PINS_PORTC              (PWM_CH0_PIN | PWM_CH3_PIN  | PWM_CH4_PIN  | PWM_CH5_PIN  | PWM_CH6_PIN  | PWM_CH8_PIN | \
                                         PWM_CH14_PIN | PWM_CH15_PIN | PWM_CH16_PIN | PWM_CH17_PIN)
#define PWM_ALL_PINS_PORTD              (PWM_CH9_PIN  | PWM_CH10_PIN  | PWM_CH11_PIN | PWM_CH12_PIN)


static volatile uint32_t pwm_channel_ticks[18] = { 0 };
static volatile pwm_buffer_state_t pwm_channels_buffers_state = PWM_BUFFERS_UNLOCK;
static uint32_t pwm_period_counter = 0;


//  ***************************************************************************
/// @brief  PWM initialization
/// @param  none
/// @return none
//  ***************************************************************************
void pwm_init(void) {

    // Initialize GPIO
    REG_PIOA_PER  = PWM_ALL_PINS_PORTA;
    REG_PIOA_OER  = PWM_ALL_PINS_PORTA;
    REG_PIOA_SODR = PWM_ALL_PINS_PORTA;
	
	REG_PIOB_PER  = PWM_ALL_PINS_PORTB;
	REG_PIOB_OER  = PWM_ALL_PINS_PORTB;
	REG_PIOB_SODR = PWM_ALL_PINS_PORTB;
    
    REG_PIOC_PER  = PWM_ALL_PINS_PORTC;
    REG_PIOC_OER  = PWM_ALL_PINS_PORTC;
    REG_PIOC_SODR = PWM_ALL_PINS_PORTC;
    
    REG_PIOD_PER  = PWM_ALL_PINS_PORTD;
    REG_PIOD_OER  = PWM_ALL_PINS_PORTD;
    REG_PIOD_SODR = PWM_ALL_PINS_PORTD;

    // Enable timer clocks
    REG_PMC_PCER0 |= PMC_PCER0_PID27 | PMC_PCER0_PID28 | PMC_PCER0_PID29 | PMC_PCER0_PID30 | PMC_PCER0_PID31;
    REG_PMC_PCER1 |= PMC_PCER1_PID32 | PMC_PCER1_PID33 | PMC_PCER1_PID34 | PMC_PCER1_PID35;

    // Initialize sync timer (PWM period)
    REG_TC0_CMR0 = TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1;
    REG_TC0_RC0  = PWM_PERIOD_TICKS;
    REG_TC0_IER0 = TC_IER_CPCS | TC_IER_COVFS;
    
    // Initialize PWM channels timers
    uint32_t reg_cmr_value = TC_CMR_WAVE | TC_CMR_WAVSEL_UP | TC_CMR_EEVT_XC0 | TC_CMR_TCCLKS_TIMER_CLOCK1;
    uint32_t reg_ier_value = TC_IER_CPAS | TC_IER_CPBS | TC_IER_CPCS;
    REG_TC1_CMR0 = REG_TC1_CMR1 = REG_TC1_CMR2 = REG_TC2_CMR0 = REG_TC2_CMR1 = REG_TC2_CMR2 = reg_cmr_value;
    REG_TC1_IER0 = REG_TC1_IER1 = REG_TC1_IER2 = REG_TC2_IER0 = REG_TC2_IER1 = REG_TC2_IER2 = reg_ier_value;
    
    // Initialize buffers
    for (uint32_t i = 0; i < 18; ++i) {
        pwm_channel_ticks[i] = 0;
    }        
	
    // Unlock buffers
    pwm_channels_buffers_state = PWM_BUFFERS_UNLOCK;
	
	// Enable timers IRQ
	NVIC_EnableIRQ(TC0_IRQn);
	NVIC_EnableIRQ(TC3_IRQn);
    NVIC_EnableIRQ(TC4_IRQn);
    NVIC_EnableIRQ(TC5_IRQn);
    NVIC_EnableIRQ(TC6_IRQn);
    NVIC_EnableIRQ(TC7_IRQn);
    NVIC_EnableIRQ(TC8_IRQn);
}

//  ***************************************************************************
/// @brief  PWM enable
/// @note	PWM channels timers auto enable in next PWM cycle
/// @param  none
/// @return none
//  ***************************************************************************
void pwm_enable(void) {

    // Enable sync timer (PWM period)
    REG_TC0_CCR0 = TC_CCR_SWTRG | TC_CCR_CLKEN;
    
    pwm_channels_buffers_state = PWM_BUFFERS_UNLOCK;
}

//  ***************************************************************************
/// @brief  PWM disable
/// @param  none
/// @return none
//  ***************************************************************************
void pwm_disable(void) {
    
    // Disable sync timer (PWM period)
    REG_TC0_CCR0 = TC_CCR_CLKDIS;

    // Disable all PWM channels
    REG_TC1_CCR0 = TC_CCR_CLKDIS;
    REG_TC1_CCR1 = TC_CCR_CLKDIS;
    REG_TC1_CCR2 = TC_CCR_CLKDIS;
    REG_TC2_CCR0 = TC_CCR_CLKDIS;
    REG_TC2_CCR1 = TC_CCR_CLKDIS;
    REG_TC2_CCR2 = TC_CCR_CLKDIS;
}

//  ***************************************************************************
/// @brief  Set PWM buffers state
/// @param  state: new PWM buffers state 
/// @return none
//  ***************************************************************************
void pwm_set_buffers_state(pwm_buffer_state_t state) {
	
	pwm_channels_buffers_state = state;
}
    
//  ***************************************************************************
/// @brief  Set channel pulse width
/// @param  ch: PWM channel index
/// @param  width: pulse width
/// @return counter value
//  ***************************************************************************
void pwm_set_width(uint32_t ch, uint32_t width) {

    pwm_channel_ticks[ch] = PULSE_WIDTH_TO_TICKS(width);
}

//  ***************************************************************************
/// @brief  PWM get sync timer counter
/// @param  none
/// @return counter value
//  ***************************************************************************
uint32_t pwm_get_counter(void) {
    return pwm_period_counter;
}





//  ***************************************************************************
/// @brief  Sync timer ISR
/// @return none
//  ***************************************************************************
void TC0_Handler(void) {

    uint32_t status = REG_TC0_SR0;

    if (status & TC_SR_CPCS) {
		
		++pwm_period_counter;
        
        // Connect all pins to VCC (reset state)
        REG_PIOA_SODR = PWM_ALL_PINS_PORTA;
		REG_PIOB_SODR = PWM_ALL_PINS_PORTB;
        REG_PIOC_SODR = PWM_ALL_PINS_PORTC;
        REG_PIOD_SODR = PWM_ALL_PINS_PORTD;

        // Load pulse width to PWM channels
        if (pwm_channels_buffers_state == PWM_BUFFERS_UNLOCK) {
			
            REG_TC1_RA0 = pwm_channel_ticks[0];
            REG_TC1_RB0 = pwm_channel_ticks[1];
            REG_TC1_RC0 = pwm_channel_ticks[2];
            REG_TC1_RA1 = pwm_channel_ticks[3];
            REG_TC1_RB1 = pwm_channel_ticks[4];
            REG_TC1_RC1 = pwm_channel_ticks[5];
            REG_TC1_RA2 = pwm_channel_ticks[6];
            REG_TC1_RB2 = pwm_channel_ticks[7];
            REG_TC1_RC2 = pwm_channel_ticks[8];
            REG_TC2_RA0 = pwm_channel_ticks[9];
            REG_TC2_RB0 = pwm_channel_ticks[10];
            REG_TC2_RC0 = pwm_channel_ticks[11];
            REG_TC2_RA1 = pwm_channel_ticks[12];
            REG_TC2_RB1 = pwm_channel_ticks[13];
            REG_TC2_RC1 = pwm_channel_ticks[14];
            REG_TC2_RA2 = pwm_channel_ticks[15];
            REG_TC2_RB2 = pwm_channel_ticks[16];
            REG_TC2_RC2 = pwm_channel_ticks[17];
        }        

        // Start PWM cycle
        REG_TC1_CCR0 = TC_CCR_SWTRG | TC_CCR_CLKEN;
        REG_TC1_CCR1 = TC_CCR_SWTRG | TC_CCR_CLKEN;
        REG_TC1_CCR2 = TC_CCR_SWTRG | TC_CCR_CLKEN;
        REG_TC2_CCR0 = TC_CCR_SWTRG | TC_CCR_CLKEN;
        REG_TC2_CCR1 = TC_CCR_SWTRG | TC_CCR_CLKEN;
        REG_TC2_CCR2 = TC_CCR_SWTRG | TC_CCR_CLKEN;
    }
}

//  ***************************************************************************
/// @brief  PWM channels 0-2 ISR
/// @param  none
/// @return none
//  ***************************************************************************
void TC3_Handler(void) {

    uint32_t status = REG_TC1_SR0;

    if (status & TC_SR_CPAS) { REG_PIOC_CODR = PWM_CH0_PIN; }
    if (status & TC_SR_CPBS) { REG_PIOB_CODR = PWM_CH1_PIN; }
    if (status & TC_SR_CPCS) { REG_PIOB_CODR = PWM_CH2_PIN; }
}

//  ***************************************************************************
/// @brief  PWM channels 3-5 ISR
/// @param  none
/// @return none
//  ***************************************************************************
void TC4_Handler(void) {

    uint32_t status = REG_TC1_SR1;

    if (status & TC_SR_CPAS) { REG_PIOC_CODR = PWM_CH3_PIN; }
    if (status & TC_SR_CPBS) { REG_PIOC_CODR = PWM_CH4_PIN; }
    if (status & TC_SR_CPCS) { REG_PIOC_CODR = PWM_CH5_PIN; }
}

//  ***************************************************************************
/// @brief  PWM channels 6-8 ISR
/// @param  none
/// @return none
//  ***************************************************************************
void TC5_Handler(void) {

    uint32_t status = REG_TC1_SR2;

    if (status & TC_SR_CPAS) { REG_PIOC_CODR = PWM_CH6_PIN; }
    if (status & TC_SR_CPBS) { REG_PIOA_CODR = PWM_CH7_PIN; }
    if (status & TC_SR_CPCS) { REG_PIOC_CODR = PWM_CH8_PIN; }
}

//  ***************************************************************************
/// @brief  PWM channels 9-11 ISR
/// @param  none
/// @return none
//  ***************************************************************************
void TC6_Handler(void) {

    uint32_t status = REG_TC2_SR0;

    if (status & TC_SR_CPAS) { REG_PIOD_CODR = PWM_CH9_PIN;  }
    if (status & TC_SR_CPBS) { REG_PIOD_CODR = PWM_CH10_PIN; }
    if (status & TC_SR_CPCS) { REG_PIOD_CODR = PWM_CH11_PIN; }
}

//  ***************************************************************************
/// @brief  PWM channels 12-14 ISR
/// @param  none
/// @return none
//  ***************************************************************************
void TC7_Handler(void) {

    uint32_t status = REG_TC2_SR1;

    if (status & TC_SR_CPAS) { REG_PIOD_CODR = PWM_CH12_PIN; }
    if (status & TC_SR_CPBS) { REG_PIOA_CODR = PWM_CH13_PIN; }
    if (status & TC_SR_CPCS) { REG_PIOC_CODR = PWM_CH14_PIN; }
}

//  ***************************************************************************
/// @brief  PWM channels 15-17 ISR
/// @param  none
/// @return none
//  ***************************************************************************
void TC8_Handler(void) {

    uint32_t status = REG_TC2_SR2;
    
    if (status & TC_SR_CPAS) { REG_PIOC_CODR = PWM_CH15_PIN; }
    if (status & TC_SR_CPBS) { REG_PIOC_CODR = PWM_CH16_PIN; }
    if (status & TC_SR_CPCS) { REG_PIOC_CODR = PWM_CH17_PIN; }
}
