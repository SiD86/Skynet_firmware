//  ***************************************************************************
/// @file    usart3_pdc.c
/// @author  NeoProg
//  ***************************************************************************
#include <sam.h>
#include <stdbool.h>
#include "usart3_pdc.h"
#define USART_BAUDRATE						(175000)
#define TX_PIN								(PIO_PD4)
#define RX_PIN								(PIO_PD5)
#define MAX_BUFFER_SIZE						(64)


static uint8_t tx_buffer[MAX_BUFFER_SIZE] = { 0 };
static uint8_t rx_buffer[MAX_BUFFER_SIZE] = { 0 };


//  ***************************************************************************
/// @brief	Initialize USART
/// @note	Mode 8N1
//  ***************************************************************************
void usart3_init() {

	// Enable USART3 clock 
	REG_PMC_PCER0 = PMC_PCER0_PID20;
	while ((REG_PMC_PCSR0 & PMC_PCER0_PID20) == 0);

	// Enable PDC clock
	REG_PMC_PCER1 = PMC_PCER1_PID39;
	while ( (REG_PMC_PCSR1 & PMC_PCER1_PID39) == 0 );
	
	// Configure TX as output (B peripheral function) without pull-up
	REG_PIOD_PER   = TX_PIN;
	REG_PIOD_OER   = TX_PIN;
	REG_PIOD_PUDR  = TX_PIN;
	REG_PIOD_PDR   = TX_PIN;
	REG_PIOD_ABSR |= TX_PIN;

	// Configure RX as input (B peripheral function) with pull-up
	REG_PIOD_PER   = RX_PIN;
	REG_PIOD_ODR   = RX_PIN;
	REG_PIOD_PUER  = RX_PIN;
	REG_PIOD_PDR   = RX_PIN;
	REG_PIOD_ABSR |= RX_PIN;

	// Disable PDC channels and reset TX and RX
	REG_USART3_PTCR = US_PTCR_TXTDIS | US_PTCR_RXTDIS;
	REG_USART3_CR = US_CR_RSTTX | US_CR_RSTRX | US_CR_RSTSTA;

	// Configure 8N1 mode
	REG_USART3_MR = US_MR_CHRL_8_BIT | US_MR_PAR_NO | US_MR_NBSTOP_1_BIT | US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK | US_MR_CHMODE_NORMAL;

	// Configure baud rate
	REG_USART3_BRGR = (SystemCoreClock / USART_BAUDRATE) / 4;

	// Disable all interrupts
	REG_USART3_IDR = 0xFFFFFFFF;
	NVIC_EnableIRQ(USART1_IRQn);

	// Configure PDC channels
	REG_USART3_TCR = 0;
	REG_USART3_TPR = (uint32_t)tx_buffer;
	REG_USART3_RCR = 0;
	REG_USART3_RPR = (uint32_t)rx_buffer;

	// Enable TX and RX
	REG_USART3_CR = US_CR_TXEN | US_CR_RXEN;
}

//  ***************************************************************************
/// @brief	Check USART errors
/// @note	Check overrun error, framing error, parity error. Need reset USART
/// @return	true - error, false - no errors
//  ***************************************************************************
bool usart3_is_error() {
	uint32_t reg = REG_USART3_CSR;
	return (reg & (US_CSR_OVRE | US_CSR_FRAME | US_CSR_PARE));
}

//  ***************************************************************************
/// @brief	Reset USART
/// @note	Reset status register, reset transmitter and receiver
/// @param	tx: true - reset TX
/// @param	rx: true - reset RX
//  ***************************************************************************
void usart3_reset(bool tx, bool rx) {

	if (tx == true) {

		// Disable PDC channel and reset TX
		REG_USART3_PTCR = US_PTCR_TXTDIS;
		REG_USART3_CR = US_CR_RSTTX | US_CR_RSTSTA;

		// Reset PDC channel
		REG_USART3_TCR = 0;
		REG_USART3_TPR = (uint32_t)tx_buffer;

		// Enable TX
		REG_USART3_CR = US_CR_TXEN;
	}

	if (rx == true) {

		// Disable PDC channel and reset RX
		REG_USART3_PTCR = US_PTCR_RXTDIS;
		REG_USART3_CR = US_CR_RSTRX | US_CR_RSTSTA;

		// Disable all interrupts
		REG_USART3_IDR = 0xFFFFFFFF;

		// Reset PDC channel
		REG_USART3_RCR = 0;
		REG_USART3_RPR = (uint32_t)rx_buffer;

		// Enable RX
		REG_USART3_CR = US_CR_RXEN;
	}
}

//  ***************************************************************************
/// @brief	Start asynchronous transmit
/// @param	size: bytes count for transmit
//  ***************************************************************************
void usart3_start_tx(uint32_t size) {

	// Initialize DMA for transfer 
	REG_USART3_PTCR = US_PTCR_TXTDIS;
	REG_USART3_TPR = (uint32_t)tx_buffer;
	REG_USART3_TCR = size;
	REG_USART3_PTCR = US_PTCR_TXTEN;
}

//  ***************************************************************************
/// @brief	Check transmit complete
/// @return None
//  ***************************************************************************
bool usart3_is_tx_complete() {
	uint32_t reg = REG_USART3_CSR;
	return (reg & US_CSR_TXEMPTY);
}

//  ***************************************************************************
/// @brief	Get TX buffer address
/// @return Buffer address
//  ***************************************************************************
uint8_t* usart3_get_tx_buffer_address() {
	return tx_buffer;
}



//  ***************************************************************************
/// @brief	Start asynchronous receive
/// @param	clock_speed: I2C_SPEED_100KHZ or I2C_SPEED_400KHZ constants
/// @return	none
//  ***************************************************************************
void usart3_start_rx() {

	// Disable DMA
	REG_USART3_PTCR = US_PTCR_RXTDIS;

	// Initialize frame timeout
	REG_USART3_RTOR = 35;
	REG_USART3_CR |= US_CR_STTTO;
	REG_USART3_IER = US_IER_TIMEOUT;

	// Initialize DMA for receive
	REG_USART3_RPR = (uint32_t)rx_buffer;
	REG_USART3_RCR = MAX_BUFFER_SIZE;

	// Enable DMA
	REG_USART3_PTCR = US_PTCR_RXTEN;
}

//  ***************************************************************************
/// @brief	Check frame receive complete
/// @param	None
/// @return	true - frame received, false - no
//  ***************************************************************************
bool usart3_is_frame_received() {
	return REG_USART3_CSR & US_CSR_TIMEOUT;
}

//  ***************************************************************************
/// @brief	Get received frame size
/// @param	None
/// @return	Frame size
//  ***************************************************************************
uint32_t usart3_get_frame_size() {
	return MAX_BUFFER_SIZE - REG_USART3_RCR;
}

//  ***************************************************************************
/// @brief	Get TX buffer address
/// @return Buffer address
//  ***************************************************************************
uint8_t* usart3_get_rx_buffer_address() {
	return rx_buffer;
}



//  ***************************************************************************
/// @brief	USART3 ISR
/// @note	This for only frame timeout IRQ
/// @return	none
//  ***************************************************************************
void USART3_Handler() {
	
	// // Frame received - disable DMA and frame timeout IRQ
	REG_USART3_IDR = US_IER_TIMEOUT;
	REG_USART3_PTCR = US_PTCR_RXTDIS;
}