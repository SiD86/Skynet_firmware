//  ***************************************************************************
/// @file    uart.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"

#define UART_BAUDRATE						(115200)
#define UART_PORT							(PIOA)
#define TX_PIN								(PIO_PA9)
#define RX_PIN								(PIO_PA8)


//  ***************************************************************************
/// @brief  UART initialize
/// @param  none
/// @return none
//  ***************************************************************************
void uart_init() {
	
	// Enable UART clock
	REG_PMC_PCER0 = PMC_PCER0_PID8;
	while ((REG_PMC_PCSR0 & PMC_PCER0_PID8) == 0);

	// Configure TX and RX as A peripheral function
	REG_PIOA_PDR = TX_PIN | RX_PIN;		// Disable PIO control, enable peripheral control

	// Disable PDC channels and reset TX and RX
	REG_UART_PTCR = UART_PTCR_TXTDIS | UART_PTCR_RXTDIS;
	REG_UART_CR = UART_CR_RSTTX | UART_CR_RSTRX | UART_CR_RSTSTA;

	// Configure 8N1 mode
	REG_UART_MR = US_MR_CHRL_8_BIT | US_MR_PAR_NO | US_MR_NBSTOP_1_BIT | US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK | US_MR_CHMODE_NORMAL;

	// Configure baudrate
	REG_UART_BRGR = (SystemCoreClock / UART_BAUDRATE) / 16;

	// Disable all interrupts
	REG_UART_IDR = 0xFFFFFFFF;
	
	// Enable TX only
	REG_UART_CR = UART_CR_TXEN | UART_CR_RXDIS;
}

//  ***************************************************************************
/// @brief  UART write data
/// @param  data: pointer to data for write
/// @param  size: bytes count for write
/// @return none
//  ***************************************************************************
void uart_write(uint8_t* data, uint32_t size) {

	// Reset UART
	REG_UART_CR = UART_CR_RSTTX | UART_CR_RSTRX | UART_CR_RSTSTA;
	
	// Enable TX only 
	REG_UART_CR = UART_CR_TXEN | UART_CR_RXDIS;

	while (size != 0) {

		// Wait TXRDY and send byte
		if (REG_UART_SR & UART_SR_TXRDY) {

			REG_UART_THR = *data;

			++data;
			--size;
		}
	}
	
	while ((REG_UART_SR & UART_SR_TXEMPTY) == 0);
}

//  ***************************************************************************
/// @brief  Print debug message
/// @param  msg: message
/// @return none
//  ***************************************************************************
void uart_print_debug_message(char* msg) {
	
	int size = strlen(msg);
	uart_write((uint8_t*)msg, size);
	uart_write((uint8_t*)"\n", 1);
}

//  ***************************************************************************
/// @brief  UART print number
/// @param  number: number
/// @return none
//  ***************************************************************************
void uart_print_number(double number) {
	
	char buffer[32];
	sprintf(buffer, "%f", number);
	
	uart_print_debug_message(buffer);
}

