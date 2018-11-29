//  ***************************************************************************
/// @file    usart0_pdc.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#include "usart0_pdc.h"
#define USART_BAUDRATE						(175000)
#define TX_PIN								(PIO_PA11)
#define RX_PIN								(PIO_PA10)
#define MAX_BUFFER_SIZE						(32)


static uint8_t tx_buffer[MAX_BUFFER_SIZE] = { 0 };
static uint8_t rx_buffer[MAX_BUFFER_SIZE] = { 0 };


//  ***************************************************************************
/// @brief	Initialization USART
/// @note	Mode 8N1
//  ***************************************************************************
void usart0_init() {

	// Enable clock 
	REG_PMC_PCER0 = PMC_PCER0_PID17;
	while ((REG_PMC_PCSR0 & PMC_PCER0_PID17) == 0);
	
	// Configure TX as output (A peripheral function) without pull-up
	REG_PIOA_PER   = TX_PIN;
	REG_PIOA_OER   = TX_PIN;
	REG_PIOA_PUDR  = TX_PIN;
	REG_PIOA_PDR   = TX_PIN;

	// Configure RX as input (A peripheral function) with pull-up
	REG_PIOA_PER   = RX_PIN;
	REG_PIOA_ODR   = RX_PIN;
	REG_PIOA_PUER  = RX_PIN;
	REG_PIOA_PDR   = RX_PIN;

	// Disable PDC channels and reset TX and RX
	REG_USART0_PTCR = US_PTCR_TXTDIS | US_PTCR_RXTDIS;
	REG_USART0_CR = US_CR_RSTTX | US_CR_RSTRX | US_CR_RSTSTA;

	// Configure 8N1 mode
	REG_USART0_MR = US_MR_CHRL_8_BIT | US_MR_PAR_NO | US_MR_NBSTOP_1_BIT | US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK | US_MR_CHMODE_NORMAL;

	// Configure baud rate
	REG_USART0_BRGR = (SystemCoreClock / USART_BAUDRATE) / 16;

	// Disable all interrupts
	REG_USART0_IDR = 0xFFFFFFFF;
	NVIC_EnableIRQ(USART0_IRQn);

	// Configure PDC channels
	REG_USART0_TCR = 0;
	REG_USART0_TPR = (uint32_t)tx_buffer;
	REG_USART0_RCR = 0;
	REG_USART0_RPR = (uint32_t)rx_buffer;

	// Enable TX and RX
	REG_USART0_CR = US_CR_TXEN | US_CR_RXEN;
}

//  ***************************************************************************
/// @brief	Check USART errors
/// @note	Check overrun error, framing error, parity error. Need reset USART
/// @return	true - error, false - no errors
//  ***************************************************************************
bool usart0_is_error() {
	uint32_t reg = REG_USART0_CSR;
	return (reg & (US_CSR_OVRE | US_CSR_FRAME | US_CSR_PARE));
}

//  ***************************************************************************
/// @brief	Reset USART
/// @note	Reset status register, reset transmitter and receiver
/// @param	tx: true - reset TX
/// @param	rx: true - reset RX
//  ***************************************************************************
void usart0_reset(bool tx, bool rx) {

	if (tx == true) {

		// Disable PDC channel and reset TX
		REG_USART0_PTCR = US_PTCR_TXTDIS;
		REG_USART0_CR = US_CR_RSTTX | US_CR_RSTSTA;

		// Reset PDC channel
		REG_USART0_TCR = 0;
		REG_USART0_TPR = (uint32_t)tx_buffer;

		// Enable TX
		REG_USART0_CR = US_CR_TXEN;
	}

	if (rx == true) {

		// Disable PDC channel and reset RX
		REG_USART0_PTCR = US_PTCR_RXTDIS;
		REG_USART0_CR = US_CR_RSTRX | US_CR_RSTSTA;

		// Disable all interrupts
		REG_USART0_IDR = 0xFFFFFFFF;

		// Reset PDC channel
		REG_USART0_RCR = 0;
		REG_USART0_RPR = (uint32_t)rx_buffer;

		// Enable RX
		REG_USART0_CR = US_CR_RXEN;
	}
}

//  ***************************************************************************
/// @brief	Start asynchronous transmit
/// @param	size: bytes count for transmit
//  ***************************************************************************
void usart0_start_tx(uint32_t size) {

	// Initialize DMA for transfer 
	REG_USART0_PTCR = US_PTCR_TXTDIS;
	REG_USART0_TPR = (uint32_t)tx_buffer;
	REG_USART0_TCR = size;
	REG_USART0_PTCR = US_PTCR_TXTEN;
}

//  ***************************************************************************
/// @brief	Check transmit complete
/// @return None
//  ***************************************************************************
bool usart0_is_tx_complete() {
	uint32_t reg = REG_USART0_CSR;
	return (reg & US_CSR_TXEMPTY);
}

//  ***************************************************************************
/// @brief	Get TX buffer address
/// @param	none
/// @return Buffer address
//  ***************************************************************************
uint8_t* usart0_get_tx_buffer_address() {
	return tx_buffer;
}



//  ***************************************************************************
/// @brief	Start asynchronous receive
/// @return	none
//  ***************************************************************************
void usart0_start_rx() {

	// Disable DMA
	REG_USART0_PTCR = US_PTCR_RXTDIS;

	// Initialize frame timeout
	REG_USART0_RTOR = 35 * 8;
	REG_USART0_CR |= US_CR_STTTO;
	REG_USART0_IER = US_IER_TIMEOUT;

	// Initialize DMA for receive
	REG_USART0_RPR = (uint32_t)rx_buffer;
	REG_USART0_RCR = MAX_BUFFER_SIZE;

	// Enable DMA
	REG_USART0_PTCR = US_PTCR_RXTEN;
}

//  ***************************************************************************
/// @brief	Check frame receive complete
/// @return	true - frame received, false - no
//  ***************************************************************************
bool usart0_is_frame_received() {
	uint32_t reg = REG_USART0_CSR;
	return reg & US_CSR_TIMEOUT;
}

//  ***************************************************************************
/// @brief	Get received frame size
/// @return	Frame size
//  ***************************************************************************
uint32_t usart0_get_frame_size() {
	return MAX_BUFFER_SIZE - REG_USART0_RCR;
}

//  ***************************************************************************
/// @brief	Get TX buffer address
/// @return Buffer address
//  ***************************************************************************
uint8_t* usart0_get_rx_buffer_address() {
	return rx_buffer;
}



//  ***************************************************************************
/// @brief	USART0 ISR
/// @note	This for only frame timeout IRQ
//  ***************************************************************************
void USART0_Handler() {
	
	// Frame received - disable DMA and frame timeout IRQ
	REG_USART0_IDR = US_IDR_TIMEOUT;
	REG_USART0_PTCR = US_PTCR_RXTDIS;
}